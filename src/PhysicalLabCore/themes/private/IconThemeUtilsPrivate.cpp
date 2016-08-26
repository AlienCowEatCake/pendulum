/*
   Copyright (C) 2011-2016,
        Andrei V. Kurochkin     <kurochkin.andrei.v@yandex.ru>
        Mikhail E. Aleksandrov  <alexandroff.m@gmail.com>
        Peter S. Zhigalov       <peter.zhigalov@gmail.com>

   This file is part of the `PhysicalLabCore' library.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "IconThemeUtilsPrivate.h"
#include <QtGlobal>
#include <QFileInfo>
#if defined (Q_OS_WIN)
#include <windows.h>
#include <cstring>
#endif
#include <cstdlib>
#include <cassert>

namespace IconThemeUtils {

/// @brief Получить стандартную иконку Qt
/// @param fallbackPath - путь для иконки по умолчанию
/// @return Стандартная иконка Qt, сконвертированная в QIcon
QIcon GetQtLogo(const QString & fallbackPath)
{
    // Лого Qt, соответствующее версии, с которой скомпилено приложение, можно вытащить из ресурсов QMessageBox
    const QStringList knownPaths = QStringList()
            << QString::fromLatin1(":/trolltech/qmessagebox/images/qtlogo-64.png")
            << QString::fromLatin1(":/qt-project.org/qmessagebox/images/qtlogo-64.png");
    for(QStringList::ConstIterator it = knownPaths.begin(); it != knownPaths.end(); ++it)
    {
        QFileInfo info(*it);
        if(info.exists())
            return QIcon(info.filePath());
    }
    assert(false);
    return fallbackPath.isEmpty() ? QIcon() : QIcon(fallbackPath);
}

/// @brief Получить первую валидную иконку из списка
/// @param[in] iconsList - список иконок
/// @return Первая валидная иконка из списка или пустая иконка, если валидных нет
QIcon GetFirstValidIcon(const QList<QIcon> & iconsList)
{
    for(QList<QIcon>::ConstIterator it = iconsList.begin(); it != iconsList.end(); ++it)
    {
        const QIcon & icon = *it;
        if(!icon.isNull())
            return icon;
    }
    return QIcon();
}

#if defined (Q_OS_WIN)

namespace {

/// @brief Конвертер HBITMAP -> QImage
/// Основан на src/gui/image/qpixmap_win.cpp
QImage fromWinHBITMAP(HDC hdc, HBITMAP bitmap, int w, int h)
{
    BITMAPINFO bmi;
    memset(&bmi, 0, sizeof(bmi));
    bmi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth       = w;
    bmi.bmiHeader.biHeight      = -h;
    bmi.bmiHeader.biPlanes      = 1;
    bmi.bmiHeader.biBitCount    = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    bmi.bmiHeader.biSizeImage   = static_cast<DWORD>(w * h * 4);

    QImage image(w, h, QImage::Format_ARGB32_Premultiplied);
    if(image.isNull())
        return image;

    // Get bitmap bits
    uchar * data = reinterpret_cast<uchar*>(malloc(bmi.bmiHeader.biSizeImage));
    if(GetDIBits(hdc, bitmap, 0, static_cast<UINT>(h), data, &bmi, DIB_RGB_COLORS))
    {
        // Create image and copy data into image.
        for(int y = 0; y < h; ++y)
        {
            void * dest = reinterpret_cast<void*>(image.scanLine(y));
            void * src = data + y * image.bytesPerLine();
            memcpy(dest, src, static_cast<std::size_t>(image.bytesPerLine()));
        }
    }
    else
    {
        qWarning("fromWinHBITMAP(), failed to get bitmap bits");
    }
    free(data);
    return image;
}

/// @brief Конвертер HICON -> QPixmap
/// Основан на src/gui/image/qpixmap_win.cpp
QPixmap fromWinHICON(HICON icon)
{
    bool foundAlpha = false;
    HDC screenDevice = GetDC(0);
    HDC hdc = CreateCompatibleDC(screenDevice);
    ReleaseDC(0, screenDevice);

    ICONINFO iconinfo;
    bool result = GetIconInfo(icon, &iconinfo); //x and y Hotspot describes the icon center
    if(!result)
        qWarning("QPixmap::fromWinHICON(), failed to GetIconInfo()");

    const int w = static_cast<int>(iconinfo.xHotspot * 2);
    const int h = static_cast<int>(iconinfo.yHotspot * 2);

    BITMAPINFOHEADER bitmapInfo;
    bitmapInfo.biSize        = sizeof(BITMAPINFOHEADER);
    bitmapInfo.biWidth       = w;
    bitmapInfo.biHeight      = h;
    bitmapInfo.biPlanes      = 1;
    bitmapInfo.biBitCount    = 32;
    bitmapInfo.biCompression = BI_RGB;
    bitmapInfo.biSizeImage   = 0;
    bitmapInfo.biXPelsPerMeter = 0;
    bitmapInfo.biYPelsPerMeter = 0;
    bitmapInfo.biClrUsed       = 0;
    bitmapInfo.biClrImportant  = 0;
    DWORD * bits;

    HBITMAP winBitmap = CreateDIBSection(hdc, reinterpret_cast<BITMAPINFO*>(&bitmapInfo), DIB_RGB_COLORS,
                                         reinterpret_cast<VOID**>(&bits), NULL, 0);
    HGDIOBJ oldhdc = reinterpret_cast<HBITMAP>(SelectObject(hdc, winBitmap));
    DrawIconEx(hdc, 0, 0, icon, w, h, 0, 0, DI_NORMAL);
    QImage image = fromWinHBITMAP(hdc, winBitmap, w, h);

    for(int y = 0; y < h && !foundAlpha; y++)
    {
        QRgb * scanLine = reinterpret_cast<QRgb*>(image.scanLine(y));
        for(int x = 0; x < w; x++)
        {
            if(qAlpha(scanLine[x]) != 0)
            {
                foundAlpha = true;
                break;
            }
        }
    }
    if(!foundAlpha)
    {
        //If no alpha was found, we use the mask to set alpha values
        DrawIconEx(hdc, 0, 0, icon, w, h, 0, 0, DI_MASK);
        QImage mask = fromWinHBITMAP(hdc, winBitmap, w, h);

        for(int y = 0; y < h; y++)
        {
            QRgb * scanlineImage = reinterpret_cast<QRgb *>(image.scanLine(y));
            QRgb * scanlineMask = mask.isNull() ? 0 : reinterpret_cast<QRgb*>(mask.scanLine(y));
            for(int x = 0; x < w; x++)
            {
                if(scanlineMask && qRed(scanlineMask[x]) != 0)
                    scanlineImage[x] = 0; //mask out this pixel
                else
                    scanlineImage[x] |= 0xff000000; // set the alpha channel to 255
            }
        }
    }
    //dispose resources created by iconinfo call
    DeleteObject(iconinfo.hbmMask);
    DeleteObject(iconinfo.hbmColor);

    SelectObject(hdc, oldhdc); //restore state
    DeleteObject(winBitmap);
    DeleteDC(hdc);
    return QPixmap::fromImage(image);
}

} // namespace

/// @brief Получить иконку из SHELL32.DLL
/// @param[in] index - Индекс иконки (см. enum ShellIcons)
/// @return Иконка из SHELL32.DLL, сконвертированная в QIcon
/// http://www.codeproject.com/Articles/2405/Retrieving-shell-icons
QIcon GetIconFromShell(int index)
{
    HICON smallIcon = NULL, largeIcon = NULL;

    // Shell icons can be customized by the registry:
    // HKLM\SOFTWARE\Microsoft\Windows\CurrentVersion\Explorer\Shell Icons
    // "<ShellIconIndex>" = "<Filename>,<IconIndex>"
    // E.g.
    // "3" = "c:\MyFolderIcon.ico,1"
    HKEY keyShellIcons;
    if(RegOpenKeyExA(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Icons",
                     0, KEY_READ, &keyShellIcons) == ERROR_SUCCESS)
    {
        char buffer[MAX_PATH];
        DWORD size = MAX_PATH;
        if(RegQueryValueExA(keyShellIcons, QString::number(index).toLatin1().data(), NULL, NULL,
                            reinterpret_cast<LPBYTE>(buffer), &size) == ERROR_SUCCESS)
        {
            QStringList value = QString::fromLocal8Bit(buffer).split(QChar::fromLatin1(','));
            ExtractIconExA(value.at(0).toLocal8Bit().data(), atoi(value.at(1).toLocal8Bit().data()),
                           &largeIcon, &smallIcon, 1);
        }
        RegCloseKey(keyShellIcons);
    }

    // Not customized? Then get the original icon from shell23.dll
    if(!smallIcon && !largeIcon)
        ExtractIconExA("SHELL32.DLL", index, &largeIcon, &smallIcon, 1);

    // Сконвертируем в формат Qt
    QIcon icon;
    if(smallIcon)
    {
        icon.addPixmap(fromWinHICON(smallIcon));
        DestroyIcon(smallIcon);
    }
    if(largeIcon)
    {
        icon.addPixmap(fromWinHICON(largeIcon));
        DestroyIcon(largeIcon);
    }
    return icon;
}

#elif !defined (Q_OS_MAC)

/// @brief Получить иконку из системной темы
/// @param matchedList - список имен подходящих иконок
/// @param fallbackPath - путь для иконки по умолчанию
/// @return Первая существующая иконка из списка / иконка по умолчанию / пустая иконка
/// @note Некорректно работает начиная с Qt 5.4.0
QIcon GetIconFromTheme(const QStringList & matchedList, const QString & fallbackPath)
{
#if (QT_VERSION >= QT_VERSION_CHECK(4, 6, 0))
    for(QStringList::ConstIterator it = matchedList.begin(); it != matchedList.end(); ++it)
    {
        QIcon icon = QIcon::fromTheme(*it);
        if(!icon.isNull())
            return icon;
    }
#else
    Q_UNUSED(matchedList);
#endif
    return fallbackPath.isEmpty() ? QIcon() : QIcon(fallbackPath);
}

#endif

} // namespace IconThemeUtils


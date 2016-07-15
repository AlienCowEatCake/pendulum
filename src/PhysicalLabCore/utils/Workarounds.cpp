/*
   Copyright (C) 2011-2016,
        Mikhail Alexandrov  <alexandroff.m@gmail.com>
        Andrey Kurochkin    <andy-717@yandex.ru>
        Peter Zhigalov      <peter.zhigalov@gmail.com>

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

#include "Workarounds.h"

#if defined (Q_OS_WIN) && defined (USE_WIN98_WORKAROUNDS)
#include <windows.h>
#endif
#include <cstdlib>

#include <QApplication>
#include <QList>
#include <QPair>
#include <QFont>
#include <QFontDatabase>

namespace Workarounds {

/// @brief Исправить отображение локализованных шрифтов под Windows
/// @param[in] language - язык, для которого будет проводиться исправление
/// @note Функцию следует вызывать при смене языка
void FontsFix(const QString & language)
{
#if 1// defined (Q_OS_WIN)

    // Отображение название языка -> соответствующая ему WritingSystem
    static QList<QPair<QString, QFontDatabase::WritingSystem> > writingSystemMap =
            QList<QPair<QString, QFontDatabase::WritingSystem> >()
            << qMakePair(QString::fromLatin1("en"), QFontDatabase::Latin)
            << qMakePair(QString::fromLatin1("ru"), QFontDatabase::Cyrillic);

    // Найдем WritingSystem для текущего языка
    QFontDatabase::WritingSystem currentWritingSystem = QFontDatabase::Any;
    for(QList<QPair<QString, QFontDatabase::WritingSystem> >::Iterator it = writingSystemMap.begin(); it != writingSystemMap.end(); ++it)
    {
        if(it->first == language)
        {
            currentWritingSystem = it->second;
            break;
        }
    }

    // Шрифт стандартный, по умолчанию
    static QFont defaultFont = qApp->font();
    // Шрифт Tahoma, если стандартный не поддерживает выбранный язык
    QFont fallbackFont = defaultFont;
    fallbackFont.setFamily(QString::fromLatin1("Tahoma"));

    // Проверим, умеет ли стандартный шрифт писать на новом языке
    static QFontDatabase fontDatabase;
    if(!fontDatabase.families(currentWritingSystem).contains(defaultFont.family(), Qt::CaseInsensitive))
        qApp->setFont(fallbackFont);   // Если не умеет - заменим на Tahoma
    else
        qApp->setFont(defaultFont);    // Если умеет, то вернем его обратно

#if defined (USE_WIN98_WORKAROUNDS)
    // Для Win98 форсированно заменяем шрифты на Tahoma для всех не-английских локалей
    static DWORD dwVersion = (DWORD)(LOBYTE(LOWORD(GetVersion())));
    if(dwVersion <= 4)
    {
        if(language != QString::fromLatin1("en"))
            qApp->setFont(fallbackFont);
        else
            qApp->setFont(defaultFont);
    }
#endif

#else

    Q_UNUSED(language);

#endif
}

/// @brief Автоматически увеличить масштаб отображения для высоких DPI
/// @note Функцию следует вызывать перед созданием QApplication
/// @attention Актуально только для Qt 5.4+
void HighDPIFix()
{
    /// @todo В Qt 5.6+ используется новый механизм, этот deprecated
#if defined (HAVE_GREATER_THAN_OR_EQUALS_QT54)
    static char newEnv[] = "QT_DEVICE_PIXEL_RATIO=auto";
    char * oldEnv = getenv("QT_DEVICE_PIXEL_RATIO");
    if(!oldEnv)
        putenv(newEnv);
#endif
}

} // Workarounds

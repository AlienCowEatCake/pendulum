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

#include "ThemeUtils.h"
#include <QApplication>
#include <QStyle>
#include <QPixmap>
#include <cstdlib>
#include <cassert>
#include "private/IconThemeUtilsPrivate.h"

namespace ThemeUtils {

/// @brief Функция для определения темная используемая тема меню или нет
/// @param[in] menu - Меню, для которого выполняется эта проверка
bool MenuHasDarkTheme(const QMenu *menu)
{
    return menu->palette().color(menu->foregroundRole()).toHsv().value() > 128;
}

/// @brief Создать масштабируемую иконку из нескольких разного размера
/// @param[in] defaultImagePath - Путь к иконке по умолчанию (может быть SVG)
/// @param[in] scaledImagePaths - Список путей иконкам разного размера (растр)
/// @return Масштабируемая иконка
QIcon CreateScalableIcon(const QString& defaultImagePath, const QStringList& scaledImagePaths)
{
    QIcon result(defaultImagePath);
    for(QStringList::ConstIterator it = scaledImagePaths.begin(); it != scaledImagePaths.end(); ++it)
    {
        result.addPixmap(QPixmap(*it), QIcon::Normal);
        result.addPixmap(QPixmap(*it), QIcon::Disabled);
        result.addPixmap(QPixmap(*it), QIcon::Active);
        result.addPixmap(QPixmap(*it), QIcon::Selected);
    }
    return result;
}

namespace {

/// @brief Функция для получения иконки
/// @param[in] type - Тип иконки (см. enum IconTypes)
/// @param[in] darkBackground - true, если иконка располагается на темном фоне
/// @param[in] theme - Тема иконки (см. enum IconThemes)
/// @note Вспомогательная функция для GetIcon
QIcon GetIconHelper(IconTypes type, bool darkBackground, IconThemes theme)
{
    using namespace IconThemeUtils;

    switch(theme)
    {

    case THEME_CLASSIC:
    {
        const QString iconNameTemplate = QString::fromLatin1(":/icons/classic/%1");
        switch(type)
        {
        case ICON_QT:
            return GetQtLogo(iconNameTemplate.arg(QString::fromLatin1("qt.png")));
        case ICON_ABOUT:
            return QIcon(iconNameTemplate.arg(QString::fromLatin1("about.png")));
        case ICON_HELP:
            return QIcon(iconNameTemplate.arg(QString::fromLatin1("help.png")));
        case ICON_AUTHORS:
            return QIcon(iconNameTemplate.arg(QString::fromLatin1("authors.png")));
        case ICON_TEXT:
            return QIcon(iconNameTemplate.arg(QString::fromLatin1("text.png")));
        case ICON_SAVE:
            return QIcon(iconNameTemplate.arg(QString::fromLatin1("save.png")));
        case ICON_SAVE_AS:
            return QIcon(iconNameTemplate.arg(QString::fromLatin1("save-as.png")));
        case ICON_CLOSE:
            return QIcon(iconNameTemplate.arg(QString::fromLatin1("close.png")));
        case ICON_EXIT:
            return QIcon(iconNameTemplate.arg(QString::fromLatin1("exit.png")));
        case ICON_NEW:
        case ICON_NEW_WINDOW:
        case ICON_OPEN:
            return QIcon(); /// @todo
        }
        return QIcon();
    }

    case THEME_MODERN:
    {
        const QString iconNameTemplate = QString::fromLatin1(":/icons/modern/%2_%1.%3")
                .arg(darkBackground ? QString::fromLatin1("white") : QString::fromLatin1("black"));
#if defined (QT_SVG_LIB) && (QT_VERSION >= QT_VERSION_CHECK(5, 4, 0))
        const QString defaultExt = QString::fromLatin1("svg");
#else
        const QString defaultExt = QString::fromLatin1("png");
#endif
        const QString pixmapExt = QString::fromLatin1("png");

        switch(type)
        {
        case ICON_QT:
            return CreateScalableIcon(iconNameTemplate.arg(QString::fromLatin1("icon_qt")).arg(defaultExt),
                                      QStringList(iconNameTemplate.arg(QString::fromLatin1("icon_qt")).arg(pixmapExt)));
        case ICON_ABOUT:
            return CreateScalableIcon(iconNameTemplate.arg(QString::fromLatin1("icon_info")).arg(defaultExt),
                                      QStringList(iconNameTemplate.arg(QString::fromLatin1("icon_info")).arg(pixmapExt)));
        case ICON_HELP:
            return CreateScalableIcon(iconNameTemplate.arg(QString::fromLatin1("icon_help")).arg(defaultExt),
                                      QStringList(iconNameTemplate.arg(QString::fromLatin1("icon_help")).arg(pixmapExt)));
        case ICON_AUTHORS:
            return CreateScalableIcon(iconNameTemplate.arg(QString::fromLatin1("icon_people")).arg(defaultExt),
                                      QStringList(iconNameTemplate.arg(QString::fromLatin1("icon_people")).arg(pixmapExt)));
        case ICON_TEXT:
            return CreateScalableIcon(iconNameTemplate.arg(QString::fromLatin1("icon_text")).arg(defaultExt),
                                      QStringList(iconNameTemplate.arg(QString::fromLatin1("icon_text")).arg(pixmapExt)));
        case ICON_SAVE:
            return CreateScalableIcon(iconNameTemplate.arg(QString::fromLatin1("icon_save")).arg(defaultExt),
                                      QStringList(iconNameTemplate.arg(QString::fromLatin1("icon_save")).arg(pixmapExt)));
        case ICON_SAVE_AS:
            return CreateScalableIcon(iconNameTemplate.arg(QString::fromLatin1("icon_save_as")).arg(defaultExt),
                                      QStringList(iconNameTemplate.arg(QString::fromLatin1("icon_save_as")).arg(pixmapExt)));
        case ICON_CLOSE:
            return CreateScalableIcon(iconNameTemplate.arg(QString::fromLatin1("icon_close")).arg(defaultExt),
                                      QStringList(iconNameTemplate.arg(QString::fromLatin1("icon_close")).arg(pixmapExt)));
        case ICON_EXIT:
            return CreateScalableIcon(iconNameTemplate.arg(QString::fromLatin1("icon_exit")).arg(defaultExt),
                                      QStringList(iconNameTemplate.arg(QString::fromLatin1("icon_exit")).arg(pixmapExt)));
        case ICON_NEW:
            return CreateScalableIcon(iconNameTemplate.arg(QString::fromLatin1("icon_new")).arg(defaultExt),
                                      QStringList(iconNameTemplate.arg(QString::fromLatin1("icon_new")).arg(pixmapExt)));
        case ICON_NEW_WINDOW:
            return CreateScalableIcon(iconNameTemplate.arg(QString::fromLatin1("icon_new_window")).arg(defaultExt),
                                      QStringList(iconNameTemplate.arg(QString::fromLatin1("icon_new_window")).arg(pixmapExt)));
        case ICON_OPEN:
            return CreateScalableIcon(iconNameTemplate.arg(QString::fromLatin1("icon_open")).arg(defaultExt),
                                      QStringList(iconNameTemplate.arg(QString::fromLatin1("icon_open")).arg(pixmapExt)));

        }
        return QIcon();
    }

    case THEME_QT:
    {
        switch(type)
        {
        case ICON_QT:
            return GetQtLogo();
        case ICON_ABOUT:
        case ICON_AUTHORS:
            return QIcon(qApp->style()->standardIcon(QStyle::SP_FileDialogInfoView));
        case ICON_HELP:
            return QIcon(qApp->style()->standardIcon(QStyle::SP_DialogHelpButton));
        case ICON_TEXT:
            return QIcon(qApp->style()->standardIcon(QStyle::SP_FileDialogContentsView));
        case ICON_SAVE:
        case ICON_SAVE_AS:
            return QIcon(qApp->style()->standardIcon(QStyle::SP_DialogSaveButton));
        case ICON_CLOSE:
        case ICON_EXIT:
            return QIcon(qApp->style()->standardIcon(QStyle::SP_DialogCloseButton));
        case ICON_OPEN:
            return QIcon(qApp->style()->standardIcon(QStyle::SP_DialogOpenButton));
        case ICON_NEW:
        case ICON_NEW_WINDOW:
            return QIcon(); /// @todo
        }
    }

    case THEME_SYSTEM:
    {
#if defined (Q_OS_MAC)

    return QIcon();

#elif defined (Q_OS_WIN)

    switch(type)
    {
    case ICON_QT:
        return GetQtLogo();
    case ICON_ABOUT:
        return GetIconFromShell(SI_FAVORITES);
    case ICON_HELP:
        return GetIconFromShell(SI_STARTMENU_HELP);
    case ICON_AUTHORS:
        return GetIconFromShell(160);
    case ICON_TEXT:
        return GetIconFromShell(SI_DEF_DOCUMENT);
    case ICON_OPEN:
        return GetIconFromShell(SI_FOLDER_OPEN);
    case ICON_NEW:
        return GetFirstValidIcon(QList<QIcon>()
                                 << GetIconFromShell(290)
                                 << GetIconFromShell(SI_DEF_DOCUMENT));
    case ICON_NEW_WINDOW:
        return GetIconFromShell(SI_DEF_APPLICATION);
    case ICON_SAVE:
    case ICON_SAVE_AS:
        return GetIconFromShell(258);
    case ICON_CLOSE:
        return GetIconFromShell(132);
    case ICON_EXIT:
        return GetIconFromShell(SI_STARTMENU_SHUTDOWN);
    }

#else

    switch(type)
    {

    case ICON_QT:
        return GetQtLogo();

    case ICON_ABOUT:
        return GetIconFromTheme(QStringList()
                                << QString::fromLatin1("stock_about")
                                << QString::fromLatin1("gtk-about")
                                << QString::fromLatin1("help-about")
                                << QString::fromLatin1("gnome-about-logo"));

    case ICON_HELP:
        return GetIconFromTheme(QStringList()
                                << QString::fromLatin1("stock_help-book")
                                << QString::fromLatin1("stock_help")
                                << QString::fromLatin1("gtk-help")
                                << QString::fromLatin1("help-contents")
                                << QString::fromLatin1("gnome-help")
                                << QString::fromLatin1("browser-help")
                                << QString::fromLatin1("help"));

    case ICON_AUTHORS:
        return GetIconFromTheme(QStringList()
                                << QString::fromLatin1("stock_people")
                                << QString::fromLatin1("emblem-people")
                                << QString::fromLatin1("authors")
                                << QString::fromLatin1("text-x-authors")
                                << QString::fromLatin1("gnome-mime-text-x-authors"));

    case ICON_TEXT:
        return GetIconFromTheme(QStringList()
                                << QString::fromLatin1("gnome-mime-application-x-applix-word")
                                << QString::fromLatin1("gnome-mime-application-msword")
                                << QString::fromLatin1("application-msword")
                                << QString::fromLatin1("text-x-generic"));

    case ICON_SAVE:
        return GetIconFromTheme(QStringList()
                                << QString::fromLatin1("stock_save")
                                << QString::fromLatin1("gtk-save")
                                << QString::fromLatin1("document-save"));

    case ICON_SAVE_AS:
        return GetIconFromTheme(QStringList()
                                << QString::fromLatin1("stock_save-as")
                                << QString::fromLatin1("gtk-save-as")
                                << QString::fromLatin1("document-save-as"));

    case ICON_CLOSE:
        return GetIconFromTheme(QStringList()
                                << QString::fromLatin1("stock_close")
                                << QString::fromLatin1("gtk-close")
                                << QString::fromLatin1("window-close"));

    case ICON_EXIT:
        return GetIconFromTheme(QStringList()
                                << QString::fromLatin1("stock_exit")
                                << QString::fromLatin1("gtk-quit")
                                << QString::fromLatin1("application-exit")
                                << QString::fromLatin1("exit"));

    }

#endif
    }

    }

    assert(false);
    return QIcon();
}

} // namespace

/// @brief Функция для получения иконки
/// @param[in] type - Тип иконки (см. enum IconTypes)
/// @param[in] darkBackground - true, если иконка располагается на темном фоне
/// @param[in] theme - Тема иконки (см. enum IconThemes)
QIcon GetIcon(IconTypes type, bool darkBackground, IconThemes theme)
{
    // Приоритеты: THEME_CLASSIC > THEME_SYSTEM > THEME_QT
    // THEME_MODERN идет независимо, так как сильно отличается по стилю
    QIcon result;
    switch(theme)
    {
    case THEME_MODERN:
        result = GetIconHelper(type, darkBackground, THEME_MODERN);
        if(!result.isNull())
            return result;
        break;
    case THEME_CLASSIC:
        result = GetIconHelper(type, darkBackground, THEME_CLASSIC);
        if(!result.isNull())
            return result;
    case THEME_SYSTEM:
        result = GetIconHelper(type, darkBackground, THEME_SYSTEM);
        if(!result.isNull())
            return result;
    case THEME_QT:
        result = GetIconHelper(type, darkBackground, THEME_QT);
        if(!result.isNull())
            return result;
    }
    return result;
}

} // namespace ThemeUtils


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

#if !defined (PHYSICALLABCORE_ICONTHEMEUTILSPRIVATE_H_INCLUDED)
#define PHYSICALLABCORE_ICONTHEMEUTILSPRIVATE_H_INCLUDED

#include <QtGlobal>
#include <QIcon>
#include <QList>
#include <QString>
#include <QStringList>

namespace IconThemeUtils {

/// @brief Получить стандартную иконку Qt
/// @param fallbackPath - путь для иконки по умолчанию
/// @return Стандартная иконка Qt, сконвертированная в QIcon
QIcon GetQtLogo(const QString & fallbackPath = QString());

/// @brief Получить первую валидную иконку из списка
/// @param[in] iconsList - список иконок
/// @return Первая валидная иконка из списка или пустая иконка, если валидных нет
QIcon GetFirstValidIcon(const QList<QIcon> & iconsList);

#if defined (Q_OS_WIN)

/// @brief Иконки из состава SHELL32.DLL
/// http://www.codeproject.com/Articles/2405/Retrieving-shell-icons
enum ShellIcons
{
    SI_UNKNOWN                      = 0,    // Unknown File Type
    SI_DEF_DOCUMENT                 = 1,    // Default document
    SI_DEF_APPLICATION              = 2,    // Default application
    SI_FOLDER_CLOSED                = 3,    // Closed folder
    SI_FOLDER_OPEN                  = 4,    // Open folder
    SI_FLOPPY_514                   = 5,    // 5 1/4 floppy
    SI_FLOPPY_35                    = 6,    // 3 1/2 floppy
    SI_REMOVABLE                    = 7,    // Removable drive
    SI_HDD                          = 8,    // Hard disk drive
    SI_NETWORKDRIVE                 = 9,    // Network drive
    SI_NETWORKDRIVE_DISCONNECTED    = 10,   // network drive offline
    SI_CDROM                        = 11,   // CD drive
    SI_RAMDISK                      = 12,   // RAM disk
    SI_NETWORK                      = 13,   // Entire network
    SI_UNNAMED_14                   = 14,   // ?
    SI_MYCOMPUTER                   = 15,   // My Computer
    SI_PRINTMANAGER                 = 16,   // Printer Manager
    SI_NETWORK_NEIGHBORHOOD         = 17,   // Network Neighborhood
    SI_NETWORK_WORKGROUP            = 18,   // Network Workgroup
    SI_STARTMENU_PROGRAMS           = 19,   // Start Menu Programs
    SI_STARTMENU_DOCUMENTS          = 20,   // Start Menu Documents
    SI_STARTMENU_SETTINGS           = 21,   // Start Menu Settings
    SI_STARTMENU_FIND               = 22,   // Start Menu Find
    SI_STARTMENU_HELP               = 23,   // Start Menu Help
    SI_STARTMENU_RUN                = 24,   // Start Menu Run
    SI_STARTMENU_SUSPEND            = 25,   // Start Menu Suspend
    SI_STARTMENU_DOCKING            = 26,   // Start Menu Docking
    SI_STARTMENU_SHUTDOWN           = 27,   // Start Menu Shutdown
    SI_SHARE                        = 28,   // Sharing overlay (hand)
    SI_SHORTCUT                     = 29,   // Shortcut overlay (small arrow)
    SI_PRINTER_DEFAULT              = 30,   // Default printer overlay (small tick)
    SI_RECYCLEBIN_EMPTY             = 31,   // Recycle bin empty
    SI_RECYCLEBIN_FULL              = 32,   // Recycle bin full
    SI_DUN                          = 33,   // Dial-up Network Folder
    SI_DESKTOP                      = 34,   // Desktop
    SI_CONTROLPANEL                 = 35,   // Control Panel
    SI_PROGRAMGROUPS                = 36,   // Program Group
    SI_PRINTER                      = 37,   // Printer
    SI_FONT                         = 38,   // Font Folder
    SI_TASKBAR                      = 39,   // Taskbar
    SI_AUDIO_CD                     = 40,   // Audio CD
    SI_UNNAMED_41                   = 41,   // ?
    SI_UNNAMED_42                   = 42,   // ?
    SI_FAVORITES                    = 43,   // IE favorites
    SI_LOGOFF                       = 44,   // Start Menu Logoff
    SI_UNNAMED_45                   = 45,   // ?
    SI_UNNAMED_46                   = 46,   // ?
    SI_LOCK                         = 47,   // Lock
    SI_HIBERNATE                    = 48    // Hibernate
};

/// @brief Получить иконку из SHELL32.DLL
/// @param[in] index - Индекс иконки (см. enum ShellIcons)
/// @return Иконка из SHELL32.DLL, сконвертированная в QIcon
/// http://www.codeproject.com/Articles/2405/Retrieving-shell-icons
QIcon GetIconFromShell(int index);

#elif !defined (Q_OS_MAC)

/// @brief Получить иконку из системной темы
/// @param matchedList - список имен подходящих иконок
/// @param fallbackPath - путь для иконки по умолчанию
/// @return Первая существующая иконка из списка / иконка по умолчанию / пустая иконка
/// @note Некорректно работает начиная с Qt 5.4.0
QIcon GetIconFromTheme(const QStringList & matchedList, const QString & fallbackPath = QString());

#endif

} // namespace IconThemeUtils

#endif // PHYSICALLABCORE_ICONTHEMEUTILSPRIVATE_H_INCLUDED


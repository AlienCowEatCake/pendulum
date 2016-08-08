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

#if !defined (PHYSICALLABCORE_SETTINGSWRAPPER_H_INCLUDED)
#define PHYSICALLABCORE_SETTINGSWRAPPER_H_INCLUDED

#include <QMap>
#include <QString>
#include <QVariant>
#include <QSettings>

/// @brief Класс-обертка над настройками, хранит в себе локальную копию настроек
class SettingsWrapper
{
public:
    SettingsWrapper(const QString &settingsGroup = QString());
    ~SettingsWrapper();

    void setValue(const QString &name, const QVariant &value);
    QVariant value(const QString &key, const QVariant &defaultValue = QVariant()) const;
    void saveAll();
    void reloadAll();

private:
    mutable QSettings m_settings;
    mutable QMap<QString, QVariant> m_settingsCache;
};

#endif // PHYSICALLABCORE_SETTINGSWRAPPER_H_INCLUDED


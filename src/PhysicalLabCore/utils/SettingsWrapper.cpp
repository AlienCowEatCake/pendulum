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

#include "SettingsWrapper.h"
#include <QStringList>

SettingsWrapper::SettingsWrapper(const QString &settingsGroup)
{
    if(!settingsGroup.isEmpty())
        m_settings.beginGroup(settingsGroup);
}

SettingsWrapper::~SettingsWrapper()
{
    saveAll();
}

void SettingsWrapper::setValue(const QString &key, const QVariant &value)
{
    m_settingsCache[key] = value;
    m_settings.sync();
    m_settings.setValue(key, value);
    m_settings.sync();
}

QVariant SettingsWrapper::value(const QString &key, const QVariant &defaultValue) const
{
    QMap<QString, QVariant>::ConstIterator it = m_settingsCache.find(key);
    if(it != m_settingsCache.end())
        return it.value();
    m_settings.sync();
    QVariant value = m_settings.value(key, defaultValue);
    m_settingsCache[key] = value;
    return value;
}

void SettingsWrapper::saveAll()
{
    m_settings.sync();
    for(QMap<QString, QVariant>::ConstIterator it = m_settingsCache.begin(); it != m_settingsCache.end(); ++it)
        m_settings.setValue(it.key(), it.value());
    m_settings.sync();
}

void SettingsWrapper::reloadAll()
{
    m_settings.sync();
    QStringList keys = m_settings.allKeys();
    for(QStringList::ConstIterator it = keys.begin(); it != keys.end(); ++it)
        m_settingsCache[*it] = m_settings.value(*it);
}


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
#include <QMap>
#include <QSettings>
#include <QMutex>
#include <QDebug>
#include <QApplication>

struct SettingsWrapper::SettingsStorage
{
    SettingsStorage()
        : settings(NULL)
    {}

    ~SettingsStorage()
    {
        if(settings)
        {
            saveSettings();
            settings->deleteLater();
        }
    }

    void createSettings()
    {
        settingsMutex.lock();
        if(!settings)
            settings = new QSettings(QSettings::IniFormat, QSettings::UserScope, QApplication::organizationName());
        settingsMutex.unlock();
        qDebug() << settings->fileName();
    }

    void saveSettings()
    {
        settingsMutex.lock();
        for(QMap<QString, QMap<QString, QVariant> >::Iterator group = settingsCache.begin(); group != settingsCache.end(); ++group)
        {
            bool useGroup = !group.key().isEmpty();
            if(useGroup)
                settings->beginGroup(group.key());
            for(QMap<QString, QVariant>::Iterator value = group->begin(); value != group->end(); ++value)
            {
                settings->setValue(value.key(), value.value());
                if(settings->value(value.key()) != value.value())
                    qWarning() << "QSettings::setValue failed for key" << QString::fromLatin1("%1/%2").arg(group.key()).arg(value.key())
                               << "with type" << value.value().typeName();
            }
            if(useGroup)
                settings->endGroup();
        }
        settings->sync();
        settingsMutex.unlock();
    }

    void setValue(const QString &group, const QString &key, const QVariant &value)
    {
        settingsMutex.lock();
        settingsCache[group][key] = value;
        settingsMutex.unlock();
    }

    QVariant value(const QString &group, const QString &key, const QVariant &defaultValue)
    {
        QVariant retValue = defaultValue;
        bool foundInCache = false;
        settingsMutex.lock();
        QMap<QString, QMap<QString, QVariant> >::Iterator groupIter = settingsCache.find(group);
        if(groupIter != settingsCache.end())
        {
            QMap<QString, QVariant>::Iterator valueIter = groupIter->find(key);
            if(valueIter != groupIter->end())
            {
                retValue = valueIter.value();
                foundInCache = true;
            }
        }
        if(!foundInCache)
        {
            settings->beginGroup(group);
            QVariant newValue = settings->value(key, defaultValue);
            settings->endGroup();
            if(newValue.isValid())
            {
                settingsCache[group][key] = newValue;
                retValue = newValue;
            }
        }
        settingsMutex.unlock();
        return retValue;
    }

    QMutex settingsMutex;
    QSettings* settings;
    QMap<QString, QMap<QString, QVariant> > settingsCache;
};

SettingsWrapper::SettingsStorage SettingsWrapper::g_settingsStorage;


SettingsWrapper::SettingsWrapper(const QString &settingsGroup)
    : m_settingsGroup(settingsGroup)
{
    g_settingsStorage.createSettings();
}

SettingsWrapper::~SettingsWrapper()
{}

void SettingsWrapper::setValue(const QString &key, const QVariant &value)
{
    g_settingsStorage.setValue(m_settingsGroup, key, value);
}

QVariant SettingsWrapper::value(const QString &key, const QVariant &defaultValue) const
{
    return g_settingsStorage.value(m_settingsGroup, key, defaultValue);
}


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
#include <QVariantMap>
#include <QSettings>
#include <QMutex>
#include <QApplication>
#include <QDir>

#if defined (Q_OS_MAC)

namespace NativeSettingsStorage {

/// @brief Установить значение для заданного ключа в NSUserDefaults
/// @param[in] group - группа (секция) настроек
/// @param[in] key - ключ, для которого устанавливается значение
/// @param[in] value - значение, которое устанавливается для ключа
/// @note Реализация в SettingsWrapper_mac.mm
void setValue(const QString &group, const QString &key, const QVariant &value);

/// @brief Получить значение для заданного ключа из NSUserDefaults
/// @param[in] group - группа (секция) настроек
/// @param[in] key - ключ, для которого получается значение
/// @param[in] defaultValue - умолчательное значение, возвращается при отсутствии значения
/// @return - значение для ключа или defaultValue при отсутствии значения
/// @note Реализация в SettingsWrapper_mac.mm
QVariant value(const QString &group, const QString &key, const QVariant &defaultValue);

} // namespace NativeSettingsStorage

#endif

namespace {

#if defined (Q_OS_MAC)

/// @brief Хранилище настроек для Mac, использует NSUserDefaults. Введен по причине глючности QSettings.
class SettingsStorage
{
public:
    /// @brief Установить значение для заданного ключа в NSUserDefaults
    /// @param[in] group - группа (секция) настроек
    /// @param[in] key - ключ, для которого устанавливается значение
    /// @param[in] value - значение, которое устанавливается для ключа
    void setValue(const QString &group, const QString &key, const QVariant &value)
    {
        NativeSettingsStorage::setValue(group, key, value);
    }

    /// @brief Получить значение для заданного ключа из NSUserDefaults
    /// @param[in] group - группа (секция) настроек
    /// @param[in] key - ключ, для которого получается значение
    /// @param[in] defaultValue - умолчательное значение, возвращается при отсутствии значения
    /// @return - значение для ключа или defaultValue при отсутствии значения
    QVariant value(const QString &group, const QString &key, const QVariant &defaultValue)
    {
        return NativeSettingsStorage::value(group, key, defaultValue);
    }
};

#else

/// @brief Универсальное хранилище настроек, использует QSettings.
class SettingsStorage
{
public:
    SettingsStorage()
        : m_settings(NULL)
    {}

    ~SettingsStorage()
    {
        if(m_settings)
        {
            m_settings->sync();
            m_settings->deleteLater();
        }
    }

    /// @brief Установить значение для заданного ключа в QSettings
    /// @param[in] group - группа (секция) настроек
    /// @param[in] key - ключ, для которого устанавливается значение
    /// @param[in] value - значение, которое устанавливается для ключа
    void setValue(const QString &group, const QString &key, const QVariant &value)
    {
        if(!m_settings)
            initStorage();
        bool useGroup = !group.isEmpty();
        if(useGroup)
            m_settings->beginGroup(group);
        m_settings->setValue(key, value);
        if(useGroup)
            m_settings->endGroup();
    }

    /// @brief Получить значение для заданного ключа из QSettings
    /// @param[in] group - группа (секция) настроек
    /// @param[in] key - ключ, для которого получается значение
    /// @param[in] defaultValue - умолчательное значение, возвращается при отсутствии значения
    /// @return - значение для ключа или defaultValue при отсутствии значения
    QVariant value(const QString &group, const QString &key, const QVariant &defaultValue)
    {
        if(!m_settings)
            initStorage();
        m_settings->beginGroup(group);
        QVariant value = m_settings->value(key, defaultValue);
        m_settings->endGroup();
        return value;
    }

private:
    /// @brief Инициализация QSettings
    void initStorage()
    {
        if(!m_settings)
        {
#if defined (Q_OS_MAC)
            const QSettings::Format format = QSettings::IniFormat;
            const QString organizationName = QApplication::organizationName();
            const QString applicationName = QApplication::applicationName();
            QSettings::setPath(format, QSettings::UserScope,
                               QString::fromLatin1("%1/Library/Application Support").arg(QDir::homePath()));
#else
            const QSettings::Format format = QSettings::NativeFormat;
            const QString organizationName = QApplication::organizationName();
            const QString applicationName = QApplication::applicationName();
#endif
            m_settings = new QSettings(format, QSettings::UserScope, organizationName, applicationName);
            m_settings->setFallbacksEnabled(false);
        }
    }

    QSettings* m_settings;
};

#endif

} // namespace

/// @brief Кэш настроек
class SettingsWrapper::SettingsCache
{
public:
    SettingsCache()
    {}

    ~SettingsCache()
    {
        saveSettings();
    }

    /// @brief Установить значение для заданного ключа
    /// @param[in] group - группа (секция) настроек
    /// @param[in] key - ключ, для которого устанавливается значение
    /// @param[in] value - значение, которое устанавливается для ключа
    void setValue(const QString &group, const QString &key, const QVariant &value)
    {
        m_settingsMutex.lock();
        m_settingsCache[group][key] = value;
        m_settingsMutex.unlock();
    }

    /// @brief Получить значение для заданного ключа
    /// @param[in] group - группа (секция) настроек
    /// @param[in] key - ключ, для которого получается значение
    /// @param[in] defaultValue - умолчательное значение, возвращается при отсутствии значения
    /// @return - значение для ключа или defaultValue при отсутствии значения
    QVariant value(const QString &group, const QString &key, const QVariant &defaultValue)
    {
        QVariant retValue = defaultValue;
        bool foundInCache = false;
        m_settingsMutex.lock();
        QMap<QString, QVariantMap>::Iterator groupIter = m_settingsCache.find(group);
        if(groupIter != m_settingsCache.end())
        {
            QVariantMap::Iterator valueIter = groupIter->find(key);
            if(valueIter != groupIter->end())
            {
                retValue = valueIter.value();
                foundInCache = true;
            }
        }
        if(!foundInCache)
        {
            QVariant newValue = m_settingsStorage.value(group, key, defaultValue);
            if(newValue.isValid())
            {
                m_settingsCache[group][key] = newValue;
                retValue = newValue;
            }
        }
        m_settingsMutex.unlock();
        return retValue;
    }

private:
    /// @brief Сброс настроек в SettingsStorage
    void saveSettings()
    {
        m_settingsMutex.lock();
        for(QMap<QString, QVariantMap>::Iterator group = m_settingsCache.begin(); group != m_settingsCache.end(); ++group)
        {
            for(QVariantMap::Iterator value = group->begin(); value != group->end(); ++value)
                m_settingsStorage.setValue(group.key(), value.key(), value.value());
        }
        m_settingsMutex.unlock();
    }

    /// @brief Мьютекс, блокирующий доступ к кэшу из разных потоков
    QMutex m_settingsMutex;
    /// @brief Платформозависимое хранилище настроек
    SettingsStorage m_settingsStorage;
    /// @brief Кэш настроек, QMap<Group, QMap<Key, Value>>
    QMap<QString, QVariantMap> m_settingsCache;
};

/// @brief Глобальный кэш настроек
SettingsWrapper::SettingsCache SettingsWrapper::g_settingsCache;


/// @brief SettingsWrapper
/// @param[in] settingsGroup - группа (секция) настроек
SettingsWrapper::SettingsWrapper(const QString &settingsGroup)
    : m_settingsGroup(settingsGroup)
{}

SettingsWrapper::~SettingsWrapper()
{}

/// @brief Установить значение для заданного ключа
/// @param[in] key - ключ, для которого устанавливается значение
/// @param[in] value - значение, которое устанавливается для ключа
void SettingsWrapper::setValue(const QString &key, const QVariant &value)
{
    g_settingsCache.setValue(m_settingsGroup, key, value);
}

/// @brief Получить значение для заданного ключа
/// @param[in] key - ключ, для которого получается значение
/// @param[in] defaultValue - умолчательное значение, возвращается при отсутствии значения
/// @return - значение для ключа или defaultValue при отсутствии значения
QVariant SettingsWrapper::value(const QString &key, const QVariant &defaultValue) const
{
    return g_settingsCache.value(m_settingsGroup, key, defaultValue);
}


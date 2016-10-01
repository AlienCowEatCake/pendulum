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

#import <Foundation/Foundation.h>
#include <QString>
#include <QVariant>

namespace NativeSettingsStorage {

namespace {

QString getNativeKeyString(const QString &group, const QString &key)
{
    return (group.isEmpty() ? QString() : (group + QString::fromLatin1("/"))) + key;
}

} // namespace

void setValue(const QString &group, const QString &key, const QVariant &value)
{
    NSString *nativeKey = [NSString stringWithUTF8String: getNativeKeyString(group, key).toUtf8().data()];
    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
    NSString *nativeValue = [NSString stringWithUTF8String: value.toByteArray().toHex().data()];
    [defaults setObject: nativeValue forKey: nativeKey];
    [defaults synchronize];
}

QVariant value(const QString &group, const QString &key, const QVariant &defaultValue)
{
    NSString *nativeKey = [NSString stringWithUTF8String: getNativeKeyString(group, key).toUtf8().data()];
    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
    QString value = QString::fromNSString([defaults stringForKey: nativeKey]);
    if(value.isEmpty())
        return defaultValue;
    QVariant variantValue = QVariant(QByteArray::fromHex(value.toLatin1()));
    if(variantValue.isValid())
        return variantValue;
    return defaultValue;
}

} // namespace NativeSettingsStorage


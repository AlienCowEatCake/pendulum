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

#if !defined (PHYSICALLABCORE_IMAGESAVER_H_INCLUDED)
#define PHYSICALLABCORE_IMAGESAVER_H_INCLUDED

#include <QWidget>
#include <QString>
#include <QImage>

class ImageSaver: public QObject
{
    Q_OBJECT

public:
    ImageSaver(QWidget * parent = NULL);
    ~ImageSaver();

    QString defaultName() const;
    void setDefaultName(const QString & defaultName);

    bool save(const QImage & image);

private:
    QWidget * m_parent;
    QString m_defaultName;
    QString m_lastSavedName;
};

#endif // PHYSICALLABCORE_IMAGESAVER_H_INCLUDED


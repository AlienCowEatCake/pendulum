/* 
   Copyright (C) 2011-2015,
        Mikhail Alexandrov  <alexandroff.m@gmail.com>
        Andrey Kurochkin    <andy-717@yandex.ru>
        Peter Zhigalov      <peter.zhigalov@gmail.com>

   This file is part of the `pendulum' program.

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

#ifndef CSCENE3D_H
#define CSCENE3D_H

/* класс 3D сцены в главном окне*/

#include "CModelSpring.h"
#include "widgets/CScene3DAbstract/CScene3DAbstract.h"

class Cscene3D : public CScene3DAbstract
{
    Q_OBJECT

private:
    //модели
    CMilkshapeModel tripod, sphere, scale, spring_start, spring_end, strng;
    CModelSpring cyll;

protected:
    void paintGL();
    void initializeGL();
    void timerEvent(QTimerEvent *); // обработка события таймера

public:
    Cscene3D(QWidget* parent = 0);

public slots:
    void actiontime();

};

#endif // CSCENE3D_H

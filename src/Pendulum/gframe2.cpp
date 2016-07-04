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

#include "gframe2.h"
#include "main.h"

gframe2::gframe2(QWidget *parent) :
    CGraphWindowAbstract(false, parent)
{
    setLabels(trUtf8("Механическая энергия"), trUtf8("t, c"), trUtf8("E, Дж"));
    update();
}

void gframe2::update()
{
    QVector<float> & arrX = this->arrX();
    QVector<float> & arrY = this->arrY();

    iter.r = m_action.r;
    iter.m = m_action.m;
    iter.k = m_action.k;
    iter.A0 = m_action.A0;
    iter.InitBall();

    double mactiont;
    if(iter.r != 0 && iter.A0 != 0)
        mactiont = log(iter.E0 / 0.001)/(2.0 * iter.sigma);
    else
        mactiont = 1.0;
    resizeGraph(0.0f, (float)mactiont, 0, (float)iter.E0);

    float di=(float)(mactiont * 1000.0 / 50.0);
    for(float i = 0.0; i <= mactiont * 1000.0; i += di)
    {
        iter.Refresh(i);
        arrX.push_back((float)(i / 1000.0) * scaleX());
        arrY.push_back((float)iter.E * scaleY());
    }

    repaint();
}

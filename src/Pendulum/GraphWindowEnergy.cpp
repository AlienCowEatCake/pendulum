/* 
   Copyright (C) 2011-2016,
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

#include "GraphWindowEnergy.h"

#include <cmath>
#include "PhysicalController.h"

GraphWindowEnergy::GraphWindowEnergy(QWidget * parent) :
    GraphWindowAbstract(false, parent), m_physicalController(NULL)
{
    setLabels(trUtf8("Механическая энергия"), trUtf8("t, c"), trUtf8("E, Дж"));
}

void GraphWindowEnergy::setPhysicalController(const PhysicalController * physicalController)
{
    m_physicalController = physicalController;
}

void GraphWindowEnergy::update()
{
    if(!m_physicalController)
        return;

    QVector<float> & arrX = this->arrX();
    QVector<float> & arrY = this->arrY();
    Action action(m_physicalController->action());

    action.InitBall();

    double mactiont;
    if(action.get_r() != 0.0 && action.get_A0() != 0.0)
        mactiont = std::log(action.get_E0() / 0.001)/(2.0 * action.get_sigma());
    else
        mactiont = 1.0;
    resizeGraph(0.0f, static_cast<float>(mactiont), 0, static_cast<float>(action.get_E0()));

    double di = mactiont * 1000.0 / 50.0;
    for(double i = 0.0; i <= mactiont * 1000.0; i += di)
    {
        action.Refresh(i);
        arrX.push_back(static_cast<float>(i / 1000.0) * scaleX());
        arrY.push_back(static_cast<float>(action.get_E()) * scaleY());
    }

    repaint();
}


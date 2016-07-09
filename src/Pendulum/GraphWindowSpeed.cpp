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

#include "GraphWindowSpeed.h"

#include <cmath>
#include "PhysicalController.h"

GraphWindowSpeed::GraphWindowSpeed(QWidget * parent) :
    GraphWindowAbstract(true, parent), m_physicalController(NULL)
{
    setLabels(trUtf8("Скорость"), trUtf8("t, c"), trUtf8("v, м/с"));
    m_numT = 3;
}

void GraphWindowSpeed::setPhysicalController(const PhysicalController * physicalController)
{
    m_physicalController = physicalController;
}

void GraphWindowSpeed::update()
{
    if(!m_physicalController)
        return;

    QVector<float> & arrX = this->arrX();
    QVector<float> & arrY = this->arrY();
    Action action(m_physicalController->action());

    if(action.get_w0() <= action.get_sigma() && std::fabs(action.get_A0()) < 0.0001)
    {
        arrX.push_back(0.0f);
        arrX.push_back(1.0f);
        arrY.push_back(0.0f);
        arrY.push_back(0.0f);

        resizeGraph(0.0f, 1.0f, -1.0f, 1.0f);
    }
    else
    {
        double mactiont;
        if(action.get_w0() <= action.get_sigma())
            mactiont = -log(fabs(0.0000001/action.get_A0())) / (m_numT * action.get_sigma());
        else
            mactiont = action.get_T();
        action.InitBall();
        double maxspeed = 0.0;

        double di = mactiont / 50.0 * 1000.0;
        for(double i = 0.0; i <= mactiont * m_numT * 1000.0; i += di)
        {
            action.Refresh(i);
            arrX.push_back(static_cast<float>(i / 1000.0));
            arrY.push_back(static_cast<float>(action.get_v()));
            if(std::fabs(action.get_v()) > maxspeed)
                maxspeed = std::fabs(action.get_v());
        }

        float maxspeedf = static_cast<float>(maxspeed);
        resizeGraph(0.0f, static_cast<float>(mactiont) * static_cast<float>(m_numT), -maxspeedf, maxspeedf);

        for(int i = 0; i <  arrY.size(); i++)
        {
            arrX[i] *= scaleX();
            arrY[i] *= scaleY();
        }
    }

    repaint();
}


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

#include "gframe.h"
#include "ui_gframe.h"
#include "main.h"



gframe::gframe(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::gframe)
{
    ui->setupUi(this);
    setCentralWidget(ui->widget);   
    ui->widget->axisX = trUtf8("t, c");
    ui->widget->axisY = trUtf8("v, м/с");
    numT = 3;
    update();
}

gframe::~gframe()
{    
    delete ui;   
}

void gframe::update()
{
    if(m_action.w0 <= m_action.sigma && fabs(m_action.A0) < 0.0001)
    {
        ui->widget->masX.push_back(0.0f);
        ui->widget->masX.push_back(1.0f);
        ui->widget->masY.push_back(0.0f);
        ui->widget->masY.push_back(0.0f);

        ui->widget->resize(0.0f, 1.0f, -1.0f, 1.0f);
    }
    else
    {
        double mactiont;
        if(m_action.w0 <= m_action.sigma)
            mactiont = -log(fabs(0.0000001/m_action.A0))/(numT*m_action.sigma);
        else
            mactiont = m_action.T;

        iter.r = m_action.r;
        iter.m = m_action.m;
        iter.k = m_action.k;
        iter.A0 = m_action.A0;
        iter.InitBall();
        double maxspeed = 0.0;

        float di = (float)(mactiont / 50.0 * 1000.0);
        for(float i = 0.0f; i <= (float)(mactiont * numT * 1000.0); i += di)
        {
            iter.Refresh(i);
            ui->widget->masX.push_back((float)(i / 1000.0));
            ui->widget->masY.push_back((float)iter.v);
            if(fabs(iter.v) > maxspeed)
                maxspeed = fabs(iter.v);
        }

        ui->widget->resize(0.0f, (float)mactiont * (float)numT, -(float)maxspeed, (float)maxspeed);

        for(size_t i = 0; i <  ui->widget->masY.size(); i++)
        {
            ui->widget->masX[i] *= ui->widget->scale_x;
            ui->widget->masY[i] *= ui->widget->scale_y;
        }
    }

    repaint();
}

void gframe::clear()
{
    ui->widget->masX.clear();
    ui->widget->masY.clear();
}



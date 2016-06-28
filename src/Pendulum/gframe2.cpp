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
#include "ui_gframe2.h"
#include "main.h"

gframe2::gframe2(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::gframe2)
{
    ui->setupUi(this);
    setCentralWidget(ui->widget);
    ui->widget->axisX = trUtf8("t, c");
    ui->widget->axisY = trUtf8("E, Дж");
    update();
}

gframe2::~gframe2()
{
    delete ui;
}

void gframe2::update()
{
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
    ui->widget->resize(0.0f, (float)mactiont, -(float)iter.E0, (float)iter.E0);

    float di=(float)(mactiont * 1000.0 / 50.0);
    for(float i = 0.0; i <= mactiont * 1000.0; i += di)
    {
        iter.Refresh(i);
        ui->widget->masX.push_back((float)(i / 1000.0) * ui->widget->scale_x);
        ui->widget->masY.push_back((float)iter.E * ui->widget->scale_y);
    }

    repaint();
}

void gframe2::clear()
{
    ui->widget->masX.clear();
    ui->widget->masY.clear();
}


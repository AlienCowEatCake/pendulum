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

#if defined(HAVE_QT5)
#include <QtWidgets>
#else
#include <QtGui>
#endif
#include "cscene2dn.h"
#include "main.h"
#include <cmath>

Cscene2dn::Cscene2dn(QWidget* parent, bool haveNegY) : QWidget(parent)
{
    haveNegativeY = haveNegY;
    // Мин/макс значения рисуемой области в локальных координатах
    min_x_loc = 0.0f;
    (haveNegativeY ? min_y_loc = -1.0f : min_y_loc = 0.0f);
    max_x_loc = 1.0f;
    max_y_loc = 1.0f;
    // Локальный размер рисуемой области без учета осей, подписей и прочего
    size_x_loc = max_x_loc - min_x_loc;
    size_y_loc = max_y_loc - min_y_loc;
    //Цвет и ширина для всех линий графиков
    grid_s_col = Qt::lightGray;
    grid_b_col = Qt::gray;
    axis_col = Qt::black;
    plot_col = Qt::black;
    grid_s_width = 1;
    grid_b_width = 1.5;
    axis_width = 3;
    plot_width = 2;
}

// Подгонка минимальных/максимальных значений и числа линий, для нормальной рисовки сетки
void Cscene2dn::adjustAxis(float & min, float & max, int & numTicks)
{
    const float axis_epsilon = 1.0f / 10000.0f;
    if(max - min < axis_epsilon)
    {
        min -= 2.0f * axis_epsilon;
        max += 2.0f * axis_epsilon;
    }

    const int MinTicks = 6;
    double grossStep = (max - min) / MinTicks;
    double step = pow(10, floor(log10(grossStep)));

    if (5 * step < grossStep)
        step *= 5;
    else if (2 * step < grossStep)
        step *= 2;

    numTicks = (int)(ceil(max / step) - floor(min / step));
    min = floor(min / step) * step;
    max = ceil(max / step) * step;
}

// Событие при изменении размера виджета
void Cscene2dn::resize(float x0, float x1, float y0, float y1)
{
    min_x = x0;
    min_y = y0;
    max_x = x1;
    max_y = y1;

    // Поправляем значения мин / макс чтобы влазило в сетку
    adjustAxis(min_x, max_x, num_ticks_x);
    adjustAxis(min_y, max_y, num_ticks_y);
    size_x = max_x - min_x;
    size_y = max_y - min_y;

    scale_x = size_x_loc / size_x;
    scale_y = size_y_loc / size_y;
}

// Функция перевода координат сцены в координаты окна
QPoint Cscene2dn::to_window(float x, float y) const
{
    // В OpenGL это был бы glOrtho
    float gl_x0, gl_y0, gl_x1, gl_y1;
    gl_x0 = -0.06f;
    gl_x1 = 1.012f;
    if(haveNegativeY)
    {
        gl_y0 = -1.03f;
        gl_y1 = 1.03f;
    }
    else
    {
        gl_y0 = -0.07f;
        gl_y1 = 1.02f;
    }
    const float gl_hx = gl_x1 - gl_x0;
    const float gl_hy = gl_y1 - gl_y0;
    // Перевод
    int xl = (int)((x - gl_x0) / gl_hx * (float)width());
    int yl = height() - (int)((y - gl_y0) / gl_hy * (float)height());
    return QPoint(xl, yl);
}

// Событие рисования
void Cscene2dn::paintEvent(QPaintEvent * event)
{
    QPainter painter;
    painter.begin(this);
    painter.setViewport(0, 0, this->width(), this->height());
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    painter.fillRect(event->rect(), QBrush(Qt::white));

    // координатная сетка
    painter.setPen(QPen(grid_s_col, grid_s_width));
    for(int i = 0; i <= num_ticks_x * 5; i++)
    {
        float x = (float)i / (float)num_ticks_x * size_x_loc;
        x *= 0.2f;
        x += min_x_loc;
        painter.drawLine(to_window(x, min_y_loc), to_window(x, max_x_loc));
    }
    for(int i = 0; i <= num_ticks_y * 5; i++)
    {
        float y = (float)i / (float)num_ticks_y * size_y_loc;
        y *= 0.2f;
        y += min_y_loc;
        painter.drawLine(to_window(min_x_loc, y), to_window(max_x_loc, y));
    }
    painter.setPen(QPen(grid_b_col, grid_b_width));
    for(int i = 0; i <= num_ticks_x; i++)
    {
        float x = (float)i / (float)num_ticks_x * size_x_loc;
        x += min_x_loc;
        painter.drawLine(to_window(x, min_y_loc), to_window(x, max_x_loc));
    }
    for(int i = 0; i <= num_ticks_y; i++)
    {
        float y = (float)i / (float)num_ticks_y * size_y_loc;
        y += min_y_loc;
        painter.drawLine(to_window(min_x_loc, y), to_window(max_x_loc, y));
    }

    // Координатные оси
    painter.setPen(QPen(axis_col, axis_width));
    if(haveNegativeY)
        painter.drawLine(to_window(0.0f, -1.0f), to_window(0.0f, 1.0f));
    else
        painter.drawLine(to_window(0.0f, 0.0f), to_window(0.0f, 1.0f));
    painter.drawLine(to_window(0.0f, 0.0f), to_window(1.0f, 0.0f));
   

    // Отрисовка шкалы
#if defined _WIN32 && defined HAVE_QT5 // Почему-то в Qt5 под Win гигантские шрифты
    QFont serifFont("Times", 9);
#else
    QFont serifFont("Times", 11);
#endif
    painter.setFont(serifFont);
    painter.setPen(QPen(axis_col));
    for(int i = 1; i < num_ticks_x; i++)
    {
        float x = (float)i / (float)num_ticks_x;
        float x_real = (float)(floor((x * size_x + min_x) * 1e5 + 0.5)) * 1e-5;
        x = x * size_x_loc + min_x_loc;
        QString st = QString::number(x_real);
        if(haveNegativeY)
            painter.drawText(to_window(x - 0.01f, -0.06f), st);
        else
            painter.drawText(to_window(x - 0.01f, -0.04f), st);
    }
    for(int i = 0; i < num_ticks_y; i++)
    {
        float y = (float)i / (float)num_ticks_y;
        float y_real = (float)(floor((y * size_y + min_y) * 1e5 + 0.5)) * 1e-5;
        y = y * size_y_loc + min_y_loc;
        QString st = QString("%1").arg(y_real, 5, 'g', -1, ' ');
        painter.drawText(to_window(-0.05f, y - 0.01f), st);
    }

    // Подписи осей
    serifFont.setBold(true);
    painter.setFont(serifFont);
    if(haveNegativeY)
    {
        painter.drawText(to_window(0.97f, -0.06f), axisX);
        painter.drawText(to_window(-0.05f, 0.955f), axisY);
    }
    else
    {
        painter.drawText(to_window(0.97f, -0.04f), axisX);
        painter.drawText(to_window(-0.05f, 0.98f), axisY);
    }
    

    // Отрисовка графика
    painter.setPen(QPen(plot_col, plot_width));
    size_t maslength = masX.size();
    for(size_t i = 1; i < maslength; i++)
        painter.drawLine(to_window(masX[i-1], masY[i-1]), to_window(masX[i], masY[i]));

    painter.end();
}

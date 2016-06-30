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

#ifndef CSCENE2DN_H
#define CSCENE2DN_H

#include <QString>
#include <QWidget>
#include <vector>

using namespace std;

class Cscene2dn : public QWidget
{
    Q_OBJECT

public:
    Cscene2dn(QWidget* parent = 0, bool haveNegY = true);
    // Подписи осей
    QString axisX, axisY;
    // Сами значения для графика
    vector<float> masX, masY;
    // Событие при изменении размера виджета
    void resize(float x0, float x1, float y0, float y1);
    // Коэффициенты масштабирования по осям x и y
    float scale_x, scale_y;
    //Интерфейсы для кастомизации отображения
    void setGridSCol(QColor col){grid_s_col = col;}
    void setGridBCol(QColor col){grid_b_col = col;}
    void setAxisCol(QColor col){axis_col = col;}
    void setPlotCol(QColor col){plot_col = col;}
    void setGridSWidth(qreal width){grid_s_width = width;}
    void setGridBWidth(qreal width){grid_b_width = width;}
    void setAxisWidth(qreal width){axis_width = width;}
    void setPlotWidth(qreal width){plot_width = width;}

protected:
    // Событие рисования
    void paintEvent(QPaintEvent *event);

private:
    // Подгонка минимальных/максимальных значений и числа линий, для нормальной рисовки сетки
    void adjustAxis(float & min, float & max, int & numTicks);
    // Тип отображения графика
    bool haveNegativeY;
    // Минимальные и максимальные значения в глобальных координатах
    float max_x, max_y;
    float min_x, min_y;
    // Число линий сетки
    int num_ticks_x, num_ticks_y;
    // Глобальный размер рисуемой области без учета осей, подписей и прочего
    float size_x, size_y;
    // Локальный размер рисуемой области без учета осей, подписей и прочего
    float size_x_loc, size_y_loc;
    // Мин/макс значения рисуемой области в локальных координатах
    float min_x_loc, min_y_loc;
    float max_x_loc, max_y_loc;

    // Функция перевода координат сцены в координаты окна
    QPoint to_window(float x, float y) const;
    //Кастомизация отображения
    QColor grid_s_col, grid_b_col, axis_col, plot_col;
    qreal grid_s_width, grid_b_width, axis_width, plot_width;
};

#endif // CSCENE2DN_H

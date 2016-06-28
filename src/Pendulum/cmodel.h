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

#ifndef CMODEL_H
#define CMODEL_H

#if !defined USE_SWRAST
#include <QGLWidget>
#include <QtOpenGL>
typedef QGLWidget BaseWidget;
#else
#include "swrast/swrast_common.h"
#include "swrast/swrast_widget.h"
typedef SWRastWidget BaseWidget;
#endif
#include <QImage>
#include <cmath>
#include <cstring>

#include "models/CMilkshapeModel.h"

// класс пружинка
class CModelSpring: public CMilkshapeModel
{
protected:
    // индексный массив
    int * a_index;
    // параметры циллиндра
    int num_stacks;
    int num_slices;
    // начальное смещение
    float offset_start;
    // конечное смещение
    float offset_end;
    // количество витков
    int num_turns;
    //диаметр витка
    float radius_turn;
    // диаметр проволоки
    float radius_spring;
    // индексация координат модели
    void indexate();
    // размер по умолчанию
    float def_size;
    // пропуск незначительных изменений
    float length_prev;
    float epsilon;
    // константы
    float pi_x2;
    float pi;
    float pi_d2;
public:
    // конструктор модели
    CModelSpring();
    // деструктор модели
    ~CModelSpring();
    // изменение размера модели
    void resize(float length);
    // инициализация
    void init();
};

#endif //CMODEL_H

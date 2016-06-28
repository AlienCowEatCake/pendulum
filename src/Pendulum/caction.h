/* 
   Copyright (C) 2011-2013,
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

#ifndef CACTION_H_
#define CACTION_H_
#include <cmath>

//НЕОБХОДИМЫЕ КОНСТАНТЫ

const double g = 9.81;
const double e = 2.718281828459045235;
const double pi1 = 3.1415926535897932384;

class Caction
{
public:
    /* ************************************СПЕЦИФИКАЦИЯ*************************************
    *  r.............КОЭФФИЦИЕНТ СОПРОТИВЛЕНИЯ                                             *
    *  m.............МАССА МАЯТНИКА                                                        *
    *  k.............КОЭФФИЦИЕНТ ЖЕСТКОСТИ ПРУЖИНЫ                                         *
    *  w.............ЧАСТОТА                                                               *
    *  w0............ЦИКЛИЧЕСКАЯ ЧАСТОТА СВОБОДНЫХ НЕЗАТУХАЮЩИХ КОЛЕБАНИЙ ДАННОЙ СИСТЕМЫ   *
    *  sigma.........КОЭФФИЦИЕНТ ЗАТУХАНИЯ                                                 *
    *  A0............НАЧАЛЬНАЯ АМПЛИТУДА                                                   *
    *  x.............СМЕЩЕНИЕ                                                              *
    *  T.............ПЕРИОД КОЛЕБАНИЙ                                                      *
    *  AtimeT........АМПЛИТУДА КОЛЕБАНИЙ В МОМЕНТ ВРЕМЕНИ, РАВНЫЙ T                        *
    *  Q.............ДОБРОТНОСТЬ                                                           *
    *  ldekrem.......ЛОГАРИФМИЧЕСКИЙ ДЕКРЕМЕНТ ЗАТУХАНИЙ                                   *
    *  oldx..........СМЕЩЕНИЕ В ПРЕДЫДУЩЕЕ ВРЕМЯ                                           *
    *  oldtime.......ПРЕДЫДУЩЕЕ ВРЕМЯ                                                      *
    *  v.............СКОРОСТЬ ГРУЗА                                                        *
    *  E0............НАЧАЛЬНАЯ МЕХАНИЧЕСКАЯ ЭНЕРГИЯ                                        *
    *  E.............ЭНЕРГИЯ В ДАННОЕ ВРЕМЯ                                                *
    ***************************************************************************************/
    double r, m, k, fita, w, w0, sigma, A0, x, T, AtimeT, Q, ldekrem, oldx, oldtime, v, E0, E;
    Caction();
    ~Caction();	                //Деструктор
    void Refresh(double Ntime); //Рабочая функция
    void InitBall();            //Инициализация начальных значений, однопроходные расчёты

    double ktime, length;
};

#endif

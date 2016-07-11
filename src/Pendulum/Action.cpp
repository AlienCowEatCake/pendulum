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

#include "Action.h"

#include <cmath>

namespace {

const double pi = 3.1415926535897932384;

} // namespace

Action::Action()
{
    m = 0.01;
    A0 = 0.5;
    k = 50;
    r = 3.0;
    InitBall();
}

void Action::InitBall()
{
    fita = 0;
    x = A0;
    w0 = std::sqrt(k / m);
    sigma = r / (2.0 * m);
    if(w0 > sigma)
    {
        w = std::sqrt(w0 * w0 - r * r / 4.0 / m / m);
        T = 2.0 * pi / std::sqrt(w0 * w0 - sigma * sigma);
        AtimeT = A0 * std::exp( - sigma * T) * std::cos(w * T + fita);
        ldekrem = std::log(A0 / AtimeT);
    }
    else
    {
        w = 0.0;
        T = 0.0;
        AtimeT = 0.0;
        ldekrem = 0.0;
    }
    Q = w0 / 2.0 / sigma;
    oldx = A0;
    oldtime = -1.0;
    v = 0.0;
    E0 = k * A0 * A0 / 2.0;
    E = E0;
}

void Action::Refresh(double Ntime)
{
    double t = Ntime / 1000.0;
    x = A0 * std::exp(- sigma * t) * std::cos(w * t + fita);
    v = (x - oldx) / (t - oldtime);
    oldx = x;
    oldtime = t;
    E = E0 * std::exp(- 2.0 * sigma * t);
}


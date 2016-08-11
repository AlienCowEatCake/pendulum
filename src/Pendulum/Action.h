/* 
   Copyright (C) 2011-2016,
        Andrei V. Kurochkin     <kurochkin.andrei.v@yandex.ru>
        Mikhail E. Aleksandrov  <alexandroff.m@gmail.com>
        Peter S. Zhigalov       <peter.zhigalov@gmail.com>

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

#if !defined (ACTION_H_INCLUDED)
#define ACTION_H_INCLUDED

class Action
{
public:
    Action();
    /// @brief Refresh - обновление состояния системы, основная рабочая функция
    /// @param[in] Ntime - время в миллисекундах
    void Refresh(double Ntime);
    /// @brief InitBall - инициализация начальных значений, однопроходные расчёты
    void InitBall();

    /// @brief set_r - установить коэффициент сопротивления
    /// @param[in] value - значение коэффициента сопротивления (кг/с)
    void set_r(double value)    { r = value; }
    /// @brief set_m - установить массу маятника
    /// @param[in] value - значение массы маятника (кг)
    void set_m(double value)    { m = value; }
    /// @brief set_k - установить коэффициент жесткости пружины
    /// @param[in] value - значение коэффициента жесткости пружины (Н/м)
    void set_k(double value)    { k = value; }
    /// @brief set_A0 - установить начальную амплитуду
    /// @param[in] value - значение начальной амплитуды (м)
    void set_A0(double value)   { A0 = value; }

    /// @brief get_r - получить коэффициент сопротивления
    double get_r() const        { return r; }
    /// @brief get_m - получить массу маятника
    double get_m() const        { return m; }
    /// @brief get_k - получить коэффициент жесткости пружины
    double get_k() const        { return k; }
    /// @brief get_w - получить частоту
    double get_w() const        { return w; }
    /// @brief get_w0 - получить циклическую частоту свободных незатухающих колебаний данной системы
    double get_w0() const       { return w0; }
    /// @brief get_sigma - получить коэффициент затухания
    double get_sigma() const    { return sigma; }
    /// @brief get_A0 - получить начальную амплитуду
    double get_A0() const       { return A0; }
    /// @brief get_x - получить смещение
    double get_x() const        { return x; }
    /// @brief get_T - получить период колебаний
    double get_T() const        { return T; }
    /// @brief get_AtimeT - получить амплитуду колебаний в момент времени, равный T
    double get_AtimeT() const   { return AtimeT; }
    /// @brief get_Q - получить добротность
    double get_Q() const        { return Q; }
    /// @brief get_ldekrem - получить логарифмический декремент затуханий
    double get_ldekrem() const  { return ldekrem; }
    /// @brief get_v - получить скорость груза
    double get_v() const        { return v; }
    /// @brief get_E0 - получить начальную механическую энергию
    double get_E0() const       { return E0; }
    /// @brief get_E - получить энергию в данное время
    double get_E() const        { return E; }

private:

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
};

#endif // ACTION_H_INCLUDED


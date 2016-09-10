/* 
   Copyright (C) 2011-2016,
        Andrei V. Kurochkin     <kurochkin.andrei.v@yandex.ru>
        Mikhail E. Aleksandrov  <alexandroff.m@gmail.com>
        Peter S. Zhigalov       <peter.zhigalov@gmail.com>

   This file is part of the `PhysicalLabCore' library.

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

#if !defined (PHYSICALLABCORE_SWRASTCONFIG_H_INCLUDED)
#define PHYSICALLABCORE_SWRASTCONFIG_H_INCLUDED

#include <cstddef>
#include <cstdlib>
#include <cassert>

/// Включить отладочный вывод fps в углу сцены
//#define SWRAST_DEBUG_FPS_OUTPUT

/// Включить assert'ы в индексах массивов и матриц
//#define SWRAST_DEBUG_RANGE_ASSERTIOINS

/// Включить assert'ы в проверках неполных реализаций
//#define SWRAST_DEBUG_INCOMPLETE_ASSERTIOINS

/// Максимальное количество источников света в сцене
#define SWRAST_LIGHT_MAX 1

namespace SWRastPrivate {

/// Базовые типы данных
typedef float       Real;
typedef double      Double;
typedef int         Integer;
typedef std::size_t Memsize;

} // namespace SWRastPrivate

#if defined (SWRAST_DEBUG_RANGE_ASSERTIOINS)
#define SWRAST_RANGE_ASSERT(x) assert(x)
#else
#define SWRAST_RANGE_ASSERT(x)
#endif

#if defined (SWRAST_DEBUG_INCOMPLETE_ASSERTIOINS)
#define SWRAST_INCOMPLETE_ASSERT(x) assert(x)
#else
#define SWRAST_INCOMPLETE_ASSERT(x)
#endif

#endif // PHYSICALLABCORE_SWRASTCONFIG_H_INCLUDED


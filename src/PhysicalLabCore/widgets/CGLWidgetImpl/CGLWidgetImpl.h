/* 
   Copyright (C) 2011-2016,
        Mikhail Alexandrov  <alexandroff.m@gmail.com>
        Andrey Kurochkin    <andy-717@yandex.ru>
        Peter Zhigalov      <peter.zhigalov@gmail.com>

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

#if !defined (PHYSICALLABCORE_CGLWIDGETIMPL_H_INCLUDED)
#define PHYSICALLABCORE_CGLWIDGETIMPL_H_INCLUDED

#if !defined (USE_SWRAST)
#include <QtOpenGL>
#include <QGLWidget>
#include "native/GLFunctions.h"
typedef QGLWidget CGLWidgetImpl;
#else
#include "software/SWRastWidget.h"
typedef SWRastWidget CGLWidgetImpl;
#include "software/GLFunctions.h"
#endif

#if !defined (GL_MULTISAMPLE)
#define GL_MULTISAMPLE  0x809D
#endif

#endif // PHYSICALLABCORE_CGLWIDGETIMPL_H_INCLUDED


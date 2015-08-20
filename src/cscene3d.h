/* 
   Copyright (C) 2011-2014,
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

#ifndef CSCENE3D_H
#define CSCENE3D_H

/* класс 3D сцены в главном окне*/
#if !defined USE_SWRAST
#include <QGLWidget>
#include <QtOpenGL>
typedef QGLWidget BaseWidget;
#else
#include "swrast/swrast_common.h"
#include "swrast/swrast_widget.h"
typedef SWRastWidget BaseWidget;
#endif
#include "cmodel.h"

#if !defined(GL_MULTISAMPLE)
#define GL_MULTISAMPLE  0x809D
#endif

class Cscene3D : public BaseWidget
{
    Q_OBJECT

private:
    //модели
    CMilkshapeModel tripod, sphere, scale, spring_start, spring_end, strng;
    CModelSpring cyll;

    GLfloat xRot;
    GLfloat yRot;
    GLfloat zRot;
    GLfloat zTra;
    GLfloat nSca;

    QPoint ptrMousePosition;
    //реакция на клавиши и мышь
    void scale_plus();
    void scale_minus();
    void rotate_up();
    void rotate_down();
    void rotate_left();
    void rotate_right();
    void translate_down();
    void translate_up();
    void defaultScene();
    // Обновление освещения при изменении масштаба
    void update_light();

protected:
    void initializeGL();
    void resizeGL(int nWidth, int nHeight);
    void paintGL();
    void mousePressEvent(QMouseEvent* pe);
    void mouseMoveEvent(QMouseEvent* pe);
    void mouseReleaseEvent(QMouseEvent*);
    void wheelEvent(QWheelEvent* pe);
    void keyPressEvent(QKeyEvent* pe);
    void timerEvent(QTimerEvent *); // обработка события таймера

public:
    Cscene3D(QWidget* parent = 0);

public slots:
    void actiontime();

};

#endif // CSCENE3D_H

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

#if defined(HAVE_QT5)
#include <QtWidgets>
#else
#include <QtGui>
#endif
#include "main.h"
#include "cscene3d.h"
#include <cmath>

unsigned long long int tot = 0;
#if !defined USE_SWRAST
int vfps = 25;
#else
int vfps = 40;
#endif
const static float pi=3.141593f, k=pi/180.0f;
int id_timer;
Caction m_action;

Cscene3D::Cscene3D(QWidget* parent) : CScene3DAbstract(parent)
{
    GLfloat light_ambient[] = {0.3f, 0.3f, 0.3f, 0.0f};     //фоновый свет
    GLfloat light_diffuse[] = {0.5f, 0.5f, 0.5f, 0.0f};     //значение диффузного света
    GLfloat light_position[] = {0.5f, 0.0f, 0.5f, 0.0f};    //позиция источника света
    setDefaultScene(SceneDefault(SceneParameters(-85.0f, 0.0f, 90.0f, 0.0f, 0.5f), 0.1f, 2.5f));
    setLightParameters(LightParameters(GL_LIGHT1, light_ambient, light_diffuse, light_position));
}

void Cscene3D::actiontime()
{
    tot += (long long unsigned int)(vfps * m_action.ktime);
    m_action.Refresh(tot);
}


void Cscene3D::timerEvent(QTimerEvent *) // событие таймера
{
    updateGL(); // обновление изображения
}

void Cscene3D::initializeGL()
{
    this->qglClearColor(Qt::white);
    GLImpl::glEnable(GL_DEPTH_TEST);
    GLImpl::glShadeModel(GL_FLAT);
    GLImpl::glEnable(GL_CULL_FACE);

    GLImpl::glEnableClientState(GL_VERTEX_ARRAY);
    GLImpl::glEnableClientState(GL_COLOR_ARRAY);

    tripod.loadModelData(":/data/m_tripod.ms3d");
    sphere.loadModelData(":/data/m_mass.ms3d");
    cyll.loadModelData(":/data/m_cyllinder.ms3d");
    cyll.init();
    scale.loadModelData(":/data/m_scale.ms3d");
    spring_start.loadModelData(":/data/m_spring_start.ms3d");
    spring_end.loadModelData(":/data/m_spring_end.ms3d");
    strng.loadModelData(":/data/m_string.ms3d");

    GLImpl::glLightfv(GL_LIGHT1, GL_AMBIENT, lightAmbient(GL_LIGHT1));    // Установка Фонового Света
    GLImpl::glLightfv(GL_LIGHT1, GL_DIFFUSE, lightDiffuse(GL_LIGHT1));    // Установка Диффузного Света
    GLImpl::glLightfv(GL_LIGHT1, GL_POSITION, lightPosition(GL_LIGHT1));  // Позиция света

    GLImpl::glEnable(GL_LIGHTING);  // Разрешить освещение
    GLImpl::glEnable (GL_LIGHT1);

    GLImpl::glShadeModel(GL_SMOOTH);

    GLImpl::glEnable(GL_MULTISAMPLE);
}

/*основная функция рисования*/
void Cscene3D::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glScalef(nSca(), nSca(), nSca());
    glTranslatef(0.0f, zTra(), 0.0f);
    glRotatef(xRot(), 1.0f, 0.0f, 0.0f);
    glRotatef(yRot(), 0.0f, 1.0f, 0.0f);
    glRotatef(zRot(), 0.0f, 0.0f, 1.0f);

    glRotatef(90.0f, 1.0f, 0.0f,0.0f);
    glTranslatef(0.0f, 0.75f, 0.0f);

    float balance = - 0.2 - 0.8 - (m_action.m * 9.81 / m_action.k);

    glTranslatef(0.0f, balance, 0.0f);
    scale.draw();
    glTranslatef(0.0f, -balance, 0.0f);

    tripod.draw();
    spring_start.draw();
    cyll.resize(- m_action.x - balance);
    cyll.draw();
    glTranslatef(0.0f, m_action.x + balance ,0.0f);
    sphere.draw();
    spring_end.draw();
    strng.draw();
}

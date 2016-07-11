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

#include "Scene3D.h"

#include <cmath>
#include "PhysicalController.h"

Scene3D::Scene3D(QWidget* parent) : Scene3DAbstract(parent), m_physicalController(NULL)
{
    const GLfloat light_ambient[] = {0.3f, 0.3f, 0.3f, 0.0f};   // фоновый свет
    const GLfloat light_diffuse[] = {0.5f, 0.5f, 0.5f, 0.0f};   // значение диффузного света
    const GLfloat light_position[] = {0.5f, 0.0f, 0.5f, 0.0f};  // позиция источника света
    setDefaultScene(SceneDefault(SceneParameters(-85.0f, 0.0f, 90.0f, 0.0f, 0.5f), 0.1f, 2.5f));
    setLightParameters(LightParameters(GL_LIGHT0, light_ambient, light_diffuse, light_position, 2.0f));
}

/// @brief setPhysicalController - установить контроллер физического процесса
void Scene3D::setPhysicalController(const PhysicalController *physicalController)
{
    m_physicalController = physicalController;
}

/// @brief initializeGL - инициализировать OpenGL-сцену
void Scene3D::initializeGL()
{
    this->qglClearColor(Qt::white);
    GLImpl::glEnable(GL_DEPTH_TEST);
    GLImpl::glShadeModel(GL_FLAT);
    GLImpl::glEnable(GL_CULL_FACE);

    GLImpl::glEnableClientState(GL_VERTEX_ARRAY);
    GLImpl::glEnableClientState(GL_COLOR_ARRAY);

    m_tripod.loadModelData(QLatin1String(":/data/m_tripod.ms3d"));
    m_sphere.loadModelData(QLatin1String(":/data/m_mass.ms3d"));
    m_cyllinder.loadModelData(QLatin1String(":/data/m_cyllinder.ms3d"));
    m_scale.loadModelData(QLatin1String(":/data/m_scale.ms3d"));
    m_spring_start.loadModelData(QLatin1String(":/data/m_spring_start.ms3d"));
    m_spring_end.loadModelData(QLatin1String(":/data/m_spring_end.ms3d"));
    m_strng.loadModelData(QLatin1String(":/data/m_string.ms3d"));

    GLImpl::glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient(GL_LIGHT0));    // Установка Фонового Света
    GLImpl::glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse(GL_LIGHT0));    // Установка Диффузного Света
    GLImpl::glLightfv(GL_LIGHT0, GL_POSITION, lightPosition(GL_LIGHT0));  // Позиция света

    GLImpl::glEnable(GL_LIGHTING);  // Разрешить освещение
    GLImpl::glEnable (GL_LIGHT0);

    GLImpl::glShadeModel(GL_SMOOTH);

    GLImpl::glEnable(GL_MULTISAMPLE);
}

/// @brief paintGL - отрисовать OpenGL-сцену
void Scene3D::paintGL()
{
    if(!m_physicalController)
        return;

    GLImpl::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    GLImpl::glMatrixMode(GL_MODELVIEW);
    GLImpl::glLoadIdentity();

    GLImpl::glScalef(nSca(), nSca(), nSca());
    GLImpl::glTranslatef(0.0f, zTra(), 0.0f);
    GLImpl::glRotatef(xRot(), 1.0f, 0.0f, 0.0f);
    GLImpl::glRotatef(yRot(), 0.0f, 1.0f, 0.0f);
    GLImpl::glRotatef(zRot(), 0.0f, 0.0f, 1.0f);

    GLImpl::glRotatef(90.0f, 1.0f, 0.0f,0.0f);
    GLImpl::glTranslatef(0.0f, 0.75f, 0.0f);

    float balance = static_cast<float>(- 0.2 - 0.8 - (m_physicalController->action().get_m() * 9.81 / m_physicalController->action().get_k()));
    float offset = static_cast<float>(m_physicalController->action().get_x()) + balance;

    GLImpl::glTranslatef(0.0f, balance, 0.0f);
    m_scale.draw();
    GLImpl::glTranslatef(0.0f, -balance, 0.0f);

    m_tripod.draw();
    m_spring_start.draw();
    m_cyllinder.resize(-offset);
    m_cyllinder.draw();
    GLImpl::glTranslatef(0.0f, offset, 0.0f);
    m_sphere.draw();
    m_spring_end.draw();
    m_strng.draw();
}


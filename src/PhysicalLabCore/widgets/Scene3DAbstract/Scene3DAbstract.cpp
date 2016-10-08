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

#include "Scene3DAbstract.h"

#include <cmath>
#include <cstring>
#include <cassert>
#include <QGesture>
#include <QGestureEvent>
#include <QPanGesture>
#include <QPinchGesture>

Scene3DAbstract::Scene3DAbstract(QWidget* parent)
    : GLWidgetImpl(parent)
{
    grabGesture(Qt::PanGesture);
    grabGesture(Qt::PinchGesture);
}

/// @brief Установить параметры сцены по-умолчанию
void Scene3DAbstract::setDefaultScene(const SceneDefault & sceneDefault)
{
    m_sceneDefault.xRot = sceneDefault.xRot;
    m_sceneDefault.yRot = sceneDefault.yRot;
    m_sceneDefault.zRot = sceneDefault.zRot;
    m_sceneDefault.zTra = sceneDefault.zTra;
    m_sceneDefault.nSca = sceneDefault.nSca;
    m_sceneDefault.scaleMin = sceneDefault.scaleMin;
    m_sceneDefault.scaleMax = sceneDefault.scaleMax;
    defaultScene();
}

/// @brief Установить источник света
void Scene3DAbstract::setLightParameters(const LightParameters & lightParameters)
{
    QList<LightParameters>::Iterator it;
    for(it = m_lightParameters.begin(); it != m_lightParameters.end(); ++it)
    {
        if(it->lightID == lightParameters.lightID)
            break;
    }
    if(it != m_lightParameters.end())
        * it = lightParameters;
    else
        m_lightParameters << lightParameters;
    updateLight();
}

/// @brief Изменение размеров виджета со сценой
void Scene3DAbstract::resizeGL(int nWidth, int nHeight)
{
    GLImpl::glMatrixMode(GL_PROJECTION);
    GLImpl::glLoadIdentity();

    GLdouble ratio = static_cast<GLdouble>(nHeight) / static_cast<GLdouble>(nWidth);
    if (nWidth>=nHeight)
        GLImpl::glOrtho(-1.0/ratio, 1.0/ratio, -1.0, 1.0, -10.0, 30.0);
    else
        GLImpl::glOrtho(-1.0, 1.0, -1.0*ratio, 1.0*ratio, -10.0, 30.0);

    GLImpl::glViewport(0, 0, static_cast<GLint>(nWidth), static_cast<GLint>(nHeight));
}

void Scene3DAbstract::mousePressEvent(QMouseEvent* pe)
{
    m_mousePosition = pe->pos();
}

/// @todo Зачем это нужно?
void Scene3DAbstract::mouseReleaseEvent(QMouseEvent*)
{
    updateGL();
}

void Scene3DAbstract::mouseMoveEvent(QMouseEvent* pe)
{    
    m_sceneParameters.xRot += 180.0f / 1.5f * static_cast<GLfloat>(pe->y()-m_mousePosition.y()) / static_cast<GLfloat>(height());
    m_sceneParameters.zRot += 180.0f / 1.5f * static_cast<GLfloat>(pe->x()-m_mousePosition.x()) / static_cast<GLfloat>(width());
    m_mousePosition = pe->pos();
    updateGL();
}

void Scene3DAbstract::wheelEvent(QWheelEvent* pe)
{
    if((pe->delta())>0)
        scale_plus();
    else if((pe->delta())<0)
        scale_minus();
    updateGL();
}

void Scene3DAbstract::keyPressEvent(QKeyEvent* pe)
{
    switch (pe->key())
    {
    case Qt::Key_Plus:
        scale_plus();
        break;

    case Qt::Key_Equal:
        scale_plus();
        break;

    case Qt::Key_Minus:
        scale_minus();
        break;

    case Qt::Key_Up:
        rotate_up();
        break;

    case Qt::Key_Down:
        rotate_down();
        break;

    case Qt::Key_Left:
        rotate_left();
        break;

    case Qt::Key_Right:
        rotate_right();
        break;

    case Qt::Key_Z:
        translate_down();
        break;

    case Qt::Key_X:
        translate_up();
        break;

    case Qt::Key_Space:
        defaultScene();
        break;
    }

    updateGL();
}

bool Scene3DAbstract::event(QEvent* event)
{
    if(event->type() == QEvent::Gesture)
        return gestureEvent(static_cast<QGestureEvent*>(event));
    return GLWidgetImpl::event(event);
}

void Scene3DAbstract::scale_plus()
{
    if(m_sceneParameters.nSca < m_sceneDefault.scaleMax)
    {
        m_sceneParameters.nSca = m_sceneParameters.nSca * 1.1f;
        updateLight();
    }
}

void Scene3DAbstract::scale_minus()
{
    if(m_sceneParameters.nSca > m_sceneDefault.scaleMin)
    {
        m_sceneParameters.nSca = m_sceneParameters.nSca / 1.1f;
        updateLight();
    }
}

void Scene3DAbstract::rotate_up()
{
    m_sceneParameters.xRot += 1.0f;
}

void Scene3DAbstract::rotate_down()
{
    m_sceneParameters.xRot -= 1.0f;
}

void Scene3DAbstract::rotate_left()
{
    m_sceneParameters.zRot += 1.0f;
}

void Scene3DAbstract::rotate_right()
{
    m_sceneParameters.zRot -= 1.0f;
}

void Scene3DAbstract::translate_down()
{
    m_sceneParameters.zTra -= 0.05f;
}

void Scene3DAbstract::translate_up()
{
    m_sceneParameters.zTra += 0.05f;
}

/// @brief Сброс сцены в первоначальное состояние
void Scene3DAbstract::defaultScene()
{
    m_sceneParameters.xRot = m_sceneDefault.xRot;
    m_sceneParameters.yRot = m_sceneDefault.yRot;
    m_sceneParameters.zRot = m_sceneDefault.zRot;
    m_sceneParameters.zTra = m_sceneDefault.zTra;
    m_sceneParameters.nSca = m_sceneDefault.nSca;
    updateLight();
}

const GLfloat * Scene3DAbstract::lightAmbient(GLenum light_ID) const
{
    for(QList<LightParameters>::ConstIterator it = m_lightParameters.begin(); it != m_lightParameters.end(); ++it)
    {
        if(it->lightID == light_ID)
            return it->lightAmbient;
    }
    assert(false);
    return m_lightParameters.begin()->lightAmbient;
}

const GLfloat * Scene3DAbstract::lightDiffuse(GLenum light_ID) const
{
    for(QList<LightParameters>::ConstIterator it = m_lightParameters.begin(); it != m_lightParameters.end(); ++it)
    {
        if(it->lightID == light_ID)
            return it->lightDiffuse;
    }
    assert(false);
    return m_lightParameters.begin()->lightDiffuse;
}

const GLfloat * Scene3DAbstract::lightPosition(GLenum light_ID) const
{
    for(QList<LightParameters>::ConstIterator it = m_lightParameters.begin(); it != m_lightParameters.end(); ++it)
    {
        if(it->lightID == light_ID)
            return it->lightPosition;
    }
    assert(false);
    return m_lightParameters.begin()->lightPosition;
}

bool Scene3DAbstract::gestureEvent(QGestureEvent* event)
{
    if(QGesture* pan = event->gesture(Qt::PanGesture))
        panTriggered(static_cast<QPanGesture*>(pan));
    if(QGesture* pinch = event->gesture(Qt::PinchGesture))
        pinchTriggered(static_cast<QPinchGesture*>(pinch));
    return true;
}

void Scene3DAbstract::panTriggered(QPanGesture* gesture)
{
    QPointF delta = gesture->delta();
    m_sceneParameters.xRot += 180.0f / 1.5f * static_cast<GLfloat>(delta.y()) / static_cast<GLfloat>(height());
    m_sceneParameters.zRot += 180.0f / 1.5f * static_cast<GLfloat>(delta.x()) / static_cast<GLfloat>(width());
    updateGL();
}

void Scene3DAbstract::pinchTriggered(QPinchGesture* gesture)
{
    QPinchGesture::ChangeFlags changeFlags = gesture->changeFlags();
    if(changeFlags & QPinchGesture::ScaleFactorChanged)
    {
        qreal scaleDelta = gesture->scaleFactor() - gesture->lastScaleFactor();
        float newScale = m_sceneParameters.nSca * static_cast<float>(scaleDelta);
        if(newScale <= m_sceneDefault.scaleMax && newScale >= m_sceneDefault.scaleMin)
        {
            m_sceneParameters.nSca = newScale;
            updateLight();
        }
    }
    updateGL();
}

/// @brief Обновление освещения при изменении масштаба
void Scene3DAbstract::updateLight()
{
    for(QList<LightParameters>::ConstIterator it = m_lightParameters.begin(); it != m_lightParameters.end(); ++it)
    {
        GLfloat correctionFactor = it->lightCorrection;
        if(correctionFactor > 0.0f)
        {
            GLfloat light_diffuse_new[] =
            {
                it->lightDiffuse[0] * correctionFactor * m_sceneParameters.nSca,
                it->lightDiffuse[1] * correctionFactor * m_sceneParameters.nSca,
                it->lightDiffuse[2] * correctionFactor * m_sceneParameters.nSca,
                it->lightDiffuse[3]
            };
            GLImpl::glLightfv(it->lightID, GL_DIFFUSE, light_diffuse_new);
        }
    }
}


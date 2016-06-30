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

#ifndef PHYSICALLABCORE_CSCENE3DABSTRACT_H_INCLUDED
#define PHYSICALLABCORE_CSCENE3DABSTRACT_H_INCLUDED

#include <QPoint>
#include <QList>
#include "../GLWidgetImpl/GLWidgetImpl.h"

/// @brief Абстрактный класс 3D сцены с камерой
class CScene3DAbstract : public GLWidgetImpl
{
    Q_OBJECT

public:
    CScene3DAbstract(QWidget* parent = 0);

    /// @brief Основные параметры описания состояния сцены
    struct SceneParameters
    {
        SceneParameters(GLfloat x_Rot = 0.0f, GLfloat y_Rot = 0.0f, GLfloat z_Rot = 0.0f, GLfloat z_Tra = 0.0f, GLfloat n_Sca = 1.0f)
            : xRot(x_Rot), yRot(y_Rot), zRot(z_Rot), zTra(z_Tra), nSca(n_Sca)
        {}
        GLfloat xRot;
        GLfloat yRot;
        GLfloat zRot;
        GLfloat zTra;
        GLfloat nSca;
    };

    /// @brief Описание источника света
    struct LightParameters
    {
        LightParameters(GLenum light_ID, const GLfloat light_ambient[4], const GLfloat light_diffuse[4], const GLfloat light_position[4])
            : lightID(light_ID)
        {
            for(std::size_t i = 0; i < 4; i++)
            {
                lightAmbient[i] = light_ambient[i];
                lightDiffuse[i] = light_diffuse[i];
                lightPosition[i] = light_position[i];
            }
        }
        GLenum lightID;           ///< идентификатор источника света (GL_LIGHT0, GL_LIGHT1, ...)
        GLfloat lightAmbient[4];  ///< фоновый свет
        GLfloat lightDiffuse[4];  ///< значение диффузного света
        GLfloat lightPosition[4]; ///< позиция источника света
    };

    /// @brief Дефолтное состояние сцены
    struct SceneDefault : SceneParameters
    {
        SceneDefault(const SceneParameters & sceneParameters = SceneParameters(), GLfloat scale_max = 0.1f, GLfloat scale_min = 3.0f)
            : SceneParameters(sceneParameters), scaleMax(scale_max), scaleMin(scale_min)
        {}
        GLfloat scaleMax;
        GLfloat scaleMin;
    };

protected:
    virtual void paintGL() = 0;
    virtual void initializeGL() = 0;

    void resizeGL(int nWidth, int nHeight);
    void mousePressEvent(QMouseEvent* pe);
    void mouseMoveEvent(QMouseEvent* pe);
    void mouseReleaseEvent(QMouseEvent*);
    void wheelEvent(QWheelEvent* pe);
    void keyPressEvent(QKeyEvent* pe);

    /// @brief Обновление освещения при изменении масштаба
    void updateLight();
    /// @brief Сброс сцены в первоначальное состояние
    void defaultScene();
    /// @brief Установить параметры сцены по-умолчанию
    void setDefaultScene(const SceneDefault & sceneDefault);
    /// @brief Установить источник света
    void setLightParameters(const LightParameters & lightParameters);

    GLfloat xRot() const { return m_sceneParameters.xRot; }
    GLfloat yRot() const { return m_sceneParameters.yRot; }
    GLfloat zRot() const { return m_sceneParameters.zRot; }
    GLfloat zTra() const { return m_sceneParameters.zTra; }
    GLfloat nSca() const { return m_sceneParameters.nSca; }
    const GLfloat * lightAmbient(GLenum light_ID) const;
    const GLfloat * lightDiffuse(GLenum light_ID) const;
    const GLfloat * lightPosition(GLenum light_ID) const;

private:
    SceneParameters m_sceneParameters;
    SceneDefault m_sceneDefault;
    QList<LightParameters> m_lightParameters;
    QPoint m_mousePosition;
    void scale_plus();
    void scale_minus();
    void rotate_up();
    void rotate_down();
    void rotate_left();
    void rotate_right();
    void translate_down();
    void translate_up();
};

#endif // PHYSICALLABCORE_CSCENE3DABSTRACT_H_INCLUDED

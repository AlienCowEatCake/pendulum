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

#if !defined (PHYSICALLABCORE_SWRASTWIDGET_H_INCLUDED)
#define PHYSICALLABCORE_SWRASTWIDGET_H_INCLUDED

#include <QWidget>
#include <QPoint>
#include <QPaintEngine>
#include <QPaintDevice>
#include <QColor>
#include <QVector>
#include <QResizeEvent>
#include "GLDefines.h"
#include "SWRastMatrix.h"

#define SW_LIGHT_MAX 1

/// @brief Класс виджет с софтверным растеризатором
class SWRastWidget : public QWidget
{
    Q_OBJECT

public:
    /// @brief Источник света
    struct light
    {
        GLboolean is_enabled;
        SWRastInternal::vec4f position;
        GLfloat ambient[4];
        GLfloat diffuse[4];
    };

public:
    SWRastWidget(QWidget * parent = 0);
    ~SWRastWidget();

    /// @brief qglClearColor - Задание фонового цвета
    /// @param[in] c - Фоновый цвет
    void qglClearColor(const QColor & c);

    virtual void initializeGL() = 0;
    virtual void resizeGL(int nWidth, int nHeight) = 0;
    virtual void paintGL() = 0;

    /// @brief convertToGLFormat - Конвертация текстур в совместимый формат
    /// @param[in] img - Исходное изображение
    /// @return Сконвертированное изображение
    static QImage convertToGLFormat(const QImage & img);

    /// @brief getContext - Получить глобальный контекст
    static SWRastWidget * getContext();

public slots:
    /// @brief updateGL - Обновление содержимого буфера и отрисовка
    void updateGL();

protected:
    /// @brief paintEvent - Отрисовка сцены
    void paintEvent(QPaintEvent *);
    /// @brief Изменение размеров окна
    void resizeEvent(QResizeEvent * event);
    /// @brief triangle - Отрисовка одного треугольника
    /// @param[in] verts - Вершинные координаты
    /// @param[in] texs - Текстурные координаты
    /// @param[in] light_intensity - Интенсивность освещения
    void triangle(const SWRastInternal::Matrix<4, 3, float> & verts,
                  const SWRastInternal::Matrix<2, 3, float> & texs,
                  const SWRastInternal::Matrix<3, 3, float> light_intensity);

    light m_lights[SW_LIGHT_MAX];   ///< Источники света
    light m_material;               ///< Материал

    bool m_is_initialized;  ///< Инициализировано ли?
    QImage m_buffer;        ///< Буфер изображения
    QPainter m_painter;     ///< На чем рисуем в данный момент
    QRgb m_background;      ///< Фоновый цвет

    QVector<SWRastInternal::vec3f> m_vertex;    ///< Вектор из вершин
    QVector<SWRastInternal::vec3f> m_normal;    ///< Вектор из нормалей
    QVector<SWRastInternal::vec2f> m_texcoord;  ///< Вектор из координат в текстуре

    GLenum m_currMatrixMode;                    ///< Текущий glMatrixMode
    SWRastInternal::matrix4f m_sw_modelview;    ///< Текущий GL_MODELVIEW
    SWRastInternal::matrix4f m_sw_viewport;     ///< Текущий glViewport
    SWRastInternal::matrix4f m_sw_projection;   ///< Текущий GL_PROJECTION

    QVector<float> m_zbuffer;   ///< z-буфер

    static QVector<QImage> m_textures;  ///< Хранилище наших текстур
    int m_current_texture;              ///< Текущая текстура
    bool m_textures_enabled;            ///< Включены ли текстуры

public:
    /// Аналоги OpenGL функций
    void glLightfv(GLenum light, GLenum pname, const GLfloat * params);
    void glMaterialfv(GLenum face, GLenum pname, const GLfloat * params);
    void glBindTexture(GLenum target, GLuint texture);
    void glGenTextures(GLsizei n, GLuint * m_textures);
    void glTexImage2D(GLenum target, GLint level, GLint internalFormat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid * data);
    void glNormal3fv(const GLfloat * v);
    void glTexCoord2f(GLfloat s, GLfloat t);
    void glVertex3fv(const GLfloat * v);
    void glBegin(GLenum mode);
    void glEnd();
    void glMatrixMode(GLenum mode);
    void glLoadIdentity();
    void glOrtho(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble near_val, GLdouble far_val);
    void glViewport(GLint x, GLint y, GLsizei width, GLsizei height);
    void glEnable(GLenum cap);
    void glDisable(GLenum cap);
    void glScalef(GLfloat x, GLfloat y, GLfloat z);
    void glTranslatef(GLfloat x, GLfloat y, GLfloat z);
    void glRotatef(GLfloat angle, GLfloat x, GLfloat y, GLfloat z);
    void gluLookAt(GLdouble eyeX, GLdouble eyeY, GLdouble eyeZ, GLdouble centerX, GLdouble centerY, GLdouble centerZ, GLdouble upX, GLdouble upY, GLdouble upZ);
    void gluLookAt(SWRastInternal::vec3f eye, SWRastInternal::vec3f center, SWRastInternal::vec3f up);
};

#endif // PHYSICALLABCORE_SWRASTWIDGET_H_INCLUDED


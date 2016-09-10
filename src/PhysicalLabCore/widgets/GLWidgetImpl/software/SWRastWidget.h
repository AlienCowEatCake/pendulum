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

#if !defined (PHYSICALLABCORE_SWRASTWIDGET_H_INCLUDED)
#define PHYSICALLABCORE_SWRASTWIDGET_H_INCLUDED

#include <QWidget>
#include <QPoint>
#include <QColor>
#include <QResizeEvent>
#include "GLDefines.h"

/// @brief Класс виджет с софтверным растеризатором
class SWRastWidget : public QWidget
{
    Q_OBJECT

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

    /// @brief grabFrameBuffer - Получить текущее изображение из буфера
    /// @param[in] withAlpha - Параметр для совместимости с QGLWidget, игнорируется
    /// @return Текущее изображение из буфера
    QImage grabFrameBuffer(bool withAlpha = false);

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
    GLboolean glIsEnabled(GLenum cap);

private:
    /// Private Implementation
    struct Impl;
    Impl * m_impl;
};

#endif // PHYSICALLABCORE_SWRASTWIDGET_H_INCLUDED


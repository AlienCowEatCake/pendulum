/*
   Copyright (C) 2022 Peter S. Zhigalov <peter.zhigalov@gmail.com>

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

#if !defined (PHYSICALLABCORE_QT6GLWIDGET_H_INCLUDED)
#define PHYSICALLABCORE_QT6GLWIDGET_H_INCLUDED

#include <QtOpenGL>
#include <QOpenGLWidget>
#include <QColor>
#include <QImage>

class QGLWidget : public QOpenGLWidget
{
    Q_OBJECT

public:
    QGLWidget(QWidget * parent = 0)
        : QOpenGLWidget(parent)
    {}

    void qglClearColor(const QColor & c) const
    {
        glClearColor(c.redF(), c.greenF(), c.blueF(), c.alphaF());
    }

    QImage grabFrameBuffer(bool withAlpha = false)
    {
        Q_UNUSED(withAlpha);
        return grabFramebuffer();
    }

    static QImage convertToGLFormat(const QImage & img)
    {
        return img.convertToFormat(QImage::Format_ARGB32).rgbSwapped().mirrored(false, true);
    }

public slots:
    void updateGL()
    {
        update();
    }
};

#endif // PHYSICALLABCORE_QT6GLWIDGET_H_INCLUDED


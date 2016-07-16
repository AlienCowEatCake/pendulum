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

#include "SWRastWidget.h"

#include <limits>
#include <algorithm>

//#define DEBUG_FPS_OUTPUT

#if defined (DEBUG_FPS_OUTPUT)
#include <QTime>
#include <QFont>
#endif

QVector<QImage> SWRastWidget::m_textures;

namespace {

const int SW_LIGHT_LAST = GL_LIGHT0 + SW_LIGHT_MAX - 1;

// Raster - This backend implements all rendering in pure software and is
// always used to render into QImages. For optimal performance only use the
// format types QImage::Format_ARGB32_Premultiplied, QImage::Format_RGB32 or
// QImage::Format_RGB16. Any other format, including QImage::Format_ARGB32, has
// significantly worse performance. This engine is used by default for QWidget
// and QPixmap.

/// @brief Предпочтительный формат изображений (текстур и буфера)
const QImage::Format preferredFormat = QImage::Format_RGB16;
/// @brief Количество байт на пиксель предпочтительного формата изображений
const int preferredBytesPerPixel = 2;

} // namespace

namespace GLImpl {

/// @brief Контекст, он глобальный и может быть только один
SWRastWidget * g_SWContext;

} // GLImpl

SWRastWidget::SWRastWidget(QWidget * parent)
    : QWidget(parent),
      m_is_initialized(false), m_background(Qt::white), m_sw_viewport(SWRastInternal::matrix4f::identity()),
      m_current_texture(0), m_textures_enabled(false)
{
    m_buffer = QImage(width(), height(), preferredFormat);
    GLImpl::g_SWContext = this;
    memset(m_lights, 0, sizeof(light) * SW_LIGHT_MAX);
    this->glMatrixMode(GL_PROJECTION);
    this->glLoadIdentity();
    this->glMatrixMode(GL_MODELVIEW);
    this->glLoadIdentity();
}

SWRastWidget::~SWRastWidget()
{
    if(GLImpl::g_SWContext == this)
        GLImpl::g_SWContext = NULL;
}

/// @brief qglClearColor - Задание фонового цвета
/// @param[in] c - Фоновый цвет
void SWRastWidget::qglClearColor(const QColor & c)
{
    m_background = c.rgb();
}

/// @brief updateGL - Обновление содержимого буфера и отрисовка
void SWRastWidget::updateGL()
{
    // Инициализируем если оно нужно
    if(!m_is_initialized)
    {
        initializeGL();
        m_is_initialized = true;
    }
    // Если буфер не того размера - создадим правильный
    if(m_buffer.width() != width() || m_buffer.height() != height())
    {
        m_buffer = QImage(width(), height(), preferredFormat);
    }
    // Сперва нужно залить фон
    m_buffer.fill(m_background);
    // И инициализировать z-буфер
    m_zbuffer.resize(m_buffer.width() * m_buffer.height());
    for(int i = 0; i < m_zbuffer.size(); i++)
        m_zbuffer[i] = - std::numeric_limits<float>::max();
    // Ну а дальше запустим саму отрисовку
    paintGL();
    repaint();
}

/// @brief paintEvent - Отрисовка сцены
void SWRastWidget::paintEvent(QPaintEvent *)
{
    if(m_buffer.width() == width() || m_buffer.height() == height())
    {
        QPainter pb;
        pb.begin(this);
        pb.setViewport(0, 0, width(), height());
        pb.drawImage(0, 0, m_buffer);

#if defined (DEBUG_FPS_OUTPUT)
        static QTime t;
        static int counter = -1;
        static QString fps;
        static QFont mono(QString::fromLatin1("Monospace"), 16, QFont::Bold);
        if(counter < 0)
        {
            t.start();
            counter = 0;
        }
        else if(t.elapsed() >= 1000)
        {
            fps = QString::number(counter * 1000 / t.elapsed());
            counter = 0;
            t.restart();
        }
        else
        {
            counter++;
        }
        pb.setFont(mono);
        pb.setPen(QPen(QColor(255 - qRed(m_background), 255 - qGreen(m_background), 255 - qBlue(m_background))));
        pb.drawText(QRect(0, 0, width(), height()), Qt::AlignTop | Qt::AlignRight, fps);
#endif

        pb.end();
    }
}

/// @brief Изменение размеров окна
void SWRastWidget::resizeEvent(QResizeEvent * event)
{
    resizeGL(event->size().width(), event->size().height());
    updateGL();
}

void SWRastWidget::gluLookAt(GLdouble eyeX, GLdouble eyeY, GLdouble eyeZ,
                             GLdouble centerX, GLdouble centerY, GLdouble centerZ,
                             GLdouble upX, GLdouble upY, GLdouble upZ)
{
    this->gluLookAt(SWRastInternal::vec3f(static_cast<float>(eyeX), static_cast<float>(eyeY), static_cast<float>(eyeZ)),
                    SWRastInternal::vec3f(static_cast<float>(centerX), static_cast<float>(centerY), static_cast<float>(centerZ)),
                    SWRastInternal::vec3f(static_cast<float>(upX), static_cast<float>(upY), static_cast<float>(upZ)));
}

void SWRastWidget::gluLookAt(SWRastInternal::vec3f eye, SWRastInternal::vec3f center, SWRastInternal::vec3f up)
{
    SWRastInternal::vec3f z = (eye - center).normalize();
    SWRastInternal::vec3f x = cross(up, z).normalize();
    SWRastInternal::vec3f y = cross(z, x).normalize();
    SWRastInternal::matrix4f minv(SWRastInternal::matrix4f::identity());
    SWRastInternal::matrix4f tr(SWRastInternal::matrix4f::identity());
    for(SWRastInternal::size_type i = 0; i < 3; i++)
    {
        minv[0][i] = x[i];
        minv[1][i] = y[i];
        minv[2][i] = z[i];
        tr[i][3] = -center[i];
    }
    m_sw_modelview = minv * tr;
}

void SWRastWidget::glLightfv(GLenum light, GLenum pname, const GLfloat * params)
{
    std::size_t index = static_cast<std::size_t>(light - GL_LIGHT0);
    switch(pname)
    {
    case GL_AMBIENT:
        for(std::size_t i = 0; i < 4; i++)
            m_lights[index].ambient[i] = params[i];
        break;
    case GL_DIFFUSE:
        for(std::size_t i = 0; i < 4; i++)
            m_lights[index].diffuse[i] = params[i];
        break;
    case GL_POSITION:
        assert(params[3] == 0.0f);
        for(std::size_t i = 0; i < 4; i++)
            m_lights[index].position[i] = params[i];
        m_lights[index].position = m_sw_modelview * m_lights[index].position;
        break;
    }
}

void SWRastWidget::glMaterialfv(GLenum face, GLenum pname, const GLfloat * params)
{
    Q_UNUSED(face);
    switch(pname)
    {
    case GL_AMBIENT:
        for(std::size_t i = 0; i < 4; i++)
            m_material.ambient[i] = params[i];
        break;
    case GL_DIFFUSE:
        for(std::size_t i = 0; i < 4; i++)
            m_material.diffuse[i] = params[i];
        break;
    }
}

/// @brief convertToGLFormat - Конвертация текстур в совместимый формат
/// @param[in] img - Исходное изображение
/// @return Сконвертированное изображение
QImage SWRastWidget::convertToGLFormat(const QImage & img)
{
    return img.convertToFormat(preferredFormat).mirrored(false, true);
}

/// @brief getContext - Получить глобальный контекст
SWRastWidget * SWRastWidget::getContext()
{
    return GLImpl::g_SWContext;
}

void SWRastWidget::glBindTexture(GLenum target, GLuint texture)
{
    Q_UNUSED(target);
    m_current_texture = static_cast<int>(texture) - 1;
}

void SWRastWidget::glGenTextures(GLsizei n, GLuint * textures)
{
    for(int i = 0; i < n; i++)
    {
        SWRastWidget::m_textures.push_back(QImage());
        textures[i] = static_cast<GLuint>(SWRastWidget::m_textures.size());
    }
}

void SWRastWidget::glTexImage2D(GLenum target, GLint level, GLint internalFormat,
                                GLsizei width, GLsizei height, GLint border,
                                GLenum format, GLenum type, const GLvoid * data)
{
    Q_UNUSED(target);
    Q_UNUSED(level);
    Q_UNUSED(internalFormat);
    Q_UNUSED(border);
    Q_UNUSED(format);
    Q_UNUSED(type);
    m_textures[m_current_texture] = QImage(static_cast<int>(width), static_cast<int>(height), preferredFormat);
    memcpy(m_textures[m_current_texture].bits(), data, static_cast<std::size_t>(width * height * preferredBytesPerPixel));
}

void SWRastWidget::glNormal3fv(const GLfloat * v)
{
    m_normal.push_back(SWRastInternal::vec3f(v[0], v[1], v[2]));
}

void SWRastWidget::glTexCoord2f(GLfloat s, GLfloat t)
{
    m_texcoord.push_back(SWRastInternal::vec2f(s, t));
}

void SWRastWidget::glVertex3fv(const GLfloat * v)
{
    m_vertex.push_back(SWRastInternal::vec3f(v[0], v[1], v[2]));
}

void SWRastWidget::glBegin(GLenum mode)
{
    Q_UNUSED(mode);
    assert(mode == GL_TRIANGLES);
}

/// @brief triangle - Отрисовка одного треугольника
/// @param[in] verts - Вершинные координаты
/// @param[in] texs - Текстурные координаты
/// @param[in] light_intensity - Интенсивность освещения
void SWRastWidget::triangle(const SWRastInternal::Matrix<4, 3, float> & verts,
                            const SWRastInternal::Matrix<2, 3, float> & texs,
                            const SWRastInternal::Matrix<3, 3, float> light_intensity)
{
    // Переводим в экранные координаты
    SWRastInternal::Matrix<4, 3, float> nodes = m_sw_viewport * verts;
    // Строим матрицу L-координат
    typedef double L_type;
    SWRastInternal::Matrix<3, 3, L_type> L;
    for(std::size_t i = 0; i < 2; i++)
        for(std::size_t j = 0; j < 3; j++)
            L[i][j] = static_cast<double>(nodes[i][j]);
    for(std::size_t i = 0; i < 3; i++)
        L[2][i] = 1.0;
    L_type det_D;
    L = inverse(L, det_D);
    // Если матрица не разложиласть - значит какая-то фигня, треугольник вырожден
    if(std::fabs(det_D) < 1e-12) return;

    // Находим границы изображения и текстуры
    SWRastInternal::vec2i buf_size(m_buffer.width() - 1, m_buffer.height() - 1);
    SWRastInternal::vec2f tex_size;
    if(m_textures_enabled)
        tex_size = SWRastInternal::vec2f(m_textures[m_current_texture].width() - 1, m_textures[m_current_texture].height() - 1);

    // Находим прямоугольник, в который вписан наш треугольник
    SWRastInternal::vec2i bound_min(std::numeric_limits<int>::max(), std::numeric_limits<int>::max());
    SWRastInternal::vec2i bound_max(std::numeric_limits<int>::min(), std::numeric_limits<int>::min());
    for(std::size_t i = 0; i < 3; i++)
    {
        for(std::size_t j = 0; j < 2; j++)
        {
            bound_min[j] = std::max(0,           std::min(bound_min[j], static_cast<int>(nodes[j][i])));
            bound_max[j] = std::min(buf_size[j], std::max(bound_max[j], static_cast<int>(nodes[j][i])));
        }
    }

    // Теперь проходим по прямоугольнику и закрашиваем что попало в треугольник
    for(int x = bound_min[0]; x <= bound_max[0]; x++)
    {
        for(int y = bound_min[1]; y <= bound_max[1]; y++)
        {
            // Рассчитаем L-координаты точки
            SWRastInternal::vec3f L_point;
            for(std::size_t i = 0; i < 3; i++)
                L_point[i] = static_cast<float>(L[i][2] + L[i][0] * x + L[i][1] * y);

            // Если L-координаты не в диапазоне [0,1] - значит точка вне треугольника
            if(L_point[0] > 1.0f || L_point[1] > 1.0f || L_point[2] > 1.0f)
                continue;
            if(L_point[0] < 0.0f || L_point[1] < 0.0f || L_point[2] < 0.0f)
                continue;

            // Проверим, что точка находится спереди от всего уже отрисованного
            int zbuffer_ind = x + y * m_buffer.width();
            float frag_depth = verts[2] * L_point;
            if(m_zbuffer[zbuffer_ind] > frag_depth)
                continue;
            m_zbuffer[zbuffer_ind] = frag_depth;

            // Подсчитаем уровень освещенности и раскрасим
            SWRastInternal::vec3f intensity = light_intensity * L_point;
            if(m_textures_enabled)
            {
                SWRastInternal::vec2f uv = texs * L_point;
                QRgb rgb = m_textures[m_current_texture].pixel(static_cast<int>(uv[0] * tex_size[0]),
                                                           static_cast<int>(uv[1] * tex_size[1]));
                QColor color(static_cast<int>(qRed(rgb) * intensity[0]),
                             static_cast<int>(qGreen(rgb) * intensity[0]),
                             static_cast<int>(qBlue(rgb) * intensity[0]));
                m_buffer.setPixel(x, y, color.rgb());
            }
            else
            {
                QColor color(static_cast<int>(255 * intensity[0]),
                             static_cast<int>(255 * intensity[1]),
                             static_cast<int>(255 * intensity[2]));
                m_buffer.setPixel(x, y, color.rgb());
            }
        }
    }
}

void SWRastWidget::glEnd()
{
    // Немного черной магии
    const float ambient_correct = 1.633986928f;
    float diffuse_correct = 0.7f;
    // Матричный бог, объясни мне пожалуйста строчку ниже...
    if(width() > height()) diffuse_correct *= static_cast<float>(height()) / static_cast<float>(width());

    // Фоновый свет не зависит от вершины, поэтому обсчитаем его заранее
    SWRastInternal::vec3f intensity_ambient;
    // Заодно и запомним направление источника света (для точечного источника так нельзя!)
    SWRastInternal::vec3f v3[SW_LIGHT_MAX];
    // Обойдем все источники света
    for(std::size_t j = 0; j < SW_LIGHT_MAX; j++)
    {
        // Выключенные источники нас не интересуют
        if(m_lights[j].is_enabled)
        {
            // Найдем вектор от источника
            v3[j] = SWRastInternal::proj<3>(m_sw_projection * m_lights[j].position).normalize();
            // Смешение цветов пока простое - кто ярче, того и тапки
            float r = m_lights[j].ambient[0];
            float g = m_lights[j].ambient[1];
            float b = m_lights[j].ambient[2];
            if(r > intensity_ambient[0]) intensity_ambient[0] = r;
            if(g > intensity_ambient[1]) intensity_ambient[1] = g;
            if(b > intensity_ambient[2]) intensity_ambient[2] = b;
        }
    }
    for(std::size_t j = 0; j < 3; j++)
        intensity_ambient[j] = intensity_ambient[j] * m_material.ambient[j] * ambient_correct;

    // Сохраним также произведение матриц GL_PROJECTION на GL_MODELVIEW
    SWRastInternal::matrix4f sw_pm = m_sw_projection * m_sw_modelview;
    // А также его обратно-транспонированное значение
    SWRastInternal::matrix4f sw_pm_it = sw_pm.inverse().transpose();

    // Обойдем все треугольники
    for(int i = 0; i < m_vertex.size(); i += 3)
    {
        SWRastInternal::Matrix<4, 3, float> verts;
        SWRastInternal::Matrix<2, 3, float> texs;
        SWRastInternal::Matrix<3, 3, float> light_intensity;

        // Обойдем все вершины треугольника и упакуем данные в матрицы выше
        for(std::size_t k = 0; k < 3; k++)
        {
            int vert_ind = i + static_cast<int>(k);
            SWRastInternal::vec3f norm_vert = SWRastInternal::proj<3>(sw_pm_it * SWRastInternal::embed<4>(m_normal[vert_ind], 0.0f));
            SWRastInternal::vec4f coord_vert = sw_pm * SWRastInternal::embed<4>(m_vertex[vert_ind]);
            texs.set_col(k, m_texcoord[vert_ind]);
            verts.set_col(k, coord_vert);

            // Диффузный свет зависит от вершины, поэтому в цикле смотрим его
            SWRastInternal::vec3f intensity_diffuse;
            // Далее обойдем все источники света
            for(std::size_t j = 0; j < SW_LIGHT_MAX; j++)
            {
                // Выключенные источники нас не интересуют
                if(m_lights[j].is_enabled)
                {
                    // Скалярно умножим вектор от источника на нормаль
                    float intensity = std::max(v3[j] * norm_vert, 0.0f);
                    // Смешение цветов пока простое - кто ярче, того и тапки
                    float r = m_lights[j].diffuse[0] * intensity;
                    float g = m_lights[j].diffuse[1] * intensity;
                    float b = m_lights[j].diffuse[2] * intensity;
                    if(r > intensity_diffuse[0]) intensity_diffuse[0] = r;
                    if(g > intensity_diffuse[1]) intensity_diffuse[1] = g;
                    if(b > intensity_diffuse[2]) intensity_diffuse[2] = b;
                }
            }
            // Теперь все смешиваем, не без помощи черной магии
            SWRastInternal::vec3f intensity;
            for(std::size_t j = 0; j < 3; j++)
                intensity[j] = std::min(1.0f, intensity_diffuse[j] * m_material.diffuse[j] * diffuse_correct + intensity_ambient[j]);
            light_intensity.set_col(k, intensity);
        }
        // Посылаем треугольник на отрисовку
        triangle(verts, texs, light_intensity);
    }

    m_vertex.clear();
    m_normal.clear();
    m_texcoord.clear();
}

void SWRastWidget::glMatrixMode(GLenum mode)
{
    m_currMatrixMode = mode;
}

void SWRastWidget::glLoadIdentity()
{
    switch(m_currMatrixMode)
    {
    case GL_PROJECTION:
        m_sw_projection = SWRastInternal::matrix4f::identity();
        break;
    case GL_MODELVIEW:
        m_sw_modelview = SWRastInternal::matrix4f::identity();
        // Грязный хак, иначе все перевернуто
        this->glRotatef(180.0f, 1.0f, 0.0f, 0.0f);
        break;
    }
}

void SWRastWidget::glOrtho(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble near_val, GLdouble far_val)
{
    SWRastInternal::matrix4f m = SWRastInternal::matrix4f::identity();
    m[0][0] = static_cast<float>(2.0 / (right - left));
    m[1][1] = static_cast<float>(2.0 / (top - bottom));
    m[2][2] = static_cast<float>(-2.0 / (far_val - near_val));
    m[0][3] = static_cast<float>(-(right + left) / (right - left));
    m[1][3] = static_cast<float>(-(top + bottom) / (top - bottom));
    m[2][3] = static_cast<float>(-(far_val + near_val) / (far_val - near_val));

    switch(m_currMatrixMode)
    {
    case GL_PROJECTION:
        m_sw_projection = m_sw_projection * m;
        break;
    case GL_MODELVIEW:
        m_sw_modelview = m_sw_modelview * m;
    }
}

void SWRastWidget::glViewport(GLint x, GLint y, GLsizei width, GLsizei height)
{
    m_sw_viewport = SWRastInternal::matrix4f::identity();
    m_sw_viewport[0][3] = x + width / 2.0f;
    m_sw_viewport[1][3] = y + height / 2.0f;
    m_sw_viewport[2][3] = 1.0f;
    m_sw_viewport[0][0] = width / 2.0f;
    m_sw_viewport[1][1] = height / 2.0f;
    m_sw_viewport[2][2] = 0.0f;
}

void SWRastWidget::glEnable(GLenum cap)
{
    if(cap >= GL_LIGHT0 && cap <= SW_LIGHT_LAST)
    {
        std::size_t index = static_cast<std::size_t>(cap - GL_LIGHT0);
        m_lights[index].is_enabled = true;
    }
    if(cap == GL_TEXTURE_2D)
        m_textures_enabled = true;
}

void SWRastWidget::glDisable(GLenum cap)
{
    if(cap >= GL_LIGHT0 && cap <= SW_LIGHT_LAST)
    {
        std::size_t index = static_cast<std::size_t>(cap - GL_LIGHT0);
        m_lights[index].is_enabled = false;
    }
    if(cap == GL_TEXTURE_2D)
        m_textures_enabled = false;
}

GLboolean SWRastWidget::glIsEnabled(GLenum cap)
{
    if(cap >= GL_LIGHT0 && cap <= SW_LIGHT_LAST)
    {
        std::size_t index = static_cast<std::size_t>(cap - GL_LIGHT0);
        return m_lights[index].is_enabled ? GL_TRUE : GL_FALSE;
    }
    else if(cap > SW_LIGHT_LAST && cap <= GL_MAX_LIGHTS)
        return GL_FALSE;
    else if(cap == GL_TEXTURE_2D)
        return m_textures_enabled ? GL_TRUE : GL_FALSE;
    return GL_FALSE;
}

void SWRastWidget::glScalef(GLfloat x, GLfloat y, GLfloat z)
{
    SWRastInternal::matrix4f m = SWRastInternal::matrix4f::identity();
    m[0][0] = x;
    m[1][1] = y;
    m[2][2] = z;

    switch(m_currMatrixMode)
    {
    case GL_PROJECTION:
        m_sw_projection = m_sw_projection * m;
        break;
    case GL_MODELVIEW:
        m_sw_modelview = m_sw_modelview * m;
        break;
    }
}

void SWRastWidget::glTranslatef(GLfloat x, GLfloat y, GLfloat z)
{
    SWRastInternal::matrix4f m = SWRastInternal::matrix4f::identity();
    m[0][3] = x;
    m[1][3] = y;
    m[2][3] = z;

    switch(m_currMatrixMode)
    {
    case GL_PROJECTION:
        m_sw_projection = m_sw_projection * m;
        break;
    case GL_MODELVIEW:
        m_sw_modelview = m_sw_modelview * m;
        break;
    }
}

void SWRastWidget::glRotatef(GLfloat angle, GLfloat x, GLfloat y, GLfloat z)
{
    float norm = std::sqrt(x * x + y * y + z * z);
    x /= norm;
    y /= norm;
    z /= norm;
    static const float deg2rad = 3.141593f / 180.0f;
    float c = std::cos(angle * deg2rad);
    float s = std::sin(angle * deg2rad);

    SWRastInternal::matrix4f m = SWRastInternal::matrix4f::identity();
    m[0][0] = x * x * (1.0f - c) + c;
    m[0][1] = x * y * (1.0f - c) - z * s;
    m[0][2] = x * z * (1.0f - c) + y * s;

    m[1][0] = y * x * (1.0f - c) + z * s;
    m[1][1] = y * y * (1.0f - c) + c;
    m[1][2] = y * z * (1.0f - c) - x * s;

    m[2][0] = z * x * (1.0f - c) - y * s;
    m[2][1] = z * y * (1.0f - c) + x * s;
    m[2][2] = z * z * (1.0f - c) + c;

    switch(m_currMatrixMode)
    {
    case GL_PROJECTION:
        m_sw_projection = m_sw_projection * m;
        break;
    case GL_MODELVIEW:
        m_sw_modelview = m_sw_modelview * m;
        break;
    }
}


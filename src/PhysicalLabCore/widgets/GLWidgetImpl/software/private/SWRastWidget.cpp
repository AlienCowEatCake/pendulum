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

#include <limits>
#include <algorithm>
#include <QPaintEngine>
#include <QPaintDevice>
#include <QVector>

#include "../GLDefines.h"
#include "../GLFunctions.h"
#include "../SWRastWidget.h"
#include "SWRastConfig.h"
#include "SWRastVector.h"
#include "SWRastMatrix.h"

#if defined (SWRAST_DEBUG_FPS_OUTPUT)
#include <QTime>
#include <QFont>
#endif

namespace {

const GLenum SW_LIGHT_LAST = GL_LIGHT0 + SWRAST_LIGHT_MAX - 1;

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

struct SWRastWidget::Impl
{
    /// @brief Источник света
    struct light
    {
        bool isEnabled;
        SWRastPrivate::vec4f position;
        SWRastPrivate::Real ambient[4];
        SWRastPrivate::Real diffuse[4];
    };

    Impl()
        : isInitialized(false)
        , background(Qt::white)
        , viewport(SWRastPrivate::matrix4f::identity())
        , currentTexture(0)
        , isTexturesEnabled(false)
    {
        memset(lights, 0, sizeof(light) * SWRAST_LIGHT_MAX);
    }

    /// @brief triangle - Отрисовка одного треугольника
    /// @param[in] verts - Вершинные координаты
    /// @param[in] texs - Текстурные координаты
    /// @param[in] light_intensity - Интенсивность освещения
    void triangle(const SWRastPrivate::Matrix<4, 3, SWRastPrivate::Real> & verts,
                  const SWRastPrivate::Matrix<2, 3, SWRastPrivate::Real> & texs,
                  const SWRastPrivate::Matrix<3, 3, SWRastPrivate::Real> light_intensity);

    light lights[SWRAST_LIGHT_MAX]; ///< Источники света
    light material;                 ///< Материал

    bool isInitialized; ///< Инициализировано ли?
    QImage buffer;      ///< Буфер изображения
    QPainter painter;   ///< На чем рисуем в данный момент
    QRgb background;    ///< Фоновый цвет

    QVector<SWRastPrivate::vec3f> vertexes;     ///< Вектор из вершин
    QVector<SWRastPrivate::vec3f> normales;     ///< Вектор из нормалей
    QVector<SWRastPrivate::vec2f> texcoords;    ///< Вектор из координат в текстуре

    GLenum m_currMatrixMode;                ///< Текущий glMatrixMode
    SWRastPrivate::matrix4f modelview;      ///< Текущий GL_MODELVIEW
    SWRastPrivate::matrix4f viewport;       ///< Текущий glViewport
    SWRastPrivate::matrix4f projection;     ///< Текущий GL_PROJECTION

    QVector<SWRastPrivate::Real> zBuffer;   ///< z-буфер

    static QVector<QImage> g_textures;  ///< Глобальное хранилище текстур
    int currentTexture;                 ///< Текущая текстура
    bool isTexturesEnabled;             ///< Включены ли текстуры
};

QVector<QImage> SWRastWidget::Impl::g_textures;

namespace GLImpl {

/// @brief Контекст, он глобальный и может быть только один
SWRastWidget * g_SWContext;

} // GLImpl

SWRastWidget::SWRastWidget(QWidget * parent)
    : QWidget(parent)
    , m_impl(new Impl)
{
    m_impl->buffer = QImage(width(), height(), preferredFormat);
    GLImpl::g_SWContext = this;
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
    m_impl->background = c.rgb();
}

/// @brief updateGL - Обновление содержимого буфера и отрисовка
void SWRastWidget::updateGL()
{
    // Инициализируем если оно нужно
    if(!m_impl->isInitialized)
    {
        initializeGL();
        m_impl->isInitialized = true;
    }
    // Если буфер не того размера - создадим правильный
    QImage & buffer = m_impl->buffer;
    if(buffer.width() != width() || buffer.height() != height())
    {
        buffer = QImage(width(), height(), preferredFormat);
    }
    // Сперва нужно залить фон
    buffer.fill(m_impl->background);
    // И инициализировать z-буфер
    QVector<SWRastPrivate::Real> & zBuffer = m_impl->zBuffer;
    zBuffer.resize(buffer.width() * buffer.height());
    for(int i = 0; i < zBuffer.size(); i++)
        zBuffer[i] = - std::numeric_limits<SWRastPrivate::Real>::max();
    // Ну а дальше запустим саму отрисовку
    paintGL();
    repaint();
}

/// @brief paintEvent - Отрисовка сцены
void SWRastWidget::paintEvent(QPaintEvent *)
{
    QImage & buffer = m_impl->buffer;
    if(buffer.width() == width() || buffer.height() == height())
    {
        QPainter pb;
        pb.begin(this);
        pb.setViewport(0, 0, width(), height());
        pb.drawImage(0, 0, buffer);

#if defined (SWRAST_DEBUG_FPS_OUTPUT)
        static QTime t;
        static int counter = -1;
        static QString fps;
        static const QFont mono(QString::fromLatin1("Monospace"), 16, QFont::Bold);
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
        const QRgb & background = m_impl->background;
        pb.setPen(QPen(QColor(255 - qRed(background), 255 - qGreen(background), 255 - qBlue(background))));
        pb.drawText(QRect(0, 0, buffer.width(), buffer.height()), Qt::AlignTop | Qt::AlignRight, fps);
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

void SWRastWidget::glLightfv(GLenum light, GLenum pname, const GLfloat * params)
{
    const std::size_t index = static_cast<std::size_t>(light - GL_LIGHT0);
    Impl::light * lights = m_impl->lights;
    switch(pname)
    {
    case GL_AMBIENT:
        for(std::size_t i = 0; i < 4; i++)
            lights[index].ambient[i] = static_cast<SWRastPrivate::Real>(params[i]);
        break;
    case GL_DIFFUSE:
        for(std::size_t i = 0; i < 4; i++)
            lights[index].diffuse[i] = static_cast<SWRastPrivate::Real>(params[i]);
        break;
    case GL_POSITION:
        SWRAST_INCOMPLETE_ASSERT(params[3] == 0.0f);
        for(std::size_t i = 0; i < 4; i++)
            lights[index].position[static_cast<SWRastPrivate::Memsize>(i)] = static_cast<SWRastPrivate::Real>(params[i]);
        lights[index].position = m_impl->modelview * lights[index].position;
        break;
    }
}

void SWRastWidget::glMaterialfv(GLenum face, GLenum pname, const GLfloat * params)
{
    Q_UNUSED(face);
    Impl::light & material = m_impl->material;
    switch(pname)
    {
    case GL_AMBIENT:
        for(std::size_t i = 0; i < 4; i++)
            material.ambient[i] = static_cast<SWRastPrivate::Real>(params[i]);
        break;
    case GL_DIFFUSE:
        for(std::size_t i = 0; i < 4; i++)
            material.diffuse[i] = static_cast<SWRastPrivate::Real>(params[i]);
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

/// @brief grabFrameBuffer - Получить текущее изображение из буфера
/// @param[in] withAlpha - Параметр для совместимости с QGLWidget, игнорируется
/// @return Текущее изображение из буфера
QImage SWRastWidget::grabFrameBuffer(bool withAlpha)
{
    Q_UNUSED(withAlpha);
    return m_impl->buffer;
}

/// @brief getContext - Получить глобальный контекст
SWRastWidget * SWRastWidget::getContext()
{
    return GLImpl::g_SWContext;
}

void SWRastWidget::glBindTexture(GLenum target, GLuint texture)
{
    Q_UNUSED(target);
    m_impl->currentTexture = static_cast<int>(texture) - 1;
}

void SWRastWidget::glGenTextures(GLsizei n, GLuint * textures)
{
    for(int i = 0; i < n; i++)
    {
        Impl::g_textures.push_back(QImage());
        textures[i] = static_cast<GLuint>(Impl::g_textures.size());
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
    Impl::g_textures[m_impl->currentTexture] = QImage(static_cast<int>(width), static_cast<int>(height), preferredFormat);
    memcpy(Impl::g_textures[m_impl->currentTexture].bits(), data, static_cast<std::size_t>(width * height * preferredBytesPerPixel));
}

void SWRastWidget::glNormal3fv(const GLfloat * v)
{
    m_impl->normales.push_back(SWRastPrivate::vec3f(
                                    static_cast<SWRastPrivate::Real>(v[0]),
                                    static_cast<SWRastPrivate::Real>(v[1]),
                                    static_cast<SWRastPrivate::Real>(v[2])));
}

void SWRastWidget::glTexCoord2f(GLfloat s, GLfloat t)
{
    m_impl->texcoords.push_back(SWRastPrivate::vec2f(
                                    static_cast<SWRastPrivate::Real>(s),
                                    static_cast<SWRastPrivate::Real>(t)));
}

void SWRastWidget::glVertex3fv(const GLfloat * v)
{
    m_impl->vertexes.push_back(SWRastPrivate::vec3f(
                                    static_cast<SWRastPrivate::Real>(v[0]),
                                    static_cast<SWRastPrivate::Real>(v[1]),
                                    static_cast<SWRastPrivate::Real>(v[2])));
}

void SWRastWidget::glBegin(GLenum mode)
{
    Q_UNUSED(mode);
    SWRAST_INCOMPLETE_ASSERT(mode == GL_TRIANGLES);
}

/// @brief triangle - Отрисовка одного треугольника
/// @param[in] verts - Вершинные координаты
/// @param[in] texs - Текстурные координаты
/// @param[in] light_intensity - Интенсивность освещения
void SWRastWidget::Impl::triangle(const SWRastPrivate::Matrix<4, 3, SWRastPrivate::Real> & verts,
                            const SWRastPrivate::Matrix<2, 3, SWRastPrivate::Real> & texs,
                            const SWRastPrivate::Matrix<3, 3, SWRastPrivate::Real> light_intensity)
{
    // Переводим в экранные координаты
    SWRastPrivate::Matrix<4, 3, SWRastPrivate::Real> nodes = viewport * verts;
    // Строим матрицу L-координат
    SWRastPrivate::Matrix<3, 3, SWRastPrivate::Double> L;
    for(SWRastPrivate::Memsize i = 0; i < 2; i++)
        for(SWRastPrivate::Memsize j = 0; j < 3; j++)
            L[i][j] = static_cast<SWRastPrivate::Double>(nodes[i][j]);
    for(SWRastPrivate::Memsize i = 0; i < 3; i++)
        L[2][i] = static_cast<SWRastPrivate::Double>(1);
    SWRastPrivate::Double det_D;
    L = inverse(L, det_D);
    // Если матрица не разложиласть - значит какая-то фигня, треугольник вырожден
    if(std::fabs(det_D) < 1e-12) return;

    // Находим границы изображения и текстуры
    SWRastPrivate::vec2i buf_size(buffer.width() - 1, buffer.height() - 1);
    SWRastPrivate::vec2f tex_size;
    if(isTexturesEnabled)
        tex_size = SWRastPrivate::vec2f(g_textures[currentTexture].width() - 1, g_textures[currentTexture].height() - 1);

    // Находим прямоугольник, в который вписан наш треугольник
    SWRastPrivate::vec2i bound_min(std::numeric_limits<SWRastPrivate::Integer>::max(), std::numeric_limits<SWRastPrivate::Integer>::max());
    SWRastPrivate::vec2i bound_max(std::numeric_limits<SWRastPrivate::Integer>::min(), std::numeric_limits<SWRastPrivate::Integer>::min());
    for(SWRastPrivate::Memsize i = 0; i < 3; i++)
    {
        for(SWRastPrivate::Memsize j = 0; j < 2; j++)
        {
            bound_min[j] = std::max(0,           std::min(bound_min[j], static_cast<SWRastPrivate::Integer>(nodes[j][i])));
            bound_max[j] = std::min(buf_size[j], std::max(bound_max[j], static_cast<SWRastPrivate::Integer>(nodes[j][i])));
        }
    }

    // Теперь проходим по прямоугольнику и закрашиваем что попало в треугольник
    for(SWRastPrivate::Integer x = bound_min[0]; x <= bound_max[0]; x++)
    {
        for(SWRastPrivate::Integer y = bound_min[1]; y <= bound_max[1]; y++)
        {
            // Рассчитаем L-координаты точки
            SWRastPrivate::vec3f L_point;
            for(SWRastPrivate::Memsize i = 0; i < 3; i++)
                L_point[i] = static_cast<SWRastPrivate::Real>(L[i][2] + L[i][0] * x + L[i][1] * y);

            // Если L-координаты не в диапазоне [0,1] - значит точка вне треугольника
            if(L_point[0] > static_cast<SWRastPrivate::Real>(1) ||
               L_point[1] > static_cast<SWRastPrivate::Real>(1) ||
               L_point[2] > static_cast<SWRastPrivate::Real>(1) ||
               L_point[0] < static_cast<SWRastPrivate::Real>(0) ||
               L_point[1] < static_cast<SWRastPrivate::Real>(0) ||
               L_point[2] < static_cast<SWRastPrivate::Real>(0))
                continue;

            // Проверим, что точка находится спереди от всего уже отрисованного
            const int zbuffer_ind = static_cast<int>(x + y * buffer.width());
            const SWRastPrivate::Real frag_depth = verts[2] * L_point;
            if(zBuffer[zbuffer_ind] > frag_depth)
                continue;
            zBuffer[zbuffer_ind] = frag_depth;

            // Подсчитаем уровень освещенности и раскрасим
            SWRastPrivate::vec3f intensity = light_intensity * L_point;
            if(isTexturesEnabled)
            {
                const SWRastPrivate::vec2f uv = texs * L_point;
                const QRgb rgb = g_textures[currentTexture].pixel(static_cast<int>(uv[0] * tex_size[0]),
                                                                  static_cast<int>(uv[1] * tex_size[1]));
                const QColor color(static_cast<int>(qRed(rgb)   * intensity[0]),
                                   static_cast<int>(qGreen(rgb) * intensity[0]),
                                   static_cast<int>(qBlue(rgb)  * intensity[0]));
                buffer.setPixel(x, y, color.rgb());
            }
            else
            {
                const QColor color(static_cast<int>(255 * intensity[0]),
                                   static_cast<int>(255 * intensity[1]),
                                   static_cast<int>(255 * intensity[2]));
                buffer.setPixel(x, y, color.rgb());
            }
        }
    }
}

void SWRastWidget::glEnd()
{
    const Impl::light * lights   = m_impl->lights;
    const Impl::light & material = m_impl->material;
    const SWRastPrivate::matrix4f & modelview  = m_impl->modelview;
    const SWRastPrivate::matrix4f & projection = m_impl->projection;
    QVector<SWRastPrivate::vec3f> & vertexes   = m_impl->vertexes;
    QVector<SWRastPrivate::vec3f> & normales   = m_impl->normales;
    QVector<SWRastPrivate::vec2f> & texcoords  = m_impl->texcoords;

    // Немного черной магии
    const SWRastPrivate::Real ambient_correct = static_cast<SWRastPrivate::Real>(1.633986928f);
    SWRastPrivate::Real diffuse_correct = static_cast<SWRastPrivate::Real>(0.7f);
    // Матричный бог, объясни мне пожалуйста строчку ниже...
    if(width() > height()) diffuse_correct *= static_cast<SWRastPrivate::Real>(height()) / static_cast<SWRastPrivate::Real>(width());

    // Фоновый свет не зависит от вершины, поэтому обсчитаем его заранее
    SWRastPrivate::vec3f intensity_ambient;
    // Заодно и запомним направление источника света (для точечного источника так нельзя!)
    SWRastPrivate::vec3f v3[SWRAST_LIGHT_MAX];
    // Обойдем все источники света
    for(std::size_t j = 0; j < SWRAST_LIGHT_MAX; j++)
    {
        // Выключенные источники нас не интересуют
        if(lights[j].isEnabled)
        {
            // Найдем вектор от источника
            v3[j] = SWRastPrivate::proj<3>(projection * lights[j].position).normalize();
            // Смешение цветов пока простое - кто ярче, того и тапки
            const SWRastPrivate::Real r = lights[j].ambient[0];
            const SWRastPrivate::Real g = lights[j].ambient[1];
            const SWRastPrivate::Real b = lights[j].ambient[2];
            if(r > intensity_ambient[0]) intensity_ambient[0] = r;
            if(g > intensity_ambient[1]) intensity_ambient[1] = g;
            if(b > intensity_ambient[2]) intensity_ambient[2] = b;
        }
    }
    for(SWRastPrivate::Memsize j = 0; j < 3; j++)
        intensity_ambient[j] *= material.ambient[j] * ambient_correct;

    // Сохраним также произведение матриц GL_PROJECTION на GL_MODELVIEW
    const SWRastPrivate::matrix4f sw_pm = projection * modelview;
    // А также его обратно-транспонированное значение
    const SWRastPrivate::matrix4f sw_pm_it = sw_pm.inverse().transpose();

    // Обойдем все треугольники
    for(int i = 0; i < vertexes.size(); i += 3)
    {
        SWRastPrivate::Matrix<4, 3, SWRastPrivate::Real> verts;
        SWRastPrivate::Matrix<2, 3, SWRastPrivate::Real> texs;
        SWRastPrivate::Matrix<3, 3, SWRastPrivate::Real> light_intensity;

        // Обойдем все вершины треугольника и упакуем данные в матрицы выше
        for(SWRastPrivate::Memsize k = 0; k < 3; k++)
        {
            const int vert_ind = i + static_cast<int>(k);
            SWRastPrivate::vec3f norm_vert = SWRastPrivate::proj<3>(sw_pm_it * SWRastPrivate::embed<4>(normales[vert_ind], static_cast<SWRastPrivate::Real>(0)));
            SWRastPrivate::vec4f coord_vert = sw_pm * SWRastPrivate::embed<4>(vertexes[vert_ind]);
            texs.set_col(k, texcoords[vert_ind]);
            verts.set_col(k, coord_vert);

            // Диффузный свет зависит от вершины, поэтому в цикле смотрим его
            SWRastPrivate::vec3f intensity_diffuse;
            // Далее обойдем все источники света
            for(std::size_t j = 0; j < SWRAST_LIGHT_MAX; j++)
            {
                // Выключенные источники нас не интересуют
                if(lights[j].isEnabled)
                {
                    // Скалярно умножим вектор от источника на нормаль
                    SWRastPrivate::Real intensity = std::max(v3[j] * norm_vert, static_cast<SWRastPrivate::Real>(0));
                    // Смешение цветов пока простое - кто ярче, того и тапки
                    SWRastPrivate::Real r = lights[j].diffuse[0] * intensity;
                    SWRastPrivate::Real g = lights[j].diffuse[1] * intensity;
                    SWRastPrivate::Real b = lights[j].diffuse[2] * intensity;
                    if(r > intensity_diffuse[0]) intensity_diffuse[0] = r;
                    if(g > intensity_diffuse[1]) intensity_diffuse[1] = g;
                    if(b > intensity_diffuse[2]) intensity_diffuse[2] = b;
                }
            }
            // Теперь все смешиваем, не без помощи черной магии
            SWRastPrivate::vec3f intensity;
            for(SWRastPrivate::Memsize j = 0; j < 3; j++)
                intensity[j] = std::min(static_cast<SWRastPrivate::Real>(1), intensity_diffuse[j] * material.diffuse[j] * diffuse_correct + intensity_ambient[j]);
            light_intensity.set_col(k, intensity);
        }
        // Посылаем треугольник на отрисовку
        m_impl->triangle(verts, texs, light_intensity);
    }

    vertexes.clear();
    normales.clear();
    texcoords.clear();
}

void SWRastWidget::glMatrixMode(GLenum mode)
{
    m_impl->m_currMatrixMode = mode;
}

void SWRastWidget::glLoadIdentity()
{
    switch(m_impl->m_currMatrixMode)
    {
    case GL_PROJECTION:
        m_impl->projection = SWRastPrivate::matrix4f::identity();
        break;
    case GL_MODELVIEW:
        m_impl->modelview = SWRastPrivate::matrix4f::identity();
        // Грязный хак, иначе все перевернуто
        this->glRotatef(180.0f, 1.0f, 0.0f, 0.0f);
        break;
    }
}

void SWRastWidget::glOrtho(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble near_val, GLdouble far_val)
{
    SWRastPrivate::matrix4f m = SWRastPrivate::matrix4f::identity();
    m[0][0] = static_cast<SWRastPrivate::Real>(2.0 / (right - left));
    m[1][1] = static_cast<SWRastPrivate::Real>(2.0 / (top - bottom));
    m[2][2] = static_cast<SWRastPrivate::Real>(-2.0 / (far_val - near_val));
    m[0][3] = static_cast<SWRastPrivate::Real>(-(right + left) / (right - left));
    m[1][3] = static_cast<SWRastPrivate::Real>(-(top + bottom) / (top - bottom));
    m[2][3] = static_cast<SWRastPrivate::Real>(-(far_val + near_val) / (far_val - near_val));

    switch(m_impl->m_currMatrixMode)
    {
    case GL_PROJECTION:
        m_impl->projection = m_impl->projection * m;
        break;
    case GL_MODELVIEW:
        m_impl->modelview = m_impl->modelview * m;
    }
}

void SWRastWidget::glViewport(GLint x, GLint y, GLsizei width, GLsizei height)
{
    SWRastPrivate::matrix4f & viewport = m_impl->viewport;
    viewport = SWRastPrivate::matrix4f::identity();
    viewport[0][3] = static_cast<SWRastPrivate::Real>(x) + width / static_cast<SWRastPrivate::Real>(2);
    viewport[1][3] = static_cast<SWRastPrivate::Real>(y) + height / static_cast<SWRastPrivate::Real>(2);
    viewport[2][3] = static_cast<SWRastPrivate::Real>(1);
    viewport[0][0] = width / static_cast<SWRastPrivate::Real>(2);
    viewport[1][1] = height / static_cast<SWRastPrivate::Real>(2);
    viewport[2][2] = static_cast<SWRastPrivate::Real>(0);
}

void SWRastWidget::glEnable(GLenum cap)
{
    if(cap >= GL_LIGHT0 && cap <= SW_LIGHT_LAST)
    {
        const std::size_t index = static_cast<std::size_t>(cap - GL_LIGHT0);
        m_impl->lights[index].isEnabled = true;
    }
    if(cap == GL_TEXTURE_2D)
        m_impl->isTexturesEnabled = true;
}

void SWRastWidget::glDisable(GLenum cap)
{
    if(cap >= GL_LIGHT0 && cap <= SW_LIGHT_LAST)
    {
        const std::size_t index = static_cast<std::size_t>(cap - GL_LIGHT0);
        m_impl->lights[index].isEnabled = false;
    }
    if(cap == GL_TEXTURE_2D)
        m_impl->isTexturesEnabled = false;
}

GLboolean SWRastWidget::glIsEnabled(GLenum cap)
{
    if(cap >= GL_LIGHT0 && cap <= SW_LIGHT_LAST)
    {
        const std::size_t index = static_cast<std::size_t>(cap - GL_LIGHT0);
        return m_impl->lights[index].isEnabled ? GL_TRUE : GL_FALSE;
    }
    else if(cap > SW_LIGHT_LAST && cap <= GL_MAX_LIGHTS)
        return GL_FALSE;
    else if(cap == GL_TEXTURE_2D)
        return m_impl->isTexturesEnabled ? GL_TRUE : GL_FALSE;
    return GL_FALSE;
}

void SWRastWidget::glScalef(GLfloat x, GLfloat y, GLfloat z)
{
    SWRastPrivate::matrix4f m = SWRastPrivate::matrix4f::identity();
    m[0][0] = static_cast<SWRastPrivate::Real>(x);
    m[1][1] = static_cast<SWRastPrivate::Real>(y);
    m[2][2] = static_cast<SWRastPrivate::Real>(z);

    switch(m_impl->m_currMatrixMode)
    {
    case GL_PROJECTION:
        m_impl->projection = m_impl->projection * m;
        break;
    case GL_MODELVIEW:
        m_impl->modelview = m_impl->modelview * m;
        break;
    }
}

void SWRastWidget::glTranslatef(GLfloat x, GLfloat y, GLfloat z)
{
    SWRastPrivate::matrix4f m = SWRastPrivate::matrix4f::identity();
    m[0][3] = static_cast<SWRastPrivate::Real>(x);
    m[1][3] = static_cast<SWRastPrivate::Real>(y);
    m[2][3] = static_cast<SWRastPrivate::Real>(z);

    switch(m_impl->m_currMatrixMode)
    {
    case GL_PROJECTION:
        m_impl->projection = m_impl->projection * m;
        break;
    case GL_MODELVIEW:
        m_impl->modelview = m_impl->modelview * m;
        break;
    }
}

void SWRastWidget::glRotatef(GLfloat angle, GLfloat x, GLfloat y, GLfloat z)
{
    SWRastPrivate::Real xr = static_cast<SWRastPrivate::Real>(x);
    SWRastPrivate::Real yr = static_cast<SWRastPrivate::Real>(y);
    SWRastPrivate::Real zr = static_cast<SWRastPrivate::Real>(z);
    const SWRastPrivate::Real norm = std::sqrt(xr * xr + yr * yr + zr * zr);
    xr /= norm;
    yr /= norm;
    zr /= norm;
    static const SWRastPrivate::Real deg2rad = static_cast<SWRastPrivate::Real>(3.14159265358979323846 / 180.0);
    const SWRastPrivate::Real angle_rad = static_cast<SWRastPrivate::Real>(angle) * deg2rad;
    const SWRastPrivate::Real c = std::cos(angle_rad);
    const SWRastPrivate::Real s = std::sin(angle_rad);
    const SWRastPrivate::Real c1 = (static_cast<SWRastPrivate::Real>(1) - c);

    SWRastPrivate::matrix4f m = SWRastPrivate::matrix4f::identity();
    m[0][0] = xr * xr * c1 + c;
    m[0][1] = xr * yr * c1 - zr * s;
    m[0][2] = xr * zr * c1 + yr * s;

    m[1][0] = yr * xr * c1 + zr * s;
    m[1][1] = yr * yr * c1 + c;
    m[1][2] = yr * zr * c1 - xr * s;

    m[2][0] = zr * xr * c1 - yr * s;
    m[2][1] = zr * yr * c1 + xr * s;
    m[2][2] = zr * zr * c1 + c;

    switch(m_impl->m_currMatrixMode)
    {
    case GL_PROJECTION:
        m_impl->projection = m_impl->projection * m;
        break;
    case GL_MODELVIEW:
        m_impl->modelview = m_impl->modelview * m;
        break;
    }
}


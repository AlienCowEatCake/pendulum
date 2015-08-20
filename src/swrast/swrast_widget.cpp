#include "swrast_widget.h"

// Пока контекст глобальный и может быть только один, увы
static SWRastWidget * sw_context;

// Конструктор
SWRastWidget::SWRastWidget(QWidget * parent) : QWidget(parent)
{
    buffer = QImage(width(), height(), QImage::Format_RGB888);
    background = QBrush(Qt::white);
    is_initialized = false;
    sw_context = this;
    memset(lights, 0, sizeof(light) * SW_LIGHT_MAX);
}

// Задание фонового цвета
void SWRastWidget::qglClearColor(const QColor & c)
{
    background = QBrush(c);
}

// Рисуем линию
template<typename QPointX>
void SWRastWidget::line(const QPointX & p1, const QPointX & p2, const QColor & color)
{
    painter.setPen(QPen(color));
    painter.drawLine(p1, p2);
}

// Рисуем треугольник
template<typename QPointX>
void SWRastWidget::triangle(const QPointX p[3], const QColor & color)
{
    painter.setPen(QPen(color));
    painter.setBrush(QBrush(color));
    painter.drawPolygon(p, 3);
}

// Обновление содержимого буфера
void SWRastWidget::updateGL()
{
    // Инициализируем если оно нужно
    if(!is_initialized)
    {
        initializeGL();
        is_initialized = true;
    }
    // Если буфер не того размера - создадим правильный
    if(buffer.width() != width() || buffer.height() != height())
    {
        buffer = QImage(width(), height(), QImage::Format_RGB888);
    }
    // А теперь отрисуем
    QPaintDevice * device = & buffer;
    painter.begin(device);
    painter.setViewport(currViewport);
    painter.fillRect(currViewport, background);
    //painter.setTransform(projection);
    paintGL();
    painter.end();
    repaint();
}

// Отрисовка сцены
void SWRastWidget::paintEvent(QPaintEvent *)
{
    if(buffer.width() == width() || buffer.height() == height())
    {
        QPainter pb;
        pb.begin(this);
        pb.setViewport(0, 0, width(), height());
        pb.drawImage(0, 0, buffer);
        pb.end();
    }
}

// Изменение размеров окна
void SWRastWidget::resizeEvent(QResizeEvent * event)
{
    resizeGL(event->size().width(), event->size().height());
    updateGL();
}

void SWRastWidget::set_viewport(int x, int y, int w, int h)
{
    viewport = matrix::identity();
    viewport[0][3] = x + w / 2.0f;
    viewport[1][3] = y + h / 2.0f;
    viewport[2][3] = 1.0f;
    viewport[0][0] = w / 2.0f;
    viewport[1][1] = h / 2.0f;
    viewport[2][2] = 0.0f;
}

void SWRastWidget::set_projection(float coeff)
{
    projection = matrix::identity();
    projection[3][2] = coeff;
}

void SWRastWidget::lookat(vec3f eye, vec3f center, vec3f up)
{
    vec3f z = (eye - center).normalize();
    vec3f x = cross(up, z).normalize();
    vec3f y = cross(z, x).normalize();
    matrix minv(matrix::identity());
    matrix tr(matrix::identity());
    for(size_type i = 0; i < 3; i++)
    {
        minv[0][i] = x[i];
        minv[1][i] = y[i];
        minv[2][i] = z[i];
        tr[i][3] = -center[i];
    }
    modelview = minv * tr;
}

// =================================================================================================

void glLightfv(GLenum light, GLenum pname, const GLfloat * params)
{
    size_t index = (size_t)(light - GL_LIGHT0);
    if(index >= SW_LIGHT_MAX) index = 0;
    switch(pname)
    {
    case GL_AMBIENT:
        for(size_t i = 0; i < 4; i++)
            sw_context->lights[index].ambient[i] = params[i];
        break;
    case GL_DIFFUSE:
        for(size_t i = 0; i < 4; i++)
            sw_context->lights[index].diffuse[i] = params[i];
        break;
    case GL_POSITION:
        for(size_t i = 0; i < 4; i++)
            sw_context->lights[index].position[i] = params[i];
        break;
    }
}

void glMaterialf(GLenum face, GLenum pname, GLfloat param)
{
    Q_UNUSED(face);
    Q_UNUSED(pname);
    Q_UNUSED(param);
}

void glMaterialfv(GLenum face, GLenum pname, const GLfloat * params)
{
    Q_UNUSED(face);
    switch(pname)
    {
    case GL_AMBIENT:
        for(size_t i = 0; i < 4; i++)
            sw_context->material.ambient[i] = params[i];
        break;
    case GL_DIFFUSE:
        for(size_t i = 0; i < 4; i++)
            sw_context->material.diffuse[i] = params[i];
        break;
    }
}

void glBindTexture(GLenum target, GLuint texture)
{
    Q_UNUSED(target);
    Q_UNUSED(texture);
}

void glNormal3fv(const GLfloat * v)
{
    sw_context->normal.push_back(SWRastWidget::point3df(v[0], v[1], v[2]));
}

void glTexCoord2f(GLfloat s, GLfloat t)
{
    sw_context->texcoord.push_back(SWRastWidget::point2df(s, t));
}

void glVertex3fv(const GLfloat * v)
{
    sw_context->vertex.push_back(SWRastWidget::point3df(v[0], v[1], v[2]));
}

void glBegin(GLenum mode)
{
    Q_UNUSED(mode);
}

void glEnd()
{
    for(int i = 0; i < sw_context->vertex.size(); i += 3)
    {
        QPointF p[3] =
        {
            QPointF(sw_context->vertex[i].x, sw_context->vertex[i].y),
            QPointF(sw_context->vertex[i+1].x, sw_context->vertex[i+1].y),
            QPointF(sw_context->vertex[i+2].x, sw_context->vertex[i+2].y)
        };

        // Посчитаем цвет полученного треугольника
        SWRastWidget::point3df intensity_ambient;
        SWRastWidget::point3df intensity_diffuse;
        float intensity = 0.0f;
        memset(&intensity_ambient, 0, sizeof(SWRastWidget::point3df));
        memset(&intensity_diffuse, 0, sizeof(SWRastWidget::point3df));
        // Сперва найдем нормаль к треугольнику
        SWRastWidget::point3df v1(sw_context->vertex[i+1].x - sw_context->vertex[i].x,
                sw_context->vertex[i+1].y - sw_context->vertex[i].y,
                sw_context->vertex[i+1].z - sw_context->vertex[i].z);
        SWRastWidget::point3df v2(sw_context->vertex[i+2].x - sw_context->vertex[i+1].x,
                sw_context->vertex[i+2].y - sw_context->vertex[i+1].y,
                sw_context->vertex[i+2].z - sw_context->vertex[i+1].z);
        SWRastWidget::point3df n(v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x);
        float nn = (float)sqrt(n.x * n.x + n.y * n.y + n.z * n.z);
        n.x /= nn, n.y /= nn, n.z /= nn;
        // И его барицентр
        SWRastWidget::point3df barycenter(
                    (sw_context->vertex[i].x + sw_context->vertex[i+1].x + sw_context->vertex[i+2].x) / 3.0f,
                    (sw_context->vertex[i].y + sw_context->vertex[i+1].y + sw_context->vertex[i+2].y) / 3.0f,
                    (sw_context->vertex[i].z + sw_context->vertex[i+1].z + sw_context->vertex[i+2].z) / 3.0f);
        // Далее обойдем все источники света
        for(size_t j = 0; j < SW_LIGHT_MAX; j++)
        {
            // Выключенные источники нас не интересуют
            if(sw_context->lights[j].is_enabled)
            {
                // Найдем вектор от источника к барицентру треугольника
                SWRastWidget::point3df v3(- barycenter.x + sw_context->lights[j].position[0],
                        - barycenter.y + sw_context->lights[j].position[1],
                        - barycenter.z + sw_context->lights[j].position[2]);
                float nn = (float)sqrt(v3.x * v3.x + v3.y * v3.y + v3.z * v3.z);
                v3.x /= nn, v3.y /= nn, v3.z /= nn;
                // И скалярно умножим на нормаль
                intensity = (n.x * v3.x + n.y * v3.y + n.z * v3.z) / 3.0f;
                if(intensity < 0) intensity = 0.0f;
                // Диффузный свет
                float r = sw_context->lights[j].diffuse[0] * intensity;
                float g = sw_context->lights[j].diffuse[1] * intensity;
                float b = sw_context->lights[j].diffuse[2] * intensity;
                if(r > intensity_diffuse.x) intensity_diffuse.x = r;
                if(g > intensity_diffuse.y) intensity_diffuse.y = g;
                if(b > intensity_diffuse.z) intensity_diffuse.z = b;
                // Фоновый свет
                r = sw_context->lights[j].ambient[0];
                g = sw_context->lights[j].ambient[1];
                b = sw_context->lights[j].ambient[2];
                if(r > intensity_ambient.x) intensity_ambient.x = r;
                if(g > intensity_ambient.y) intensity_ambient.y = g;
                if(b > intensity_ambient.z) intensity_ambient.z = b;
            }
        }
        // Теперь все смешиваем
        float r = intensity_diffuse.x * sw_context->material.diffuse[0] + intensity_ambient.x * sw_context->material.ambient[0];
        float g = intensity_diffuse.y * sw_context->material.diffuse[1] + intensity_ambient.y * sw_context->material.ambient[1];
        float b = intensity_diffuse.z * sw_context->material.diffuse[2] + intensity_ambient.z * sw_context->material.ambient[2];
        if(r > 1.0f) r = 1.0f;
        if(g > 1.0f) g = 1.0f;
        if(b > 1.0f) b = 1.0f;
        if(intensity > 0) // Тут должно быть определение того, смотрим мы на объект или нет
        sw_context->triangle(p, QColor(r * 255, g * 255, b * 255));
    }

    sw_context->vertex.clear();
    sw_context->normal.clear();
    sw_context->texcoord.clear();
}

void glMatrixMode(GLenum mode)
{
    sw_context->currMatrixMode = mode;
}

void glLoadIdentity()
{
    switch(sw_context->currMatrixMode)
    {
    case GL_PROJECTION:
        //sw_context->projection = QTransform();
        break;
    }
}

void glOrtho(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble near_val, GLdouble far_val)
{
    sw_context->ortho_left = left;
    sw_context->ortho_right = right;
    sw_context->ortho_bottom = bottom;
    sw_context->ortho_top = top;
    sw_context->ortho_near = near_val;
    sw_context->ortho_far = far_val;

    qreal hor_scale = (qreal)(right - left);
    qreal ver_scale = (qreal)(top - bottom);
    qreal hor_translate = -(qreal)(left - right) * 0.5f;
    qreal ver_translate = -(qreal)(bottom - top) * 0.5f;
    //sw_context->projection.scale(sw_context->width() / hor_scale, sw_context->height() / ver_scale);
    //sw_context->projection.translate(hor_translate, ver_translate);
}

void glViewport(GLint x, GLint y, GLsizei width, GLsizei height)
{
    sw_context->currViewport = QRect(x, y, width, height);
}

void glEnable(GLenum cap)
{
    if(cap >= GL_LIGHT0 && cap <= GL_LIGHT7)
    {
        size_t index = (size_t)(cap - GL_LIGHT0);
        if(index >= SW_LIGHT_MAX) index = 0;
        sw_context->lights[index].is_enabled = true;
    }
}

void glDisable(GLenum cap)
{
    if(cap >= GL_LIGHT0 && cap <= GL_LIGHT7)
    {
        size_t index = (size_t)(cap - GL_LIGHT0);
        if(index >= SW_LIGHT_MAX) index = 0;
        sw_context->lights[index].is_enabled = false;
    }
}

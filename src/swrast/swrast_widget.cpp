#include "swrast_widget.h"
#include <limits>
#include <algorithm>

QVector<QImage> SWRastWidget::textures;

// Контекст глобальный и может быть только один, увы
SWRastWidget * sw_context;

// Конструктор
SWRastWidget::SWRastWidget(QWidget * parent) : QWidget(parent)
{
    buffer = QImage(width(), height(), QImage::Format_RGB888);
    background = QBrush(Qt::white);
    is_initialized = false;
    sw_context = this;
    memset(lights, 0, sizeof(light) * SW_LIGHT_MAX);
    current_texture = 0;
    textures_enabled = false;

    sw_viewport = matrix::identity();
    this->glMatrixMode(GL_PROJECTION);
    this->glLoadIdentity();
    this->glMatrixMode(GL_MODELVIEW);
    this->glLoadIdentity();
}

// Задание фонового цвета
void SWRastWidget::qglClearColor(const QColor & c)
{
    background = QBrush(c);
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
    painter.setViewport(0, 0, width(), height());
    // Сперва нужно залить фон
    painter.fillRect(0, 0, width(), height(), background);
    // И инициализировать z-буфер
    zbuffer.resize(static_cast<int>(width() * height()));
    for(int i = 0; i < zbuffer.size(); i++)
        zbuffer[i] = - std::numeric_limits<float>::max();
    // Ну а дальше запустим саму отрисовку
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

void SWRastWidget::gluLookAt(GLdouble eyeX, GLdouble eyeY, GLdouble eyeZ, GLdouble centerX, GLdouble centerY, GLdouble centerZ, GLdouble upX, GLdouble upY, GLdouble upZ)
{
    gluLookAt(vec3f(eyeX, eyeY, eyeZ), vec3f(centerX, centerY, centerZ), vec3f(upX, upY, upZ));
}

void SWRastWidget::gluLookAt(vec3f eye, vec3f center, vec3f up)
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
    sw_modelview = minv * tr;
}

void SWRastWidget::glLightfv(GLenum light, GLenum pname, const GLfloat * params)
{
    size_t index = (size_t)(light - GL_LIGHT0);
    switch(pname)
    {
    case GL_AMBIENT:
        for(size_t i = 0; i < 4; i++)
            lights[index].ambient[i] = params[i];
        break;
    case GL_DIFFUSE:
        for(size_t i = 0; i < 4; i++)
            lights[index].diffuse[i] = params[i];
        break;
    case GL_POSITION:
        assert(params[3] == 0.0f);
        for(size_t i = 0; i < 4; i++)
            lights[index].position[i] = params[i];
        lights[index].position = sw_modelview * lights[index].position;
        break;
    }
}

void SWRastWidget::glMaterialfv(GLenum face, GLenum pname, const GLfloat * params)
{
    Q_UNUSED(face);
    switch(pname)
    {
    case GL_AMBIENT:
        for(size_t i = 0; i < 4; i++)
            material.ambient[i] = params[i];
        break;
    case GL_DIFFUSE:
        for(size_t i = 0; i < 4; i++)
            material.diffuse[i] = params[i];
        break;
    }
}

QImage SWRastWidget::convertToGLFormat(const QImage & img)
{
    return img.convertToFormat(QImage::Format_RGB888);
}

void SWRastWidget::glBindTexture(GLenum target, GLuint texture)
{
    Q_UNUSED(target);
    current_texture = texture - 1;
}

void SWRastWidget::glGenTextures(GLsizei n, GLuint * textures)
{
    for(int i = 0; i < n; i++)
    {
        SWRastWidget::textures.push_back(QImage());
        textures[i] = SWRastWidget::textures.size();
    }
}

void SWRastWidget::glTexImage2D(GLenum target, GLint level, GLint internalFormat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid * data)
{
    Q_UNUSED(target);
    Q_UNUSED(level);
    Q_UNUSED(internalFormat);
    Q_UNUSED(border);
    Q_UNUSED(format);
    Q_UNUSED(type);
    textures[current_texture] = QImage((int)width, (int)height, QImage::Format_RGB888);
    memcpy(textures[current_texture].bits(), data, width * height * 3);
}

void SWRastWidget::glNormal3fv(const GLfloat * v)
{
    normal.push_back(vec3f(v[0], v[1], v[2]));
}

void SWRastWidget::glTexCoord2f(GLfloat s, GLfloat t)
{
    texcoord.push_back(vec2f(s, t));
}

void SWRastWidget::glVertex3fv(const GLfloat * v)
{
    vertex.push_back(vec3f(v[0], v[1], v[2]));
}

void SWRastWidget::glBegin(GLenum mode)
{
    Q_UNUSED(mode);
    assert(mode == GL_TRIANGLES);
}

// Работа с барицентрическими координатами
vec3f SWRastWidget::barycentric(vec2f a, vec2f b, vec2f c, vec2f p)
{
    vec3f s[2];
    for(size_t i = 0; i < 2; i++)
    {
        s[i][0] = c[i]-a[i];
        s[i][1] = b[i]-a[i];
        s[i][2] = a[i]-p[i];
    }
    vec3f u = cross(s[0], s[1]);
    if(fabs(u[2]) > 1e-7)
        return vec3f(1.0f - (u[0] + u[1]) / u[2], u[1] / u[2], u[0] / u[2]);
    return vec3f(-1.0f, 1.0f, 1.0f);
}

// Рисуем треугольник
void SWRastWidget::triangle(mat_t<4, 3, float> & verts, mat_t<2, 3, float> & texs, mat_t<3, 3, float> & norms, mat_t<3, 3, float> light_intensity)
{
    mat_t<3, 4, float> pts = (sw_viewport * verts).transpose();
    mat_t<3, 2, float> pts2;
    for(size_t i = 0; i < 3; i++)
        pts2[i] = proj<2>(pts[i] / pts[i][3]);

    vec2f bboxmin( std::numeric_limits<float>::max(),  std::numeric_limits<float>::max());
    vec2f bboxmax(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());
    vec2f clamp(width() - 1, height() - 1);
    for(size_t i = 0; i < 3; i++)
    {
        for(size_t j = 0; j < 2; j++)
        {
            bboxmin[j] = std::max(0.0f,     std::min(bboxmin[j], pts2[i][j]));
            bboxmax[j] = std::min(clamp[j], std::max(bboxmax[j], pts2[i][j]));
        }
    }
    vec2i p;
    QColor color;
    for(p[0] = (int)bboxmin[0]; p[0] <= (int)bboxmax[0]; p[0]++)
    {
        for(p[1] = (int)bboxmin[1]; p[1] <= (int)bboxmax[1]; p[1]++)
        {
            vec3f bc_screen = barycentric(pts2[0], pts2[1], pts2[2], p);
            vec3f bc_clip = vec3f(bc_screen[0] / pts[0][3], bc_screen[1] / pts[1][3], bc_screen[2] / pts[2][3]);
            bc_clip = bc_clip / (bc_clip[0] + bc_clip[1] + bc_clip[2]);
            float frag_depth = verts[2] * bc_clip;
            if(bc_screen[0] < 0 || bc_screen[1] < 0 || bc_screen[2] < 0 || zbuffer[p[0] + p[1] * width()] > frag_depth)
                continue;

            vec3f intensity = light_intensity * bc_clip;
            if(textures_enabled)
            {
                vec3f bn = (norms * bc_clip).normalize();
                vec2f uv = texs * bc_clip;
                mat_t<3, 3, float> ndc_tri;
                for(size_t i = 0; i < 3; i++)
                    ndc_tri.set_col(i, proj<3>(verts.col(i) / verts.col(i)[3]));
                mat_t<3, 3, float> A;
                A[0] = ndc_tri.col(1) - ndc_tri.col(0);
                A[1] = ndc_tri.col(2) - ndc_tri.col(0);
                A[2] = bn;
                A = A.inverse();
                vec3f bu = A * vec3f(texs[0][1] - texs[0][0], texs[0][2] - texs[0][0], 0);
                vec3f bv = A * vec3f(texs[1][1] - texs[1][0], texs[1][2] - texs[1][0], 0);
                mat_t<3, 3, float> B;
                B.set_col(0, bu.normalize());
                B.set_col(1, bv.normalize());
                B.set_col(2, bn);

                int tex_x = (int)(uv[0] * (textures[current_texture].width() - 1));
                int tex_y = textures[current_texture].height() - 1 - (int)(uv[1] * (textures[current_texture].height() - 1));
                QRgb rgb = textures[current_texture].pixel(tex_x, tex_y);
                color = QColor((int)(qRed(rgb) * intensity[0]), (int)(qGreen(rgb) * intensity[0]), (int)(qBlue(rgb) * intensity[0]));
            }
            else
            {
                color = QColor((int)(255 * intensity[0]), (int)(255 * intensity[1]), (int)(255 * intensity[2]));
            }

            zbuffer[p[0] + p[1] * width()] = frag_depth;
            painter.setPen(QPen(color));
            painter.setBrush(QBrush(color));
            painter.drawPoint(p[0], p[1]);
        }
    }
}

void SWRastWidget::glEnd()
{
    // Обойдем все треугольники
    for(int i = 0; i < vertex.size(); i += 3)
    {
        mat_t<4, 3, float> verts;
        mat_t<2, 3, float> texs;
        mat_t<3, 3, float> norms;
        mat_t<3, 3, float> light_intensity;

        // Немного черной магии
        const float ambient_correct = 1.633986928f;
        const float diffuse_correct = 0.7f;

        // Обойдем все вершины треугольника и упакуем данные в матрицы выше
        for(size_t k = 0; k < 3; k++)
        {
            vec3f norm_vert = proj<3>((sw_projection * sw_modelview).inverse().transpose() * embed<4>(normal[(int)(i + k)], 0.0f));
            vec4f coord_vert = sw_projection * sw_modelview * embed<4>(vertex[(int)(i + k)]);

            texs.set_col(k, texcoord[(int)(i + k)]);
            norms.set_col(k, norm_vert);
            verts.set_col(k, coord_vert);

            vec3f intensity_ambient;
            vec3f intensity_diffuse;
            // Далее обойдем все источники света
            for(size_t j = 0; j < SW_LIGHT_MAX; j++)
            {
                // Выключенные источники нас не интересуют
                if(lights[j].is_enabled)
                {
                    // Найдем вектор от источника к вершине треугольника
                    vec3f v3 = proj<3>(sw_projection * lights[j].position).normalize();
                    // И скалярно умножим на нормаль
                    float intensity = std::max(v3 * norm_vert, 0.0f);
                    // Смешение цветов пока простое - кто ярче, того и тапки
                    // Диффузный свет
                    float r = lights[j].diffuse[0] * intensity;
                    float g = lights[j].diffuse[1] * intensity;
                    float b = lights[j].diffuse[2] * intensity;
                    if(r > intensity_diffuse[0]) intensity_diffuse[0] = r;
                    if(g > intensity_diffuse[1]) intensity_diffuse[1] = g;
                    if(b > intensity_diffuse[2]) intensity_diffuse[2] = b;
                    // Фоновый свет
                    r = lights[j].ambient[0];
                    g = lights[j].ambient[1];
                    b = lights[j].ambient[2];
                    if(r > intensity_ambient[0]) intensity_ambient[0] = r;
                    if(g > intensity_ambient[1]) intensity_ambient[1] = g;
                    if(b > intensity_ambient[2]) intensity_ambient[2] = b;
                }
            }
            // Теперь все смешиваем, не без помощи черной магии
            vec3f intensity;
            for(size_t j = 0; j < 3; j++)
                intensity[j] = std::min(1.0f, intensity_diffuse[j] * material.diffuse[j] * diffuse_correct +
                                              intensity_ambient[j] * material.ambient[j] * ambient_correct);
            light_intensity.set_col(k, intensity);
        }
        // Посылаем треугольник на отрисовку
        triangle(verts, texs, norms, light_intensity);
    }

    vertex.clear();
    normal.clear();
    texcoord.clear();
}

void SWRastWidget::glMatrixMode(GLenum mode)
{
    currMatrixMode = mode;
}

void SWRastWidget::glLoadIdentity()
{
    switch(currMatrixMode)
    {
    case GL_PROJECTION:
        sw_projection = matrix::identity();
        break;
    case GL_MODELVIEW:
        sw_modelview = matrix::identity();
        // Грязный хак, иначе все перевернуто
        glRotatef(180.0f, 1.0f, 0.0f, 0.0f);
        break;
    }
}

void SWRastWidget::glOrtho(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble near_val, GLdouble far_val)
{
    matrix m = matrix::identity();
    m[0][0] = 2.0 / (right - left);
    m[1][1] = 2.0 / (top - bottom);
    m[2][2] = -2.0 / (far_val - near_val);
    m[0][3] = -(right + left) / (right - left);
    m[1][3] = -(top + bottom) / (top - bottom);
    m[2][3] = -(far_val + near_val) / (far_val - near_val);

    switch(currMatrixMode)
    {
    case GL_PROJECTION:
        sw_projection = sw_projection * m;
        break;
    case GL_MODELVIEW:
        sw_modelview = sw_modelview * m;
    }
}

void SWRastWidget::glViewport(GLint x, GLint y, GLsizei width, GLsizei height)
{
    sw_viewport = matrix::identity();
    sw_viewport[0][3] = x + width / 2.0f;
    sw_viewport[1][3] = y + height / 2.0f;
    sw_viewport[2][3] = 1.0f;
    sw_viewport[0][0] = width / 2.0f;
    sw_viewport[1][1] = height / 2.0f;
    sw_viewport[2][2] = 0.0f;
}

void SWRastWidget::glEnable(GLenum cap)
{
    if(cap >= GL_LIGHT0 && cap <= GL_LIGHT7)
    {
        size_t index = (size_t)(cap - GL_LIGHT0);
        lights[index].is_enabled = true;
    }
    if(cap == GL_TEXTURE_2D)
        textures_enabled = true;
}

void SWRastWidget::glDisable(GLenum cap)
{
    if(cap >= GL_LIGHT0 && cap <= GL_LIGHT7)
    {
        size_t index = (size_t)(cap - GL_LIGHT0);
        lights[index].is_enabled = false;
    }
    if(cap == GL_TEXTURE_2D)
        textures_enabled = false;
}

void SWRastWidget::glScalef(GLfloat x, GLfloat y, GLfloat z)
{
    matrix m = matrix::identity();
    m[0][0] = x;
    m[1][1] = y;
    m[2][2] = z;

    switch(currMatrixMode)
    {
    case GL_PROJECTION:
        sw_projection = sw_projection * m;
        break;
    case GL_MODELVIEW:
        sw_modelview = sw_modelview * m;
        break;
    }
}

void SWRastWidget::glTranslatef(GLfloat x, GLfloat y, GLfloat z)
{
    matrix m = matrix::identity();
    m[0][3] = x;
    m[1][3] = y;
    m[2][3] = z;

    switch(currMatrixMode)
    {
    case GL_PROJECTION:
        sw_projection = sw_projection * m;
        break;
    case GL_MODELVIEW:
        sw_modelview = sw_modelview * m;
        break;
    }
}

void SWRastWidget::glRotatef(GLfloat angle, GLfloat x, GLfloat y, GLfloat z)
{
    float norm = sqrt(x * x + y * y + z * z);
    x /= norm;
    y /= norm;
    z /= norm;
    static const float deg2rad = 3.141593f / 180.0f;
    float c = cos(angle * deg2rad);
    float s = sin(angle * deg2rad);

    matrix m = matrix::identity();
    m[0][0] = x * x * (1.0f - c) + c;
    m[0][1] = x * y * (1.0f - c) - z * s;
    m[0][2] = x * z * (1.0f - c) + y * s;

    m[1][0] = y * x * (1.0f - c) + z * s;
    m[1][1] = y * y * (1.0f - c) + c;
    m[1][2] = y * z * (1.0f - c) - x * s;

    m[2][0] = z * x * (1.0f - c) - y * s;
    m[2][1] = z * y * (1.0f - c) + x * s;
    m[2][2] = z * z * (1.0f - c) + c;

    switch(currMatrixMode)
    {
    case GL_PROJECTION:
        sw_projection = sw_projection * m;
        break;
    case GL_MODELVIEW:
        sw_modelview = sw_modelview * m;
        break;
    }
}

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
    background = Qt::white;
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
    background = c.rgb();
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
    // Сперва нужно залить фон
    buffer.fill(background);
    // И инициализировать z-буфер
    zbuffer.resize(static_cast<int>(buffer.width() * buffer.height()));
    for(int i = 0; i < zbuffer.size(); i++)
        zbuffer[i] = - std::numeric_limits<float>::max();
    // Ну а дальше запустим саму отрисовку
    paintGL();
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
    return img.convertToFormat(QImage::Format_RGB888).mirrored(false, true);
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

// Рисуем треугольник
void SWRastWidget::triangle(mat_t<4, 3, float> & verts, mat_t<2, 3, float> & texs, mat_t<3, 3, float> light_intensity)
{
    // Переводим в экранные координаты
    mat_t<4, 3, float> nodes = sw_viewport * verts;
    // Строим матрицу L-координат
    typedef double L_type;
    mat_t<3, 3, L_type> L;
    for(size_t i = 0; i < 2; i++)
        for(size_t j = 0; j < 3; j++)
            L[i][j] = nodes[i][j];
    for(size_t i = 0; i < 3; i++)
        L[2][i] = 1.0;
    L_type det_D;
    L = inv(L, det_D);
    // Если матрица не разложиласть - значит какая-то фигня, треугольник вырожден
    if(fabs(det_D) < 1e-12) return;

    // Находим границы изображения и текстуры
    vec2i buf_size(buffer.width() - 1, buffer.height() - 1);
    vec2f tex_size;
    if(textures_enabled)
        tex_size = vec2f(textures[current_texture].width() - 1, textures[current_texture].height() - 1);

    // Находим прямоугольник, в который вписан наш треугольник
    vec2i bound_min(std::numeric_limits<int>::max(), std::numeric_limits<int>::max());
    vec2i bound_max(std::numeric_limits<int>::min(), std::numeric_limits<int>::min());
    for(size_t i = 0; i < 3; i++)
    {
        for(size_t j = 0; j < 2; j++)
        {
            bound_min[j] = std::max(0,           std::min(bound_min[j], (int)nodes[j][i]));
            bound_max[j] = std::min(buf_size[j], std::max(bound_max[j], (int)nodes[j][i]));
        }
    }

    // Теперь проходим по прямоугольнику и закрашиваем что попало в треугольник
    for(int x = bound_min[0]; x <= bound_max[0]; x++)
    {
        for(int y = bound_min[1]; y <= bound_max[1]; y++)
        {
            // Рассчитаем L-координаты точки
            vec3f L_point;
            for(size_t i = 0; i < 3; i++)
                L_point[i] = (float)(L[i][2] + L[i][0] * x + L[i][1] * y);

            // Если L-координаты не в диапазоне [0,1] - значит точка вне треугольника
            if(L_point[0] > 1.0f || L_point[1] > 1.0f || L_point[2] > 1.0f)
                continue;
            if(L_point[0] < 0.0f || L_point[1] < 0.0f || L_point[2] < 0.0f)
                continue;

            // Проверим, что точка находится спереди от всего уже отрисованного
            int zbuffer_ind = x + y * buffer.width();
            float frag_depth = verts[2] * L_point;
            if(zbuffer[zbuffer_ind] > frag_depth)
                continue;
            zbuffer[zbuffer_ind] = frag_depth;

            // Подсчитаем уровень освещенности и раскрасим
            vec3f intensity = light_intensity * L_point;
            if(textures_enabled)
            {
                vec2f uv = texs * L_point;
                QRgb rgb = textures[current_texture].pixel((int)(uv[0] * tex_size[0]), (int)(uv[1] * tex_size[1]));
                QColor color((int)(qRed(rgb) * intensity[0]), (int)(qGreen(rgb) * intensity[0]), (int)(qBlue(rgb) * intensity[0]));
                buffer.setPixel(x, y, color.rgb());
            }
            else
            {
                QColor color((int)(255 * intensity[0]), (int)(255 * intensity[1]), (int)(255 * intensity[2]));
                buffer.setPixel(x, y, color.rgb());
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
    if(width() > height()) diffuse_correct *= (float)height() / (float)width();

    // Фоновый свет не зависит от вершины, поэтому обсчитаем его заранее
    vec3f intensity_ambient;
    // Заодно и запомним направление источника света (для точечного источника так нельзя!)
    vec3f v3[SW_LIGHT_MAX];
    // Обойдем все источники света
    for(size_t j = 0; j < SW_LIGHT_MAX; j++)
    {
        // Выключенные источники нас не интересуют
        if(lights[j].is_enabled)
        {
            // Найдем вектор от источника
            v3[j] = proj<3>(sw_projection * lights[j].position).normalize();
            // Смешение цветов пока простое - кто ярче, того и тапки
            float r = lights[j].ambient[0];
            float g = lights[j].ambient[1];
            float b = lights[j].ambient[2];
            if(r > intensity_ambient[0]) intensity_ambient[0] = r;
            if(g > intensity_ambient[1]) intensity_ambient[1] = g;
            if(b > intensity_ambient[2]) intensity_ambient[2] = b;
        }
    }
    for(size_t j = 0; j < 3; j++)
        intensity_ambient[j] = intensity_ambient[j] * material.ambient[j] * ambient_correct;

    // Сохраним также произведение матриц GL_PROJECTION на GL_MODELVIEW
    matrix sw_pm = sw_projection * sw_modelview;
    // А также его обратно-транспонированное значение
    matrix sw_pm_it = sw_pm.inverse().transpose();

    // Обойдем все треугольники
    for(int i = 0; i < vertex.size(); i += 3)
    {
        mat_t<4, 3, float> verts;
        mat_t<2, 3, float> texs;
        mat_t<3, 3, float> light_intensity;

        // Обойдем все вершины треугольника и упакуем данные в матрицы выше
        for(size_t k = 0; k < 3; k++)
        {
            int vert_ind = (int)(i + k);
            vec3f norm_vert = proj<3>(sw_pm_it * embed<4>(normal[vert_ind], 0.0f));
            vec4f coord_vert = sw_pm * embed<4>(vertex[vert_ind]);
            texs.set_col(k, texcoord[vert_ind]);
            verts.set_col(k, coord_vert);

            // Диффузный свет зависит от вершины, поэтому в цикле смотрим его
            vec3f intensity_diffuse;
            // Далее обойдем все источники света
            for(size_t j = 0; j < SW_LIGHT_MAX; j++)
            {
                // Выключенные источники нас не интересуют
                if(lights[j].is_enabled)
                {
                    // Скалярно умножим вектор от источника на нормаль
                    float intensity = std::max(v3[j] * norm_vert, 0.0f);
                    // Смешение цветов пока простое - кто ярче, того и тапки
                    float r = lights[j].diffuse[0] * intensity;
                    float g = lights[j].diffuse[1] * intensity;
                    float b = lights[j].diffuse[2] * intensity;
                    if(r > intensity_diffuse[0]) intensity_diffuse[0] = r;
                    if(g > intensity_diffuse[1]) intensity_diffuse[1] = g;
                    if(b > intensity_diffuse[2]) intensity_diffuse[2] = b;
                }
            }
            // Теперь все смешиваем, не без помощи черной магии
            vec3f intensity;
            for(size_t j = 0; j < 3; j++)
                intensity[j] = std::min(1.0f, intensity_diffuse[j] * material.diffuse[j] * diffuse_correct + intensity_ambient[j]);
            light_intensity.set_col(k, intensity);
        }
        // Посылаем треугольник на отрисовку
        triangle(verts, texs, light_intensity);
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

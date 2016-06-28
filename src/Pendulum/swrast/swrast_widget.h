#ifndef SWRAST_WIDGET_H
#define SWRAST_WIDGET_H

/*
 *  :: По поводу написания растеризатора есть следующий цикл статей на хабре ::
 *
 * Статья 1: алгоритм Брезенхэма ( http://habrahabr.ru/post/248153/ )
 * Статья 2: растеризация треугольника + отсечение задних граней ( http://habrahabr.ru/post/248159/ )
 * Статья 3: Удаление невидимых поверхностей: z-буфер ( http://habrahabr.ru/post/248179/ )
 * Статья 4: Необходимая геометрия: фестиваль матриц
 *       4а: Построение перспективного искажения ( http://habrahabr.ru/post/248611/ )
 *       4б: двигаем камеру и что из этого следует ( http://habrahabr.ru/post/248723/ )
 *       4в: новый растеризатор и коррекция перспективных искажений ( http://habrahabr.ru/post/249467/ )
 * Статья 5: Пишем шейдеры под нашу библиотеку ( http://habrahabr.ru/post/248963/ )
 * Статья 6: Чуть больше, чем просто шейдер: просчёт теней ( http://habrahabr.ru/post/249139/ )
 *
 * Статья 3.1: Настала пора рефакторинга ( http://habrahabr.ru/post/248909/ )
 * Статья 3.14: Красивый класс матриц ( http://habrahabr.ru/post/249101/ )
 * как работает новый растеризатор
 */

#include <QWidget>
#include <QPoint>
#include <QPaintEngine>
#include <QPaintDevice>
#include <QColor>
#include <QVector>
#include <QResizeEvent>
#include "swrast_common.h"
#include "swrast_geometry.h"

#define SW_LIGHT_MAX 2

// Класс виджет с софтварным растеризатором
class SWRastWidget : public QWidget
{
    Q_OBJECT

public:
    // Источник света
    struct light
    {
        GLboolean is_enabled;
        vec4f position;
        GLfloat ambient[4];
        GLfloat diffuse[4];
    };

public:
    SWRastWidget(QWidget * parent = 0);

    // Задание фонового цвета
    void qglClearColor(const QColor & c);

    // Функции для переопределения
    virtual void initializeGL() = 0;
    virtual void resizeGL(int nWidth, int nHeight) = 0;
    virtual void paintGL() = 0;

    static QImage convertToGLFormat(const QImage & img);

public slots:
    void updateGL();

protected:
    // Отрисовка сцены
    void paintEvent(QPaintEvent *);
    // Изменение размеров окна
    void resizeEvent(QResizeEvent * event);
    // Рисуем треугольник
    void triangle(mat_t<4, 3, float> & verts, mat_t<2, 3, float> & texs, mat_t<3, 3, float> light_intensity);

    light lights[SW_LIGHT_MAX]; // Источники света
    light material;             // Материал (ну да, он как свет)

    bool is_initialized;        // Инициализировано ли?
    QImage buffer;              // Буфер изображения
    QPainter painter;           // На чем рисуем в данный момент
    QRgb background;            // Фоновый цвет

    QVector<vec3f> vertex;      // Вектор из вершин
    QVector<vec3f> normal;      // Вектор из нормалей
    QVector<vec2f> texcoord;    // Вектор из координат в текстуре

    GLenum currMatrixMode;      // Текущий glMatrixMode
    matrix sw_modelview;        // Текущий GL_MODELVIEW
    matrix sw_viewport;         // Текущий glViewport
    matrix sw_projection;       // Текущий GL_PROJECTION

    QVector<float> zbuffer;     // z-буфер

    static QVector<QImage> textures;    // Хранилище наших текстур
    GLuint current_texture;             // Текущая текстура
    bool textures_enabled;              // Включены ли текстуры

public:
    // Аналоги OpenGL функций
    void glLightfv(GLenum light, GLenum pname, const GLfloat * params);
    void glMaterialfv(GLenum face, GLenum pname, const GLfloat * params);
    void glBindTexture(GLenum target, GLuint texture);
    void glGenTextures(GLsizei n, GLuint * textures);
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
    void gluLookAt(vec3f eye, vec3f center, vec3f up);
};

// Контекст глобальный и может быть только один, увы
extern SWRastWidget * sw_context;

// Перенаправим вызовы OpenGL функций в наш контекст
inline void glLightfv(GLenum light, GLenum pname, const GLfloat * params)   {sw_context->glLightfv(light, pname, params);}
inline void glMaterialfv(GLenum face, GLenum pname, const GLfloat * params) {sw_context->glMaterialfv(face, pname, params);}
inline void glBindTexture(GLenum target, GLuint texture)                    {sw_context->glBindTexture(target, texture);}
inline void glGenTextures(GLsizei n, GLuint * textures)                     {sw_context->glGenTextures(n, textures);}
inline void glTexImage2D(GLenum target, GLint level, GLint internalFormat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid * data)
                                                                            {sw_context->glTexImage2D(target, level, internalFormat, width, height, border, format, type, data);}
inline void glNormal3fv(const GLfloat * v)                                  {sw_context->glNormal3fv(v);}
inline void glTexCoord2f(GLfloat s, GLfloat t)                              {sw_context->glTexCoord2f(s, t);}
inline void glVertex3fv(const GLfloat * v)                                  {sw_context->glVertex3fv(v);}
inline void glBegin(GLenum mode)                                            {sw_context->glBegin(mode);}
inline void glEnd()                                                         {sw_context->glEnd();}
inline void glMatrixMode(GLenum mode)                                       {sw_context->glMatrixMode(mode);}
inline void glLoadIdentity()                                                {sw_context->glLoadIdentity();}
inline void glViewport(GLint x, GLint y, GLsizei width, GLsizei height)     {sw_context->glViewport(x, y, width, height);}
inline void glEnable(GLenum cap)                                            {sw_context->glEnable(cap);}
inline void glDisable(GLenum cap)                                           {sw_context->glDisable(cap);}
inline void glScalef(GLfloat x, GLfloat y, GLfloat z)                       {sw_context->glScalef(x, y, z);}
inline void glTranslatef(GLfloat x, GLfloat y, GLfloat z)                   {sw_context->glTranslatef(x, y, z);}
inline void glRotatef(GLfloat angle, GLfloat x, GLfloat y, GLfloat z)       {sw_context->glRotatef(angle, x, y, z);}
inline void glOrtho(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble near_val, GLdouble far_val)
                                                                            {sw_context->glOrtho(left, right, bottom, top, near_val, far_val);}

// Заглушки
inline void glShadeModel(GLenum) {}
inline void glEnableClientState(GLenum) {}
inline GLboolean glIsEnabled(GLenum) {return GL_TRUE;}
inline void glClear(GLbitfield) {}
inline void glTexParameteri(GLenum, GLenum, GLint) {}
inline void glMaterialf(GLenum, GLenum, GLfloat) {}

#endif // SWRAST_WIDGET_H

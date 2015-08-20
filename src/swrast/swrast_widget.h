#ifndef SWRAST_WIDGET_H
#define SWRAST_WIDGET_H
#if defined USE_SWRAST

#include <QWidget>
#include <QPoint>
#include <QPaintEngine>
#include <QPaintDevice>
#include <QColor>
#include <QVector>
#include <QTransform>
#include <QResizeEvent>
#include "swrast_common.h"

// Одного источника пока хватит за глаза
#define SW_LIGHT_MAX 1

// Класс виджет с софтварным растеризатором
class SWRastWidget : public QWidget
{
    Q_OBJECT

public:
    // Трехмерная точка (нормали, вершины)
    template<typename T> struct point3d
    {
        T x, y, z;
        point3d(T x = (T)0, T y = (T)0, T z = (T)0)
        {
            this->x = x;
            this->y = y;
            this->z = z;
        }
    };
    typedef point3d<float> point3df;
    typedef point3d<int> point3di;

    // Двумерная точка (текстуры)
    template<typename T> struct point2d
    {
        T x, y;
        point2d(T x = (T)0, T y = (T)0)
        {
            this->x = x;
            this->y = y;
        }
    };
    typedef point2d<float> point2df;
    typedef point2d<int> point2di;

    // Источник света
    struct light
    {
        GLboolean is_enabled;
        GLfloat position[4];
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

public slots:
    void updateGL();

protected:
    // Отрисовка сцены
    void paintEvent(QPaintEvent *);
    // Изменение размеров окна
    void resizeEvent(QResizeEvent * event);
    // Рисуем линию
    template<typename QPointX>
    void line(const QPointX & p1, const QPointX & p2, const QColor & color);
    // Рисуем треугольник
    template<typename QPointX>
    void triangle(const QPointX p[3], const QColor & color);

    bool is_initialized;        // Инициализировано ли?
    QImage buffer;              // Буфер изображения
    QPainter painter;           // На чем рисуем в данный момент
    QBrush background;          // Фоновый цвет
    QVector<point3df> vertex;   // Вектор из вершин
    QVector<point3df> normal;   // Вектор из нормалей
    QVector<point2df> texcoord; // Вектор из координат в текстуре
    GLenum currMatrixMode;      // Текущий glMatrixMode
    QRect currViewport;         // Текущий glViewport
    QTransform projection;      // GL_PROJECTION
    GLdouble ortho_left, ortho_right, ortho_bottom, ortho_top, ortho_near, ortho_far;   // Параметры glOrtho
    light lights[SW_LIGHT_MAX]; // Источники света
    light material;             // Материал (ну да, он как свет)

private:
    friend void glLightfv(GLenum light, GLenum pname, const GLfloat * params);
    friend void glMaterialf(GLenum face, GLenum pname, GLfloat param);
    friend void glMaterialfv(GLenum face, GLenum pname, const GLfloat * params);
    friend void glBindTexture(GLenum target, GLuint texture);
    friend void glNormal3fv(const GLfloat * v);
    friend void glTexCoord2f(GLfloat s, GLfloat t);
    friend void glVertex3fv(const GLfloat * v);
    friend void glBegin(GLenum mode);
    friend void glEnd();
    friend void glMatrixMode(GLenum mode);
    friend void glLoadIdentity();
    friend void glOrtho(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble near_val, GLdouble far_val);
    friend void glViewport(GLint x, GLint y, GLsizei width, GLsizei height);
    friend void glEnable(GLenum cap);
    friend void glDisable(GLenum cap);
};

// Аналоги OpenGL функций
void glLightfv(GLenum light, GLenum pname, const GLfloat * params);
void glMaterialf(GLenum face, GLenum pname, GLfloat param);
void glMaterialfv(GLenum face, GLenum pname, const GLfloat * params);
void glBindTexture(GLenum target, GLuint texture);
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

// Заглушки
inline void glShadeModel(GLenum) {}
inline void glEnableClientState(GLenum) {}
inline GLboolean glIsEnabled(GLenum) {return GL_TRUE;}

#endif // defined USE_SWRAST
#endif // SWRAST_WIDGET_H

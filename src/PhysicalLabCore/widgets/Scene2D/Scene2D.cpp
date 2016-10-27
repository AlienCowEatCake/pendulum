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

#include "Scene2D.h"

#include <cmath>
#include <QtGlobal>
#include <QPainter>
#include <QString>

Scene2D::Scene2D(bool haveNegativeY, QWidget* parent)
    : QWidget(parent), m_haveNegativeY(haveNegativeY), m_settings(QString::fromLatin1("Scene2D"))
{
    // Мин/макс значения рисуемой области в локальных координатах
    m_minX_local = 0.0f;
    m_minY_local = (m_haveNegativeY ? -1.0f : 0.0f);
    m_maxX_local = 1.0f;
    m_maxY_local = 1.0f;
    // Локальный размер рисуемой области без учета осей, подписей и прочего
    m_sizeX_local = m_maxX_local - m_minX_local;
    m_sizeY_local = m_maxY_local - m_minY_local;
}

// Подгонка минимальных/максимальных значений и числа линий, для нормальной рисовки сетки
void Scene2D::adjustAxis(float & min, float & max, int & numTicks)
{
    const float axis_epsilon = 1.0f / 10000.0f;
    if(max - min < axis_epsilon)
    {
        min -= 2.0f * axis_epsilon;
        max += 2.0f * axis_epsilon;
    }

    const int minTicks = 6;
    double grossStep = static_cast<double>(max - min) / minTicks;
    double step = std::pow(10, std::floor(std::log10(grossStep)));

    if (5 * step < grossStep)
        step *= 5;
    else if (2 * step < grossStep)
        step *= 2;

    numTicks = static_cast<int>(std::ceil(max / step) - std::floor(min / step));
    min = static_cast<float>(std::floor(min / step) * step);
    max = static_cast<float>(std::ceil(max / step) * step);
}

/// @brief resize - событие при изменении размера виджета
void Scene2D::resize(float x0, float x1, float y0, float y1)
{
    m_minX = x0;
    m_minY = y0;
    m_maxX = x1;
    m_maxY = y1;

    // Поправляем значения мин / макс чтобы влазило в сетку
    adjustAxis(m_minX, m_maxX, m_numTicksX);
    adjustAxis(m_minY, m_maxY, m_numTicksY);
    m_sizeX = m_maxX - m_minX;
    m_sizeY = m_maxY - m_minY;

    m_scaleX = m_sizeX_local / m_sizeX;
    m_scaleY = m_sizeY_local / m_sizeY;
}

// Функция перевода координат сцены в координаты окна
QPoint Scene2D::toWindow(float x, float y) const
{
    // В OpenGL это был бы glOrtho
    const float gl_x0 = -0.06f;
    const float gl_x1 = 1.012f;
    const float gl_y0 = (m_haveNegativeY ? -1.03f : -0.07f);
    const float gl_y1 = (m_haveNegativeY ? 1.03f : 1.02f);
    const float gl_hx = gl_x1 - gl_x0;
    const float gl_hy = gl_y1 - gl_y0;
    // Перевод
    int xl = static_cast<int>((x - gl_x0) / gl_hx * static_cast<float>(width()));
    int yl = height() - static_cast<int>((y - gl_y0) / gl_hy * static_cast<float>(height()));
    return QPoint(xl, yl);
}

/// @brief settings - Получить SettingsWrapper
SettingsWrapper & Scene2D::settings()
{
    return m_settings;
}

const SettingsWrapper & Scene2D::settings() const
{
    return m_settings;
}

/// @brief paintEvent - Событие рисования
void Scene2D::paintEvent(QPaintEvent * event)
{
    Q_UNUSED(event);
    drawGraph(this);
}

/// @brief drawGraph - Обобщённое событие отрисовки графика
void Scene2D::drawGraph(QPaintDevice * device)
{
    // Цвет и ширина для всех линий графиков
    QColor currentGridSmallColor = gridSmallColor();
    QColor currentGridBigColor   = gridBigColor();
    QColor currentAxisColor      = axisColor();
    QColor currentPlotColor      = plotColor();
    qreal currentGridSmallWidth  = gridSmallWidth();
    qreal currentGridBigWidth    = gridBigWidth();
    qreal currentAxisWidth       = axisWidth();
    qreal currentPlotWidth       = plotWidth();

    // Поехали рисовать
    QPainter painter;
    painter.begin(device);
    painter.setViewport(0, 0, device->width(), device->height());
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    painter.fillRect(0, 0, device->width(), device->height(), QBrush(Qt::white));

    // Координатная сетка
    painter.setPen(QPen(currentGridSmallColor, currentGridSmallWidth));
    for(int i = 0; i <= m_numTicksX * 5; i++)
    {
        float x = static_cast<float>(i) / static_cast<float>(m_numTicksX) * m_sizeX_local;
        x *= 0.2f;
        x += m_minX_local;
        painter.drawLine(toWindow(x, m_minY_local), toWindow(x, m_maxX_local));
    }
    for(int i = 0; i <= m_numTicksY * 5; i++)
    {
        float y = static_cast<float>(i) / static_cast<float>(m_numTicksY) * m_sizeY_local;
        y *= 0.2f;
        y += m_minY_local;
        painter.drawLine(toWindow(m_minX_local, y), toWindow(m_maxX_local, y));
    }
    painter.setPen(QPen(currentGridBigColor, currentGridBigWidth));
    for(int i = 0; i <= m_numTicksX; i++)
    {
        float x = static_cast<float>(i) / static_cast<float>(m_numTicksX) * m_sizeX_local;
        x += m_minX_local;
        painter.drawLine(toWindow(x, m_minY_local), toWindow(x, m_maxX_local));
    }
    for(int i = 0; i <= m_numTicksY; i++)
    {
        float y = static_cast<float>(i) / static_cast<float>(m_numTicksY) * m_sizeY_local;
        y += m_minY_local;
        painter.drawLine(toWindow(m_minX_local, y), toWindow(m_maxX_local, y));
    }

    // Координатные оси
    painter.setPen(QPen(currentAxisColor, currentAxisWidth));
    painter.drawLine(toWindow(0.0f, m_minY_local), toWindow(0.0f, m_maxY_local));
    painter.drawLine(toWindow(0.0f, 0.0f), toWindow(m_maxX_local, 0.0f));

    // Отрисовка шкалы
#if defined (Q_OS_WIN) && (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)) // Почему-то в Qt5 под Win гигантские шрифты
    QFont serifFont(tr("Times"), 9);
#elif defined (Q_OS_MAC)
    QFont serifFont(tr("Times"), 15);
#else
    QFont serifFont(tr("Times"), 11);
#endif
    painter.setFont(serifFont);
    painter.setPen(QPen(Qt::black));
    for(int i = 1; i < m_numTicksX; i++)
    {
        float x = static_cast<float>(i) / static_cast<float>(m_numTicksX);
        float x_real = static_cast<float>(std::floor((x * m_sizeX + m_minX) * 1e5 + 0.5)) * 1e-5f;
        x = x * m_sizeX_local + m_minX_local;
        QString st = QString::number(x_real);
        if(m_haveNegativeY)
            painter.drawText(toWindow(x - 0.01f, -0.06f), st);
        else
            painter.drawText(toWindow(x - 0.01f, -0.04f), st);
    }
    for(int i = 0; i < m_numTicksY; i++)
    {
        float y = static_cast<float>(i) / static_cast<float>(m_numTicksY);
        float y_real = static_cast<float>(std::floor((y * m_sizeY + m_minY) * 1e5 + 0.5)) * 1e-5f;
        y = y * m_sizeY_local + m_minY_local;
        QString st = QString(QString::fromLatin1("%1")).arg(y_real, 5, 'g', -1, QChar::fromLatin1(' '));
        painter.drawText(toWindow(-0.05f, y - 0.01f), st);
    }

    // Подписи осей
    serifFont.setBold(true);
    painter.setFont(serifFont);
//    painter.setBackgroundMode(Qt::OpaqueMode);
//    painter.setBackground(QBrush(Qt::white));
    if(m_haveNegativeY)
    {
        painter.drawText(toWindow(0.97f, -0.06f), m_labelX);
        painter.drawText(toWindow(-0.0545f, 0.955f), m_labelY);
    }
    else
    {
        painter.drawText(toWindow(0.97f, -0.04f), m_labelX);
        painter.drawText(toWindow(-0.0545f, 0.98f), m_labelY);
    }
//    painter.setBackgroundMode(Qt::TransparentMode);

    // Отрисовка графика
    painter.setPen(QPen(currentPlotColor, currentPlotWidth));
    int maslength = m_arrX.size();
    for(int i = 1; i < maslength; i++)
        painter.drawLine(toWindow(m_arrX[i-1], m_arrY[i-1]), toWindow(m_arrX[i], m_arrY[i]));

    painter.end();
}

/// @brief scaleX - Получить коэффициент масштабирования по оси X
float Scene2D::scaleX() const
{
    return m_scaleX;
}

/// @brief scaleY - Получить коэффициент масштабирования по оси Y
float Scene2D::scaleY() const
{
    return m_scaleY;
}

/// @brief arrX - Получить ссылку на вектор из значений по оси абсцисс
QVector<float> & Scene2D::arrX()
{
    return m_arrX;
}

/// @brief arrX - Получить ссылку на вектор из значений по оси абсцисс
const QVector<float> & Scene2D::arrX() const
{
    return m_arrX;
}

/// @brief arrY - Получить ссылку на вектор из значений по оси ординат
QVector<float> & Scene2D::arrY()
{
    return m_arrY;
}

/// @brief arrY - Получить константную ссылку на вектор из значений по оси ординат
const QVector<float> & Scene2D::arrY() const
{
    return m_arrY;
}

/// @brief setLabelX - Установить подпись оси X
void Scene2D::setLabelX(const QString &labelX)
{
    m_labelX = labelX;
}

/// @brief setLabelY - Установить подпись оси Y
void Scene2D::setLabelY(const QString &labelY)
{
    m_labelY = labelY;
}

/// @brief setGridBigColor - Установить цвет основных линий сетки
void Scene2D::setGridBigColor(const QColor &color)
{
    m_settings.setValue(QString::fromLatin1("GridBigColor"), color);
}

/// @brief setGridSmallColor - Установить цвет дополнительных линий сетки
void Scene2D::setGridSmallColor(const QColor &color)
{
    m_settings.setValue(QString::fromLatin1("GridSmallColor"), color);
}

/// @brief setAxisColor - Установить цвет осей
void Scene2D::setAxisColor(const QColor &color)
{
    m_settings.setValue(QString::fromLatin1("AxisColor"), color);
}

/// @brief setPlotColor - Установить цвет графика
void Scene2D::setPlotColor(const QColor &color)
{
    m_settings.setValue(QString::fromLatin1("PlotColor"), color);
}

/// @brief setGridSmallWidth - Установить толщину основных линий сетки
void Scene2D::setGridSmallWidth(qreal width)
{
    m_settings.setValue(QString::fromLatin1("GridSmallWidth"), width);
}

/// @brief setGridBigWidth - Установить толщину дополнительных линий сетки
void Scene2D::setGridBigWidth(qreal width)
{
    m_settings.setValue(QString::fromLatin1("GridBigWidth"), width);
}

/// @brief setAxisWidth - Установить толщину осей
void Scene2D::setAxisWidth(qreal width)
{
    m_settings.setValue(QString::fromLatin1("AxisWidth"), width);
}

/// @brief setPlotWidth - Установить толщину графика
void Scene2D::setPlotWidth(qreal width)
{
    m_settings.setValue(QString::fromLatin1("PlotWidth"), width);
}

/// @brief gridBigColor - Получить цвет основных линий сетки
QColor Scene2D::gridBigColor()
{
    static const QColor defaultValue(Qt::gray);
    const QVariant rawValue = m_settings.value(QString::fromLatin1("GridBigColor"), defaultValue);
    const QColor value = rawValue.isValid() ? rawValue.value<QColor>() : defaultValue;
    return value.isValid() ? value : defaultValue;
}

/// @brief gridSmallColor - Получить цвет дополнительных линий сетки
QColor Scene2D::gridSmallColor()
{
    static const QColor defaultValue(Qt::lightGray);
    const QVariant rawValue = m_settings.value(QString::fromLatin1("GridSmallColor"), defaultValue);
    const QColor value = rawValue.isValid() ? rawValue.value<QColor>() : defaultValue;
    return value.isValid() ? value : defaultValue;
}

/// @brief axisColor - Получить цвет осей
QColor Scene2D::axisColor()
{
    static const QColor defaultValue(Qt::black);
    const QVariant rawValue = m_settings.value(QString::fromLatin1("AxisColor"), defaultValue);
    const QColor value = rawValue.isValid() ? rawValue.value<QColor>() : defaultValue;
    return value.isValid() ? value : defaultValue;
}

/// @brief plotColor - Получить цвет графика
QColor Scene2D::plotColor()
{
    static const QColor defaultValue(Qt::darkRed);
    const QVariant rawValue = m_settings.value(QString::fromLatin1("PlotColor"), defaultValue);
    const QColor value = rawValue.isValid() ? rawValue.value<QColor>() : defaultValue;
    return value.isValid() ? value : defaultValue;
}

/// @brief gridSmallWidth - Получить толщину основных линий сетки
qreal Scene2D::gridSmallWidth()
{
    static const qreal defaultValue = static_cast<qreal>(1.0);
    const QVariant rawValue = m_settings.value(QString::fromLatin1("GridSmallWidth"), defaultValue);
    const qreal value = rawValue.isValid() ? static_cast<qreal>(rawValue.toDouble()) : defaultValue;
    return value >= 0.0 ? value : defaultValue;
}

/// @brief gridBigWidth - Получить толщину дополнительных линий сетки
qreal Scene2D::gridBigWidth()
{
    static const qreal defaultValue = static_cast<qreal>(1.5);
    const QVariant rawValue = m_settings.value(QString::fromLatin1("GridBigWidth"), defaultValue);
    const qreal value = rawValue.isValid() ? static_cast<qreal>(rawValue.toDouble()) : defaultValue;
    return value >= 0.0 ? value : defaultValue;
}

/// @brief axisWidth - Получить толщину осей
qreal Scene2D::axisWidth()
{
    static const qreal defaultValue = static_cast<qreal>(3.0);
    const QVariant rawValue = m_settings.value(QString::fromLatin1("AxisWidth"), defaultValue);
    const qreal value = rawValue.isValid() ? static_cast<qreal>(rawValue.toDouble()) : defaultValue;
    return value >= 0.0 ? value : defaultValue;
}

/// @brief plotWidth - Получить толщину графика
qreal Scene2D::plotWidth()
{
    static const qreal defaultValue = static_cast<qreal>(2.0);
    const QVariant rawValue = m_settings.value(QString::fromLatin1("PlotWidth"), defaultValue);
    const qreal value = rawValue.isValid() ? static_cast<qreal>(rawValue.toDouble()) : defaultValue;
    return value > 0.0 ? value : defaultValue;
}

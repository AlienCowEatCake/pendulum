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

#if !defined(PHYSICALLABCORE_CSCENE2D_H_INCLUDED)
#define PHYSICALLABCORE_CSCENE2D_H_INCLUDED

#include <QString>
#include <QWidget>
#include <QVector>
#include <QColor>
#include <QSettings>

/// @brief Класс 2D сцены для отрисовки графиков
class CScene2D : public QWidget
{
    Q_OBJECT

public:
    CScene2D(bool haveNegativeY, QWidget* parent = 0);
    /// @brief resize - Событие при изменении размера виджета
    void resize(float x0, float x1, float y0, float y1);

    /// @brief gridBigColor - Получить цвет основных линий сетки
    QColor gridBigColor();
    /// @brief gridSmallColor - Получить цвет дополнительных линий сетки
    QColor gridSmallColor();
    /// @brief axisColor - Получить цвет осей
    QColor axisColor();
    /// @brief plotColor - Получить цвет графика
    QColor plotColor();
    /// @brief gridSmallWidth - Получить толщину основных линий сетки
    qreal gridSmallWidth();
    /// @brief gridBigWidth - Получить толщину дополнительных линий сетки
    qreal gridBigWidth();
    /// @brief axisWidth - Получить толщину осей
    qreal axisWidth();
    /// @brief plotWidth - Получить толщину графика
    qreal plotWidth();

    /// @brief setGridBigColor - Установить цвет основных линий сетки
    void setGridBigColor(const QColor &color);
    /// @brief setGridSmallColor - Установить цвет дополнительных линий сетки
    void setGridSmallColor(const QColor &color);
    /// @brief setAxisColor - Установить цвет осей
    void setAxisColor(const QColor &color);
    /// @brief setPlotColor - Установить цвет графика
    void setPlotColor(const QColor &color);
    /// @brief setGridSmallWidth - Установить толщину основных линий сетки
    void setGridSmallWidth(qreal width);
    /// @brief setGridBigWidth - Установить толщину дополнительных линий сетки
    void setGridBigWidth(qreal width);
    /// @brief setAxisWidth - Установить толщину осей
    void setAxisWidth(qreal width);
    /// @brief setPlotWidth - Установить толщину графика
    void setPlotWidth(qreal width);

    /// @brief setLabelX - Установить подпись оси X
    void setLabelX(const QString & labelX);
    /// @brief setLabelY - Установить подпись оси Y
    void setLabelY(const QString & labelY);

    /// @brief arrX - Получить ссылку на вектор из значений по оси абсцисс
    QVector<float> & arrX();
    /// @brief arrX - Получить константную ссылку на вектор из значений по оси абсцисс
    const QVector<float> & arrX() const;
    /// @brief arrY - Получить ссылку на вектор из значений по оси ординат
    QVector<float> & arrY();
    /// @brief arrY - Получить константную ссылку на вектор из значений по оси ординат
    const QVector<float> & arrY() const;

    /// @brief scaleX - Получить коэффициент масштабирования по оси X
    float scaleX() const;
    /// @brief scaleY - Получить коэффициент масштабирования по оси Y
    float scaleY() const;

protected:
    /// @brief paintEvent - Событие рисования
    void paintEvent(QPaintEvent *event);

private:
    // Коэффициенты масштабирования по осям x и y
    float m_scaleX, m_scaleY;

    // Подписи осей
    QString m_labelX, m_labelY;
    // Сами значения для графика
    QVector<float> m_arrX, m_arrY;

    // Подгонка минимальных/максимальных значений и числа линий, для нормальной рисовки сетки
    void adjustAxis(float & min, float & max, int & numTicks);
    // Тип отображения графика
    bool m_haveNegativeY;
    // Минимальные и максимальные значения в глобальных координатах
    float m_maxX, m_maxY;
    float m_minX, m_minY;
    // Число линий сетки
    int m_numTicksX, m_numTicksY;
    // Глобальный размер рисуемой области без учета осей, подписей и прочего
    float m_sizeX, m_sizeY;
    // Локальный размер рисуемой области без учета осей, подписей и прочего
    float m_sizeX_local, m_sizeY_local;
    // Мин/макс значения рисуемой области в локальных координатах
    float m_minX_local, m_minY_local;
    float m_maxX_local, m_maxY_local;

    // Функция перевода координат сцены в координаты окна
    QPoint toWindow(float x, float y) const;

    // Настройки
    QSettings m_settings;
    // Функция для записи в настройки графиков
    void saveSetting(const QString &name, const QVariant &value);
    // Функция для чтения из настроек графиков
    QVariant loadSetting(const QString &name, const QVariant &defaultValue);
};

#endif // PHYSICALLABCORE_CSCENE2D_H_INCLUDED

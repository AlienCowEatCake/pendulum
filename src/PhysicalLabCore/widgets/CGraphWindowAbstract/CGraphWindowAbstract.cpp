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

#include "CGraphWindowAbstract.h"
#include "ui_CGraphWindowAbstract.h"

#include <QApplication>
#include <QPaintEvent>
#include <QInputDialog>
#include <QColorDialog>
#include <cmath>

#include "widgets/CScene2D/CScene2D.h"

CGraphWindowAbstract::CGraphWindowAbstract(bool haveNegativeY, QWidget *parent)
    : QMainWindow(parent),
      m_ui(new Ui::CGraphWindowAbstract)
{
    m_ui->setupUi(this);
    m_scene2D = new CScene2D(haveNegativeY, m_ui->centralwidget);
    setCentralWidget(m_scene2D);
    connect(this, SIGNAL(settingsChanged()), this, SLOT(onSettingsChanged()));
}

CGraphWindowAbstract::~CGraphWindowAbstract()
{
    delete m_ui;
}

/// @brief clear - Очистка данных графика
void CGraphWindowAbstract::clear()
{
    m_scene2D->arrX().clear();
    m_scene2D->arrY().clear();
}

/// @brief setLabels - Установка заголовка окна и подписей осей
void CGraphWindowAbstract::setLabels(const QString & title, const QString & labelX, const QString & labelY)
{
    setWindowTitle(title);
    m_scene2D->setLabelX(labelX);
    m_scene2D->setLabelY(labelY);
    update();
}

/// @brief on_actionGraphColor_triggered - Слот на событие изменения цвета графика
void CGraphWindowAbstract::on_actionGraphColor_triggered()
{
    QColor oldColor = m_scene2D->plotColor();
    QColor newColor = QColorDialog::getColor(oldColor, this
#if !defined (HAVE_LESS_THAN_QT45)
        , trUtf8("Select Graph Color")
#endif
        );
    if(newColor.isValid() && newColor != oldColor)
    {
        m_scene2D->setPlotColor(newColor);
        emit settingsChanged();
    }
}

/// @brief on_actionGraphWidth_triggered - Слот на событие изменения толщины графика
void CGraphWindowAbstract::on_actionGraphWidth_triggered()
{
    bool ok = false;
    qreal oldValue = m_scene2D->plotWidth();
    double value = QInputDialog::getDouble(this, trUtf8("Width"), trUtf8("Select Graph Width:"), static_cast<double>(oldValue), 1.0, 10.0, 1, &ok);
    qreal newValue = static_cast<qreal>(value);
    if(ok && std::fabs(oldValue - newValue) > 1e-5)
    {
        m_scene2D->setPlotWidth(newValue);
        emit settingsChanged();
    }
}

/// @brief onSettingsChanged - Слот на изменение настроек
void CGraphWindowAbstract::onSettingsChanged()
{
    repaint();
}

/// @brief arrX - Получить ссылку на вектор из значений по оси абсцисс
QVector<float> & CGraphWindowAbstract::arrX()
{
    return m_scene2D->arrX();
}

/// @brief arrX - Получить константную ссылку на вектор из значений по оси абсцисс
const QVector<float> & CGraphWindowAbstract::arrX() const
{
    return m_scene2D->arrX();
}

/// @brief arrY - Получить ссылку на вектор из значений по оси ординат
QVector<float> & CGraphWindowAbstract::arrY()
{
    return m_scene2D->arrY();
}

/// @brief arrY - Получить константную ссылку на вектор из значений по оси ординат
const QVector<float> & CGraphWindowAbstract::arrY() const
{
    return m_scene2D->arrY();
}

/// @brief resizeGraph - Перерисовка виджета с графиком под новый размер
void CGraphWindowAbstract::resizeGraph(float x0, float x1, float y0, float y1)
{
    m_scene2D->resize(x0, x1, y0, y1);
}

/// @brief scaleX - Получить коэффициент масштабирования по оси X
float CGraphWindowAbstract::scaleX() const
{
    return m_scene2D->scaleX();
}

/// @brief scaleY - Получить коэффициент масштабирования по оси Y
float CGraphWindowAbstract::scaleY() const
{
    return m_scene2D->scaleY();
}

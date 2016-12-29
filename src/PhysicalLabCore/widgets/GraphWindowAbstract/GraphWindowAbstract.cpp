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

#include "GraphWindowAbstract.h"
#include "ui_GraphWindowAbstract.h"

#include <cmath>
#include <algorithm>
#include <QtGlobal>
#include <QApplication>
#include <QPaintEvent>
#include <QInputDialog>
#include <QColorDialog>
#include <QImageWriter>
#include <QFileDialog>
#include <QMessageBox>

#include "widgets/Scene2D/Scene2D.h"
#include "themes/ThemeUtils.h"
#include "utils/ImageSaver.h"
#include "utils/ObjectsConnector.h"

GraphWindowAbstract::GraphWindowAbstract(bool haveNegativeY, QWidget *parent)
    : QMainWindow(parent)
    , m_ui(new Ui::GraphWindowAbstract)
    , m_imageSaver(new ImageSaver(this))
{
    m_ui->setupUi(this);
    m_scene2D = new Scene2D(haveNegativeY, m_ui->centralwidget);
    setCentralWidget(m_scene2D);
    connect(m_ui->actionClose, SIGNAL(triggered()), this, SLOT(close()));
    const bool darkBackground = ThemeUtils::MenuHasDarkTheme(m_ui->menuFile);
    m_ui->actionSaveGraphFile->setIcon(ThemeUtils::GetIcon(ThemeUtils::ICON_SAVE_AS, darkBackground));
    m_ui->actionClose->setIcon(ThemeUtils::GetIcon(ThemeUtils::ICON_CLOSE, darkBackground));
    static const char settingsChangedID[] = "Scene2D_SettingsChangedID";
    ObjectsConnector::RegisterReceiver(settingsChangedID, this, SLOT(onSettingsChanged()));
    ObjectsConnector::RegisterEmitter(settingsChangedID, this, SIGNAL(settingsChanged()));
}

GraphWindowAbstract::~GraphWindowAbstract()
{}

/// @brief clear - Очистка данных графика
void GraphWindowAbstract::clear()
{
    m_scene2D->arrX().clear();
    m_scene2D->arrY().clear();
}

/// @brief setLabels - Установка заголовка окна и подписей осей
void GraphWindowAbstract::setLabels(const QString & title, const QString & labelX, const QString & labelY)
{
    setWindowTitle(title);
    m_scene2D->setLabelX(labelX);
    m_scene2D->setLabelY(labelY);
    update();
}

/// @brief settings - Получить SettingsWrapper
SettingsWrapper & GraphWindowAbstract::settings()
{
    return m_scene2D->settings();
}

const SettingsWrapper & GraphWindowAbstract::settings() const
{
    return m_scene2D->settings();
}

/// @brief on_actionGraphColor_triggered - Слот на событие изменения цвета графика
void GraphWindowAbstract::on_actionGraphColor_triggered()
{
    const QColor oldColor = m_scene2D->plotColor();
#if !defined (Q_OS_MAC)
    const QColor newColor = QColorDialog::getColor(oldColor, this
#if (QT_VERSION >= QT_VERSION_CHECK(4, 5, 0))
        , tr("Select Graph Color")
#endif
        );
    if(newColor.isValid() && newColor != oldColor)
    {
        m_scene2D->setPlotColor(newColor);
        emit settingsChanged();
    }
#else
   QColorDialog dialog(oldColor, this);
   dialog.setOption(QColorDialog::NoButtons);
   connect(&dialog, SIGNAL(colorSelected(const QColor&)), m_scene2D, SLOT(setPlotColor(const QColor&)));
   connect(&dialog, SIGNAL(colorSelected(const QColor&)), this, SIGNAL(settingsChanged()));
   dialog.exec();
#endif
}

/// @brief on_actionGraphWidth_triggered - Слот на событие изменения толщины графика
void GraphWindowAbstract::on_actionGraphWidth_triggered()
{
    bool ok = false;
    const qreal oldValue = m_scene2D->plotWidth();
    const double value = QInputDialog::getDouble(this, tr("Width"), tr("Enter Graph Width:"), static_cast<double>(oldValue), 1.0, 10.0, 1, &ok);
    qreal newValue = static_cast<qreal>(value);
    if(ok && std::fabs(oldValue - newValue) > 1e-5)
    {
        m_scene2D->setPlotWidth(newValue);
        emit settingsChanged();
    }
}

/// @brief on_actionSaveGraphFile_triggered - Слот на событие сохранения графика в файл
void GraphWindowAbstract::on_actionSaveGraphFile_triggered()
{
    const QString title = (windowTitle().isEmpty() ? tr("Graph") : windowTitle());
    m_imageSaver->setDefaultName(title + QString::fromLatin1(".png"));
    QImage image(m_scene2D->width(), m_scene2D->height(), QImage::Format_ARGB32_Premultiplied);
    m_scene2D->drawGraph(& image);
    m_imageSaver->save(image);
}

/// @brief onSettingsChanged - Слот на изменение настроек
void GraphWindowAbstract::onSettingsChanged()
{
    update();
}

/// @brief arrX - Получить ссылку на вектор из значений по оси абсцисс
QVector<float> & GraphWindowAbstract::arrX()
{
    return m_scene2D->arrX();
}

/// @brief arrX - Получить константную ссылку на вектор из значений по оси абсцисс
const QVector<float> & GraphWindowAbstract::arrX() const
{
    return m_scene2D->arrX();
}

/// @brief arrY - Получить ссылку на вектор из значений по оси ординат
QVector<float> & GraphWindowAbstract::arrY()
{
    return m_scene2D->arrY();
}

/// @brief arrY - Получить константную ссылку на вектор из значений по оси ординат
const QVector<float> & GraphWindowAbstract::arrY() const
{
    return m_scene2D->arrY();
}

/// @brief resizeGraph - Перерисовка виджета с графиком под новый размер
void GraphWindowAbstract::resizeGraph(float x0, float x1, float y0, float y1)
{
    m_scene2D->resize(x0, x1, y0, y1);
}

/// @brief scaleX - Получить коэффициент масштабирования по оси X
float GraphWindowAbstract::scaleX() const
{
    return m_scene2D->scaleX();
}

/// @brief scaleY - Получить коэффициент масштабирования по оси Y
float GraphWindowAbstract::scaleY() const
{
    return m_scene2D->scaleY();
}

/// @brief settingsMenu - Получить меню с настройками окна для возможности добавления новых настроек
QMenu * GraphWindowAbstract::settingsMenu() const
{
    return m_ui->menuSettings;
}

/// @brief changeEvent - событие при изменении чего-либо (например, языка)
void GraphWindowAbstract::changeEvent(QEvent *event)
{
    QWidget::changeEvent(event);
    if(event->type() == QEvent::LanguageChange)
        m_ui->retranslateUi(this);
}


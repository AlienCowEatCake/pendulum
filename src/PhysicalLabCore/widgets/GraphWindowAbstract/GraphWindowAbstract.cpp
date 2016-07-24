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

GraphWindowAbstract::GraphWindowAbstract(bool haveNegativeY, QWidget *parent)
    : QMainWindow(parent),
      m_ui(new Ui::GraphWindowAbstract)
{
    m_ui->setupUi(this);
    m_scene2D = new Scene2D(haveNegativeY, m_ui->centralwidget);
    setCentralWidget(m_scene2D);
    connect(this, SIGNAL(settingsChanged()), this, SLOT(onSettingsChanged()));
    connect(m_ui->actionClose, SIGNAL(triggered()), this, SLOT(close()));
}

GraphWindowAbstract::~GraphWindowAbstract()
{
    delete m_ui;
}

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

/// @brief on_actionGraphColor_triggered - Слот на событие изменения цвета графика
void GraphWindowAbstract::on_actionGraphColor_triggered()
{
    QColor oldColor = m_scene2D->plotColor();
    QColor newColor = QColorDialog::getColor(oldColor, this
#if (QT_VERSION >= QT_VERSION_CHECK(4, 5, 0))
        , tr("Select Graph Color")
#endif
        );
    if(newColor.isValid() && newColor != oldColor)
    {
        m_scene2D->setPlotColor(newColor);
        emit settingsChanged();
    }
}

/// @brief on_actionGraphWidth_triggered - Слот на событие изменения толщины графика
void GraphWindowAbstract::on_actionGraphWidth_triggered()
{
    bool ok = false;
    qreal oldValue = m_scene2D->plotWidth();
    double value = QInputDialog::getDouble(this, tr("Width"), tr("Enter Graph Width:"), static_cast<double>(oldValue), 1.0, 10.0, 1, &ok);
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
    QList<QByteArray> supported = QImageWriter::supportedImageFormats();
    QString formats, formatsAll;
    for(QList<QByteArray>::iterator it = supported.begin(); it != supported.end(); ++it)
    {
        *it = (*it).toLower();
        QString format = QString::fromLatin1(*it);
        formatsAll.append(formatsAll.length() > 0 ? QString::fromLatin1(" *.") : QString::fromLatin1("*.")).append(format);
        if(formats.length() > 0)
            formats.append(QString::fromLatin1(";;"));
        formats.append(format.toUpper()).append(QString::fromLatin1(" ")).append(tr("Images"))
               .append(QString::fromLatin1(" (*.")).append(format).append(QString::fromLatin1(")"));
    }
    formatsAll.prepend(tr("All Images").append(QString::fromLatin1(" ("))).append(QString::fromLatin1(");;"));
    formats.prepend(formatsAll);

    const QByteArray extensionDefault("png");
    if(m_lastSaved.isEmpty())
    {
        QString title = (windowTitle().isEmpty() ? tr("Graph") : windowTitle());
        m_lastSaved = title.append(QString::fromLatin1(".")).append(QString::fromLatin1(extensionDefault));
    }
    QString filename = QFileDialog::getSaveFileName(this, tr("Save Image File"), m_lastSaved, formats);
    if(filename.length() == 0) return;

    QByteArray extension;
    int found = filename.lastIndexOf(QChar::fromLatin1('.'));
    if(found == -1)
    {
        filename.append(QString::fromLatin1(".")).append(QString::fromLatin1(extensionDefault));
        extension = extensionDefault;
    }
    else
    {
        extension = filename.right(filename.length() - found - 1).toLower().toLocal8Bit();
        if(std::find(supported.begin(), supported.end(), extension) == supported.end())
        {
            filename.append(QString::fromLatin1(".")).append(QString::fromLatin1(extensionDefault));
            extension = extensionDefault;
        }
    }
    m_lastSaved = filename;

    bool saved = false;

    QImage image(m_scene2D->width(), m_scene2D->height(), QImage::Format_ARGB32_Premultiplied);
    m_scene2D->drawGraph(& image);
    saved = image.save(filename);

    if(!saved)
        QMessageBox::critical(this, tr("Error"), tr("Error: Can't save file"), QMessageBox::Ok, QMessageBox::Ok);
}

/// @brief onSettingsChanged - Слот на изменение настроек
void GraphWindowAbstract::onSettingsChanged()
{
    repaint();
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

/// @brief changeEvent - событие при изменении чего-либо (например, языка)
void GraphWindowAbstract::changeEvent(QEvent *event)
{
    QWidget::changeEvent(event);
    if(event->type() == QEvent::LanguageChange)
        m_ui->retranslateUi(this);
}


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

#include "SplashScreenWindow.h"

#include <QtGlobal>
#include <QApplication>
#include <QDesktopWidget>
#include <QPoint>
#include <QTimer>
#include <QPixmap>
#include <QGraphicsPixmapItem>
#include <QGraphicsRectItem>
#include <QPaintEvent>
#if defined (QT_SVG_LIB)
#include <QGraphicsSvgItem>
#endif

SplashScreenWindow::SplashScreenWindow(QWidget * parent) :
    QGraphicsView(parent), m_graphicsItem(NULL)
{
    setScene(new QGraphicsScene(this));
    setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint |
                   Qt::WindowSystemMenuHint
#if (QT_VERSION >= QT_VERSION_CHECK(4, 5, 0))
                   | Qt::WindowCloseButtonHint
#endif
                   );
    setWindowModality(Qt::ApplicationModal);

    // Закрываем через 10 секунд, если еще живы
    QTimer::singleShot(10000, this, SLOT(close()));
    // Показываем при следующей обработке сообщений
    QTimer::singleShot(0, this, SLOT(show()));

    // Перемещение в центр экрана
    moveToCenter();

    // Отключение скроллбара
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    setRenderHint(QPainter::Antialiasing);
    setRenderHint(QPainter::TextAntialiasing);
    setRenderHint(QPainter::SmoothPixmapTransform);
}

SplashScreenWindow::~SplashScreenWindow()
{}

/// @brief setGraphicsItem - Загрузить QGraphicsItem в сцену
void SplashScreenWindow::setGraphicsItem(QGraphicsItem * graphicsItem)
{
    scene()->clear();
    resetTransform();
    m_graphicsItem = graphicsItem;
    m_graphicsItem->setFlags(QGraphicsItem::ItemClipsToShape);
    m_graphicsItem->setCacheMode(QGraphicsItem::NoCache);
    scene()->addItem(m_graphicsItem);
    QRectF boundingRect = m_graphicsItem->boundingRect();
    scene()->setSceneRect(boundingRect);
    setGeometry(boundingRect.toRect());
    setFixedSize(boundingRect.size().toSize());
    moveToCenter();
}

/// @brief setPixmap - Загрузить изображение в окно
void SplashScreenWindow::setPixmap(const QString & filename)
{
    setGraphicsItem(new QGraphicsPixmapItem(QPixmap(filename)));
}

#if defined (QT_SVG_LIB)
/// @brief setSVG - Загрузить SVG изображение в окно
void SplashScreenWindow::setSVG(const QString & filename)
{
    setGraphicsItem(new QGraphicsSvgItem(filename));
}
#endif

/// @brief setTitle - Установить заголовок окна
void SplashScreenWindow::setTitle(const QString & title)
{
#if !defined (Q_OS_MAC)
    setWindowTitle(title);
#else
    Q_UNUSED(title);
#endif
}

/// @brief keyPressEvent - Реакция на нажатие клавиши (закрытие)
void SplashScreenWindow::keyPressEvent(QKeyEvent *)
{
    close();
}

/// @brief mousePressEvent - Реакция на нажатие мыши (закрытие)
void SplashScreenWindow::mousePressEvent(QMouseEvent *)
{
    close();
}

/// @brief moveToCenter - Перемещение окна в центр экрана
void SplashScreenWindow::moveToCenter()
{
    QPoint center = QApplication::desktop()->availableGeometry().center();
    QPoint corner = QApplication::desktop()->availableGeometry().topLeft();
    center.setX(center.x() - this->width() / 2);
    center.setY(center.y() - this->height() / 2);
    if(center.x() <= corner.x() || center.y() <= corner.y())
        this->move(corner);
    else
        this->move(center);
}


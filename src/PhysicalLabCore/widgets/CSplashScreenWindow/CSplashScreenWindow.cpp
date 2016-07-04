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

#include "CSplashScreenWindow.h"
#include "ui_CSplashScreenWindow.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QPoint>
#include <QTimer>
#include <QPixmap>

CSplashScreenWindow::CSplashScreenWindow(QWidget * parent) :
    QWidget(parent),
    m_ui(new Ui::CSplashScreenWindow)
{
    m_ui->setupUi(this);
    setWindowFlags(Qt::Dialog);
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowModality(Qt::ApplicationModal);

    // Закрываем через 10 секунд, если еще живы
    QTimer::singleShot(10000, this, SLOT(close()));
    // Показываем при следующей обработке сообщений
    QTimer::singleShot(0, this, SLOT(show()));

    // Перемещение в центр экрана
    moveToCenter();
}

CSplashScreenWindow::~CSplashScreenWindow()
{
    delete m_ui;
}

/// @brief setPixmap - Загрузить изображение в окно
void CSplashScreenWindow::setPixmap(const QString & filename)
{
    QPixmap image(filename);
    if(!image.isNull())
    {
        m_ui->label->setGeometry(0, 0, image.width(), image.height());
        m_ui->label->setPixmap(image);
        adjustSize();
        moveToCenter();
    }
}

/// @brief setTitle - Установить заголовок окна
void CSplashScreenWindow::setTitle(const QString & title)
{
    setWindowTitle(title);
}

/// @brief keyPressEvent - Реакция на нажатие клавиши (закрытие)
void CSplashScreenWindow::keyPressEvent(QKeyEvent *)
{
    close();
}

/// @brief mousePressEvent - Реакция на нажатие мыши (закрытие)
void CSplashScreenWindow::mousePressEvent(QMouseEvent *)
{
    close();
}

/// @brief moveToCenter - Перемещение окна в центр экрана
void CSplashScreenWindow::moveToCenter()
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


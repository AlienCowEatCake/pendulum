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

#include "SplashScreenWindow.h"
#include "ui_SplashScreenWindow.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QPoint>
#include <QTimer>
#include <QPixmap>

SplashScreenWindow::SplashScreenWindow(QWidget * parent) :
    QWidget(parent),
    m_ui(new Ui::SplashScreenWindow)
{
    m_ui->setupUi(this);
    setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint |
                   Qt::WindowSystemMenuHint
#if !defined (HAVE_LESS_THAN_QT45)
                   | Qt::WindowCloseButtonHint
#endif
                   );
//    setAttribute(Qt::WA_DeleteOnClose);
    setWindowModality(Qt::ApplicationModal);

    // Закрываем через 10 секунд, если еще живы
    QTimer::singleShot(10000, this, SLOT(close()));
    // Показываем при следующей обработке сообщений
    QTimer::singleShot(0, this, SLOT(show()));

    // Перемещение в центр экрана
    moveToCenter();
}

SplashScreenWindow::~SplashScreenWindow()
{
    delete m_ui;
}

/// @brief setPixmap - Загрузить изображение в окно
void SplashScreenWindow::setPixmap(const QString & filename)
{
    QPixmap image(filename);
    if(!image.isNull())
    {
        m_ui->label->setGeometry(0, 0, image.width(), image.height());
        m_ui->label->setPixmap(image);
        adjustSize();
        setMinimumSize(size());
        setMaximumSize(size());
        moveToCenter();
    }
}

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

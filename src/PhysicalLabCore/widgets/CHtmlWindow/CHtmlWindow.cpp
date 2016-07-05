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

#include "CHtmlWindow.h"
#include "ui_CHtmlWindow.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QPoint>
#include <QFile>

CHtmlWindow::CHtmlWindow(QWidget * parent) :
    QWidget(parent),
    m_ui(new Ui::CHtmlWindow)
{
    m_ui->setupUi(this);
    setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::WindowSystemMenuHint |
                   Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint);
    setWindowModality(Qt::NonModal);

    // Перемещение в центр экрана
    moveToCenter();

    // Отключение скроллбара
    m_ui->textBrowser->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_ui->textBrowser->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // Разрешить открывать внешние ссылки
    m_ui->textBrowser->setOpenExternalLinks(true);
}

CHtmlWindow::~CHtmlWindow()
{
    delete m_ui;
}

/// @brief setScrollBarEnabled - Включить или отключить вертикальный скроллбар
void CHtmlWindow::setScrollBarEnabled(bool enabled)
{
    m_ui->textBrowser->setVerticalScrollBarPolicy(enabled ? Qt::ScrollBarAsNeeded : Qt::ScrollBarAlwaysOff);
}

/// @brief setTitle - Установить заголовок окна
void CHtmlWindow::setTitle(const QString & title)
{
    setWindowTitle(title);
}

/// @brief loadHtml - Загрузить html из файла
void CHtmlWindow::loadHtml(const QString & filename)
{
    QFile file(filename);
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QString htmlString = QString::fromUtf8(file.readAll());
    m_ui->textBrowser->setHtml(htmlString);
}

/// @brief setSize - Установить размер окна
void CHtmlWindow::setSize(int width, int height)
{
    m_ui->textBrowser->setGeometry(0, 0, width, height);
    adjustSize();
    setMinimumSize(size());
    setMaximumSize(size());
    moveToCenter();
}


/// @brief moveToCenter - Перемещение окна в центр экрана
void CHtmlWindow::moveToCenter()
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


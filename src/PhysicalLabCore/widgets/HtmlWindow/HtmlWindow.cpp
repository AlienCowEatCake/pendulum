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

#include "HtmlWindow.h"
#include "ui_HtmlWindow.h"

#include <QtGlobal>
#include <QApplication>
#include <QDesktopWidget>
#include <QPoint>
#include <QFile>

HtmlWindow::HtmlWindow(QWidget * parent) :
    QWidget(parent),
    m_ui(new Ui::HtmlWindow)
{
    if(parent)
    {
        // Окно не должно быть поверх родительского окна, оно должно уметь уходить на
        // задний план, но при этом должно уничтожиться при уничтожении родителя.
        connect(parent, SIGNAL(destroyed(QObject*)), this, SLOT(deleteLater()));
        setParent(NULL);
    }
    m_ui->setupUi(this);
    setWindowFlags(Qt::Window | Qt::CustomizeWindowHint | Qt::WindowTitleHint |
                   Qt::WindowSystemMenuHint | Qt::WindowMinimizeButtonHint
#if (QT_VERSION >= QT_VERSION_CHECK(4, 5, 0))
                   | Qt::WindowCloseButtonHint
#endif
                   );
    setWindowModality(Qt::NonModal);

    // Перемещение в центр экрана
    moveToCenter();

    // Отключение скроллбара
    m_ui->textBrowser->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_ui->textBrowser->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // Разрешить открывать внешние ссылки
    m_ui->textBrowser->setOpenExternalLinks(true);
}

HtmlWindow::~HtmlWindow()
{
    delete m_ui;
}

/// @brief setScrollBarEnabled - Включить или отключить вертикальный скроллбар
void HtmlWindow::setScrollBarEnabled(bool enabled)
{
    m_ui->textBrowser->setVerticalScrollBarPolicy(enabled ? Qt::ScrollBarAsNeeded : Qt::ScrollBarAlwaysOff);
}

/// @brief setTitle - Установить заголовок окна
void HtmlWindow::setTitle(const QString & title)
{
    setWindowTitle(title);
}

/// @brief loadHtml - Загрузить html из файла
void HtmlWindow::loadHtml(const QString & filename)
{
    QFile file(filename);
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QString htmlString = QString::fromUtf8(file.readAll());
    m_ui->textBrowser->setHtml(htmlString);
}

/// @brief setSize - Установить размер окна
void HtmlWindow::setSize(int width, int height)
{
    m_ui->textBrowser->setGeometry(0, 0, width, height);
    adjustSize();
    setFixedSize(size());
    moveToCenter();
}


/// @brief moveToCenter - Перемещение окна в центр экрана
void HtmlWindow::moveToCenter()
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


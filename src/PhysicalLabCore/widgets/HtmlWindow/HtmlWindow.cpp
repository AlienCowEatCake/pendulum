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

#include "HtmlWindow.h"

#include <QtGlobal>
#include <QApplication>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QScreen>
#else
#include <QDesktopWidget>
#endif
#include <QPoint>
#include <QFile>
#include <QTextBrowser>
#include <QVBoxLayout>

HtmlWindow::HtmlWindow(QWidget * parent)
    : QWidget(parent)
    , m_textBrowser(new QTextBrowser(this))
{
    QVBoxLayout * layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(m_textBrowser);

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
    m_textBrowser->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_textBrowser->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // Разрешить открывать внешние ссылки
    m_textBrowser->setOpenExternalLinks(true);
}

/// @brief setScrollBarEnabled - Включить или отключить вертикальный скроллбар
void HtmlWindow::setScrollBarEnabled(bool enabled)
{
    m_textBrowser->setVerticalScrollBarPolicy(enabled ? Qt::ScrollBarAsNeeded : Qt::ScrollBarAlwaysOff);
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
    const QString htmlString = QString::fromUtf8(file.readAll());
    m_textBrowser->setHtml(htmlString);
}

/// @brief setSize - Установить фиксированный размер окна
void HtmlWindow::setSize(int width, int height)
{
    setGeometry(0, 0, width, height);
    setFixedSize(size());
    setWindowFlags(windowFlags() & ~Qt::WindowMaximizeButtonHint);
    moveToCenter();
}

/// @brief setPreferredSize - Установить предпочтительный не фиксированный размер окна
void HtmlWindow::setPreferredSize(int width, int height)
{
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    QScreen *screen = qApp->primaryScreen();
    const QRect availableGeometry = screen->availableGeometry();
#else
    const QRect availableGeometry = QApplication::desktop()->availableGeometry();
#endif
    const QSize newSize(qMin(availableGeometry.width(), width), qMin(availableGeometry.height(), height));
    setGeometry(0, 0, newSize.width(), newSize.height());
    setMinimumSize(size());
    setWindowFlags(windowFlags() | Qt::WindowMaximizeButtonHint);
    moveToCenter();
    if(newSize.width() < width && m_textBrowser->horizontalScrollBarPolicy() == Qt::ScrollBarAlwaysOff)
        m_textBrowser->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    if(newSize.height() < height && m_textBrowser->verticalScrollBarPolicy() == Qt::ScrollBarAlwaysOff)
        m_textBrowser->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
}

/// @brief moveToCenter - Перемещение окна в центр экрана
void HtmlWindow::moveToCenter()
{
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    QScreen *screen = qApp->primaryScreen();
    QPoint center = screen->availableGeometry().center();
    QPoint corner = screen->availableGeometry().topLeft();
#else
    QPoint center = QApplication::desktop()->availableGeometry().center();
    QPoint corner = QApplication::desktop()->availableGeometry().topLeft();
#endif
    center.setX(center.x() - this->width() / 2);
    center.setY(center.y() - this->height() / 2);
    if(center.x() <= corner.x() || center.y() <= corner.y())
        this->move(corner);
    else
        this->move(center);
}


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

#if !defined (PHYSICALLABCORE_SPLASHSCREENWINDOW_H_INCLUDED)
#define PHYSICALLABCORE_SPLASHSCREENWINDOW_H_INCLUDED

#include <QGraphicsView>
#include <QString>

/// @brief Класс окна-заставки
class SplashScreenWindow : public QGraphicsView
{
    Q_OBJECT

public:
    explicit SplashScreenWindow(QWidget * parent = 0);
    ~SplashScreenWindow();

    /// @brief setPixmap - Загрузить изображение в окно
    void setPixmap(const QString & filename);
#if defined (QT_SVG_LIB)
    /// @brief setSVG - Загрузить SVG изображение в окно
    void setSVG(const QString & filename);
#endif
    /// @brief setTitle - Установить заголовок окна
    void setTitle(const QString & title);

protected:
    /// @brief keyPressEvent - Реакция на нажатие клавиши (закрытие)
    void keyPressEvent(QKeyEvent *);
    /// @brief mousePressEvent - Реакция на нажатие мыши (закрытие)
    void mousePressEvent(QMouseEvent *);
    /// @brief closeEvent - Перехват закрытия окна
    void closeEvent(QCloseEvent *);

private slots:
    /// @brief tryFirstClose - Закрыть окно, если оно не было закрыто ранее
    void tryFirstClose();

private:
    /// @brief moveToCenter - Перемещение окна в центр экрана
    void moveToCenter();

    /// @brief setGraphicsItem - Загрузить QGraphicsItem в сцену
    void setGraphicsItem(QGraphicsItem * graphicsItem);

    QGraphicsItem * m_graphicsItem;
    bool m_wasClosed;
};

#endif // PHYSICALLABCORE_SPLASHSCREENWINDOW_H_INCLUDED


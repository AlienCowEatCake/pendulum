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

#if !defined(PHYSICALLABCORE_CHTMLWINDOW_H_INCLUDED)
#define PHYSICALLABCORE_CHTMLWINDOW_H_INCLUDED

#include <QWidget>
#include <QString>

namespace Ui {
class CHtmlWindow;
}

class CHtmlWindow : public QWidget
{
    Q_OBJECT
    
public:
    explicit CHtmlWindow(QWidget * parent = 0);
    ~CHtmlWindow();

    /// @brief setScrollBarEnabled - Включить или отключить вертикальный скроллбар
    void setScrollBarEnabled(bool enabled = true);
    /// @brief setTitle - Установить заголовок окна
    void setTitle(const QString & title);
    /// @brief loadHtml - Загрузить html из файла
    void loadHtml(const QString & filename);
    /// @brief setSize - Установить размер окна
    void setSize(int width, int height);
    
private:
    /// @brief moveToCenter - Перемещение окна в центр экрана
    void moveToCenter();

    Ui::CHtmlWindow * m_ui;
};

#endif // PHYSICALLABCORE_CHTMLWINDOW_H_INCLUDED


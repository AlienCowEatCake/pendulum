/* 
   Copyright (C) 2011-2014,
        Mikhail Alexandrov  <alexandroff.m@gmail.com>
        Andrey Kurochkin    <andy-717@yandex.ru>
        Peter Zhigalov      <peter.zhigalov@gmail.com>

   This file is part of the `pendulum' program.

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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFrame>
#include "gframe.h"
#include "gframe1.h"
#include "gframe2.h"
#include "cscene3d.h"
#include "help.h"
#include "author.h"
#include "manual.h"
#include "license.h"
#include "frame.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:

    QTimer *qtmr;
    bool buttonflag, io;
    Frame *fr;
    gframe *speed;
    gframe1 *delta;
    gframe2 *energy;
    help *spravka;
    author *auth;
    manual *manuala;
    license *licensef;
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
protected:

    void closeEvent(QCloseEvent *);

private slots:
    void on_action_triggered();

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_horizontalSlider_valueChanged(int value);

    void on_horizontalSlider_2_valueChanged(int value);

    void on_horizontalSlider_3_valueChanged(int value);

    void on_horizontalSlider_4_valueChanged(int value);

    void on_horizontalSlider_5_valueChanged(int value);

    void on_action_2_triggered();

    void on_action_3_triggered();

    void on_horizontalSlider_6_valueChanged(int value);

    void disptime();

    void on_action_4_triggered();

    void on_action_5_triggered();

    void on_action_6_triggered();

    void on_action_7_triggered();

private:
    Ui::MainWindow *ui; 
};

#endif // MAINWINDOW_H

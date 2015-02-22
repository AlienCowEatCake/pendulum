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

#if defined(HAVE_QT5)
#include <QtWidgets>
#else
#include <QtGui>
#endif
#include "main.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "caction.h"

#if defined(_WIN32) || defined(WIN32)

#include <QFont>
#include <QFontDatabase>

#if defined(HAVE_OLD_QT)
#include <windows.h>
#endif

// Фикс шрифтов (замена стандартных на Tahoma, если стандартные не поддерживают кириллицу)
template<typename T> void fix_fonts(T * widget)
{
    static QFontDatabase qfd;
#if defined(HAVE_OLD_QT)
    static DWORD dwVersion = (DWORD)(LOBYTE(LOWORD(GetVersion())));
    if(!qfd.families(QFontDatabase::Cyrillic).contains(widget->font().family(), Qt::CaseInsensitive) || dwVersion <= 4)
#else
    if(!qfd.families(QFontDatabase::Cyrillic).contains(widget->font().family(), Qt::CaseInsensitive))
#endif
    {
        QFont font_tahoma = widget->font();
        font_tahoma.setFamily("Tahoma");
        widget->setFont(font_tahoma);
    }
}

#endif

bool go(false);

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
#if defined(VERSION_NUMBER)
    setWindowTitle(trUtf8("Пружинный маятник в среде с сопротивлением") + QString(" ") + VERSION_NUMBER);
#else
    setWindowTitle(trUtf8("Пружинный маятник в среде с сопротивлением"));
#endif
    setCentralWidget(ui->widget);
    qtmr = new QTimer(this);
    connect(qtmr, SIGNAL(timeout()), ui->widget ,SLOT(actiontime()));
    connect(qtmr, SIGNAL(timeout()), ui->widget ,SLOT(updateGL()));
    connect(qtmr,SIGNAL(timeout()),this,SLOT(disptime()));
    //=======
    ui->lcdNumber->setDecMode();
    ui->lcdNumber->setSegmentStyle(QLCDNumber::Flat);
    //=======
    speed = new gframe;
    speed->setHidden(true);
    delta = new gframe1;
    delta->setHidden(true);
    energy = new gframe2;
    energy->setHidden(true);
    spravka = new help;
    spravka->setHidden(true);
    auth = new author;
    auth->setHidden(true);
    manuala = new manual;
    manuala->setHidden(true);
    licensef = new license;
    licensef->setHidden(true);
    //дефолтные значения(пример)
    m_action.m = 0.01;
    m_action.A0 = 0.5;
    m_action.k = 50;
    m_action.r = 3;
    ui->horizontalSlider->setValue(10);//масса груза m
    ui->horizontalSlider_2->setValue(50);//начальное смещение
    ui->horizontalSlider_3->setValue(50); //коэффициент жесткости k
    ui->horizontalSlider_4->setValue(3); //коэффициент трения r
    ui->horizontalSlider_5->setValue(100);//скорость эксперимента
    buttonflag = io = false;
    vfps = 40;
    ui->horizontalSlider_6->setValue(vfps);

    // О Qt
    connect(ui->action_about_qt,SIGNAL(triggered()),qApp,SLOT(aboutQt()));

    // Фикс шрифтов
#if defined(_WIN32) || defined(WIN32)
    fix_fonts(this);
    fix_fonts(ui->menuBar);
    fix_fonts(ui->menu);
    fix_fonts(ui->menu_2);
    fix_fonts(ui->groupBox);
    fix_fonts(ui->groupBox_2);
    fix_fonts(ui->label);
    fix_fonts(ui->label_2);
    fix_fonts(ui->label_3);
    fix_fonts(ui->label_4);
    fix_fonts(ui->label_5);
    fix_fonts(ui->label_6);
    fix_fonts(ui->label_7);
    fix_fonts(ui->label_8);
    fix_fonts(ui->label_9);
    fix_fonts(ui->label_10);
    fix_fonts(ui->label_11);
    fix_fonts(ui->label_12);
    fix_fonts(ui->label_13);
    fix_fonts(ui->label_14);
    fix_fonts(ui->pushButton);
    fix_fonts(ui->pushButton_2);
    fix_fonts(ui->action);
    fix_fonts(ui->action_2);
    fix_fonts(ui->action_3);
    fix_fonts(ui->action_4);
    fix_fonts(ui->action_5);
    fix_fonts(ui->action_6);
    fix_fonts(ui->action_7);
    fix_fonts(ui->action_about_qt);
#endif

    // окно-заставка
    fr = new Frame;
    fr->show();
    fr->startTimer(10);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_action_triggered()
{
    //скорость
    if(speed->isHidden())
        speed->setHidden(false);
    else
        speed->setHidden(true);
}

void MainWindow::on_action_2_triggered()
{
    //смещение
    if(delta->isHidden())
        delta->setHidden(false);
    else
        delta->setHidden(true);
}

void MainWindow::on_action_3_triggered()
{
    //энергия
    if(energy->isHidden())
        energy->setHidden(false);
    else
        energy->setHidden(true);
}


void MainWindow::on_pushButton_clicked()
{
    //кнопка старт/пауза
    io = false;
    if(!buttonflag)
    {
        ui->horizontalSlider->setEnabled(false);
        ui->horizontalSlider_2->setEnabled(false);
        ui->horizontalSlider_3->setEnabled(false);
        ui->horizontalSlider_4->setEnabled(false);        
        ui->pushButton->setText(trUtf8("Стоп"));
        buttonflag = true;
        qtmr->start(vfps);
        go = true;
    }
    else
    {       
        buttonflag = false;
        ui->pushButton->setText(trUtf8("Старт"));
        qtmr->stop();
        go = false;
    }
}

void MainWindow::on_pushButton_2_clicked()
{
    //кнопка стоп
    qtmr->stop();
    go = false;

    ui->horizontalSlider->setEnabled(true);
    ui->horizontalSlider_2->setEnabled(true);
    ui->horizontalSlider_3->setEnabled(true);
    ui->horizontalSlider_4->setEnabled(true);
    ui->pushButton->setEnabled(true);
    tot = 0;
    io = false; 
    if(buttonflag)
        ui->pushButton->click();
    m_action.x = m_action.A0;
    ui->lcdNumber->display(0);
    ui->widget->updateGL();
}

void MainWindow::on_horizontalSlider_valueChanged(int value)
{
    //масса груза m
    double v1;
    v1 = value/1000.0;
    v1 = ((int)(v1*100.0))/100.0;
    m_action.m = v1;
    QString st;
    st = QString::number(v1);
    ui->label_2->setText(st);
    m_action.InitBall();
    if(speed) {
        speed->clear();
        speed->update();
    }
    if(delta) {
        delta->clear();
        delta->update();
    }
    if(energy) {
        energy->clear();
        energy->update();
    }
    ui->widget->updateGL();   
}

void MainWindow::on_horizontalSlider_2_valueChanged(int value)
{
    //начальное смещение
    double v1;
    v1 = value/100.0 - 0.8;
    if(fabs(v1) < 0.001)
        v1 =0.0;
    m_action.A0 = m_action.x = v1;
    m_action.InitBall();
    QString st;
    st = QString::number(v1);
    ui->label_4->setText(st);
    ui->widget->updateGL();
    if(speed) {
        speed->clear();
        speed->update();
    }
    if(delta) {
        delta->clear();
        delta->update();
    }
    if(energy) {
        energy->clear();
        energy->update();
    } 
}

void MainWindow::on_horizontalSlider_3_valueChanged(int value)
{
    //коэффициент жесткости k    
    m_action.k = value;
    m_action.InitBall();
    QString st;
    st = QString::number(value);
    ui->label_6->setText(st);
    ui->widget->updateGL();
    if(speed) {
        speed->clear();
        speed->update();
    }
    if(delta) {
        delta->clear();
        delta->update();
    }
    if(energy) {
        energy->clear();
        energy->update();
    } 
}

void MainWindow::on_horizontalSlider_4_valueChanged(int value)
{
    //коэффициент трения r
    double n1;
    //n1 = exp((double)value/100.0)-1;
    n1 = exp((double)value/100.0*1.675)/exp(1.7)-0.18; //magic-magic
    n1 = ((int)(n1*100))/100.0;
    m_action.r = n1;
    m_action.InitBall();
    QString st;
    st = QString::number(n1);
    ui->label_8->setText(st);
    ui->widget->updateGL();
    if(speed) {
        speed->clear();
        speed->update();
    }
    if(delta) {
        delta->clear();
        delta->update();
    }
    if(energy) {
        energy->clear();
        energy->update();
    }
}

void MainWindow::on_horizontalSlider_5_valueChanged(int value)
{
    //скорость эксперимента
    QString st;
    st = QString::number(value);
    ui->label_10->setText(st);
    m_action.ktime = value/100.0;
    m_action.InitBall();
    ui->widget->updateGL();   
}


void MainWindow::on_horizontalSlider_6_valueChanged(int value)
{
    //качество картинки
    vfps = value;
    qtmr-> stop();
    if(go)
        qtmr->start(vfps);
}

void MainWindow::disptime()
{
    QString str;
    //str.setNum(tot/1000.0,g,2); //WTF?
    str.setNum(tot/1000.0,10,2);
    ui->lcdNumber->display(str);

}

void MainWindow::on_action_4_triggered()
{
    //о программе
    if(spravka->isHidden())
        spravka->setHidden(false);
    else
        spravka->setHidden(true);
}

void MainWindow::on_action_5_triggered()
{
    //разработчики
    if(auth->isHidden())
        auth->setHidden(false);
    else
        auth->setHidden(true);
}

void MainWindow::closeEvent(QCloseEvent *)
{
    delete speed;
    delete delta;
    delete energy;
    delete spravka;
    delete auth;
    delete manuala;
    delete licensef;
    delete fr;
}

void MainWindow::on_action_6_triggered()
{
    //руководство пользователя
    if(manuala->isHidden())
        manuala->setHidden(false);
    else
        manuala->setHidden(true);
}

void MainWindow::on_action_7_triggered()
{
    //лицензия
    if(licensef->isHidden())
        licensef->setHidden(false);
    else
        licensef->setHidden(true);
}

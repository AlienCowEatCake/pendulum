/* 
   Copyright (C) 2011-2016,
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

#if !defined (MAINWINDOW_H_INCLUDED)
#define MAINWINDOW_H_INCLUDED

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class HtmlWindow;
class SplashScreenWindow;
class GraphWindowSpeed;
class GraphWindowOffset;
class GraphWindowEnergy;
class PhysicalController;

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:

    explicit MainWindow(QWidget * parent = NULL);
    ~MainWindow();
    
protected:

    /// @brief Обработчик события закрытия окна
    void closeEvent(QCloseEvent *);

private slots:

    /// @brief Слот на обновление времени на дисплее
    void updateLcdDisplay();
    /// @brief Слот на нажатие кнопки старт/пауза
    void on_pushButtonStart_clicked();
    /// @brief Слот на нажатие кнопки стоп
    void on_pushButtonStop_clicked();
    /// @brief Слот на открытие графика скорости из меню
    void on_actionSpeed_triggered();
    /// @brief Слот на открытие графика смещения из меню
    void on_actionOffset_triggered();
    /// @brief Слот на открытие графика энергии из меню
    void on_actionEnergy_triggered();

    /// @brief Слот на изменение ползунка массы груза m
    void on_horizontalSliderMass_valueChanged(int value);
    /// @brief Слот на изменение ползунка начального смещения
    void on_horizontalSliderAmplitude_valueChanged(int value);
    /// @brief Слот на изменение ползунка коэффициента жесткости k
    void on_horizontalSliderRestitution_valueChanged(int value);
    /// @brief Слот на изменение ползунка коэффициента трения r
    void on_horizontalSliderDamping_valueChanged(int value);
    /// @brief Слот на изменение ползунка скорости эксперимента
    void on_horizontalSliderSpeed_valueChanged(int value);
    /// @brief Слот на изменение ползунка качества картинки
    void on_horizontalSliderQuality_valueChanged(int value);

    /// @brief Слот на открытие окна "О программе" из меню
    void on_actionAbout_triggered();
    /// @brief Слот на открытие окна "Разработчики" из меню
    void on_actionAuthors_triggered();
    /// @brief Слот на открытие окна "Руководство пользователя" из меню
    void on_actionManual_triggered();
    /// @brief Слот на открытие окна "Лицензия" из меню
    void on_actionLicense_triggered();

    /// @brief Слот на включение английского языка из меню
    void on_actionEnglish_triggered();
    /// @brief Слот на включение русского языка из меню
    void on_actionRussian_triggered();

private:
    /// @brief Функция для применения локализации
    /// @todo Это не особо рабочая заготовка, надо переписать
    void updateTranslations(QString language = QString());

    Ui::MainWindow *m_ui;

    PhysicalController *m_physicalController;

    SplashScreenWindow *m_splashWindow;
    GraphWindowSpeed *m_speedWindow;
    GraphWindowOffset *m_offsetWindow;
    GraphWindowEnergy *m_energyWindow;
    HtmlWindow *m_helpWindow;
    HtmlWindow *m_authorsWindow;
    HtmlWindow *m_manualWindow;
    HtmlWindow *m_licenseWindow;
};

#endif // MAINWINDOW_H_INCLUDED


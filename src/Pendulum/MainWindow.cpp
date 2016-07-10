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

#include "MainWindow.h"
#include "ui_MainWindow.h"

#if defined (Q_OS_WIN) && defined (USE_WIN98_WORKAROUNDS)
#include <windows.h>
#endif

#include <cmath>
#if defined (HAVE_QT5)
#include <QtWidgets>
#else
#include <QtGui>
#endif
#include <QTranslator>
#include <QPair>
#include <QMenu>
#include <QSettings>
#include <QLocale>
#include <QResizeEvent>
#include <QObject>
#include <QFont>
#include <QFontDatabase>

#include "widgets/HtmlWindow/HtmlWindow.h"
#include "widgets/SplashScreenWindow/SplashScreenWindow.h"
#include "GraphWindowSpeed.h"
#include "GraphWindowOffset.h"
#include "GraphWindowEnergy.h"
#include "PhysicalController.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    m_ui(new Ui::MainWindow),
    m_physicalController(new PhysicalController(this))
{
    m_ui->setupUi(this);
    setCentralWidget(m_ui->widget);
    m_ui->widget->setPhysicalController(m_physicalController);
    m_physicalController->connectToTimer(m_ui->widget, SLOT(updateGL()));
    m_physicalController->connectToTimer(this, SLOT(updateLcdDisplay()));
    // =======
    m_ui->lcdNumber->setDecMode();
    m_ui->lcdNumber->setSegmentStyle(QLCDNumber::Flat);
    // =======
    m_speedWindow = new GraphWindowSpeed;
    m_speedWindow->setPhysicalController(m_physicalController);
    m_speedWindow->setHidden(true);
    // =======
    m_offsetWindow = new GraphWindowOffset;
    m_offsetWindow->setPhysicalController(m_physicalController);
    m_offsetWindow->setHidden(true);
    // =======
    m_energyWindow = new GraphWindowEnergy;
    m_energyWindow->setPhysicalController(m_physicalController);
    m_energyWindow->setHidden(true);
    // =======
    m_helpWindow = new HtmlWindow;
    m_helpWindow->setHidden(true);
    m_helpWindow->setSize(880, 540);
    m_helpWindow->setScrollBarEnabled();
    // =======
    m_authorsWindow = new HtmlWindow;
    m_authorsWindow->setHidden(true);
    m_authorsWindow->setSize(670, 400);
    // =======
    m_manualWindow = new HtmlWindow;
    m_manualWindow->setHidden(true);
    m_manualWindow->setSize(880, 540);
    m_manualWindow->setScrollBarEnabled();
    // =======
    m_licenseWindow = new HtmlWindow;
    m_licenseWindow->setHidden(true);
    m_licenseWindow->setSize(560, 380);
    // =======
    m_ui->horizontalSliderMass->setValue(10);       // масса груза m
    m_ui->horizontalSliderAmplitude->setValue(50);     // начальное смещение
    m_ui->horizontalSliderRestitution->setValue(50);     // коэффициент жесткости k
    m_ui->horizontalSlider_4->setValue(3);      // коэффициент трения r
    m_ui->horizontalSliderSpeed->setValue(100);    // скорость эксперимента
    m_ui->horizontalSliderQuality->setValue(m_physicalController->timerStep());

    // О Qt
    connect(m_ui->actionAboutQt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

    // Соединяем графики друг с другом, чтобы они могли сообщать об изменении настроек
    connect(m_speedWindow, SIGNAL(settingsChanged()), m_offsetWindow, SLOT(onSettingsChanged()));
    connect(m_speedWindow, SIGNAL(settingsChanged()), m_energyWindow, SLOT(onSettingsChanged()));
    connect(m_offsetWindow, SIGNAL(settingsChanged()), m_speedWindow, SLOT(onSettingsChanged()));
    connect(m_offsetWindow, SIGNAL(settingsChanged()), m_energyWindow, SLOT(onSettingsChanged()));
    connect(m_energyWindow, SIGNAL(settingsChanged()), m_speedWindow, SLOT(onSettingsChanged()));
    connect(m_energyWindow, SIGNAL(settingsChanged()), m_offsetWindow, SLOT(onSettingsChanged()));

    // Окно-заставка
    m_splashWindow = new SplashScreenWindow;

    // Переводы и подгрузка ресурсов
    updateTranslations();
}

/// @brief Исправить отображение локализованных шрифтов под Windows
void MainWindow::fontsFix(const QString & language)
{
#if defined (Q_OS_WIN)

    // Отображение название языка -> соответствующая ему WritingSystem
    static QList<QPair<QString, QFontDatabase::WritingSystem> > writingSystemMap =
            QList<QPair<QString, QFontDatabase::WritingSystem> >()
            << qMakePair(QString("en"), QFontDatabase::Latin)
            << qMakePair(QString("ru"), QFontDatabase::Cyrillic);

    // Найдем WritingSystem для текущего языка
    QFontDatabase::WritingSystem currentWritingSystem = QFontDatabase::Any;
    for(QList<QPair<QString, QFontDatabase::WritingSystem> >::Iterator it = writingSystemMap.begin(); it != writingSystemMap.end(); ++it)
    {
        if(it->first == language)
        {
            currentWritingSystem = it->second;
            break;
        }
    }

    // Шрифт стандартный, по умолчанию
    static QFont defaultFont = qApp->font();
    // Шрифт Tahoma, если стандартный не поддерживает выбранный язык
    QFont fallbackFont = defaultFont;
    fallbackFont.setFamily("Tahoma");

    // Проверим, умеет ли стандартный шрифт писать на новом языке
    static QFontDatabase fontDatabase;
    if(!fontDatabase.families(currentWritingSystem).contains(defaultFont.family(), Qt::CaseInsensitive))
        qApp->setFont(fallbackFont);   // Если не умеет - заменим на Tahoma
    else
        qApp->setFont(defaultFont);    // Если умеет, то вернем его обратно

#if defined (USE_WIN98_WORKAROUNDS)
    // Для Win98 форсированно заменяем шрифты на Tahoma для всех не-английских локалей
    static DWORD dwVersion = (DWORD)(LOBYTE(LOWORD(GetVersion())));
    if(dwVersion <= 4)
    {
        if(language != "en")
            qApp->setFont(fallbackFont);
        else
            qApp->setFont(defaultFont);
    }
#endif

#else

    Q_UNUSED(language);

#endif
}

/// @brief Функция для применения локализации
/// @todo Это не особо рабочая заготовка, надо переписать
void MainWindow::updateTranslations(QString language)
{
    // Отображение название языка -> соответствующая ему менюшка
    static QList<QPair<QString, QAction *> > languagesMap = QList<QPair<QString, QAction *> >()
            << qMakePair(QString("en"), m_ui->actionEnglish)
            << qMakePair(QString("ru"), m_ui->actionRussian);

    // Определим системную локаль
    static QString systemLang;
    if(systemLang.isEmpty())
    {
        QString systemLocale = QLocale::system().name().toLower();
        for(QList<QPair<QString, QAction *> >::Iterator it = languagesMap.begin(); it != languagesMap.end(); ++it)
        {
            if(systemLocale.startsWith(it->first))
            {
                systemLang = it->first;
                break;
            }
        }
        if(systemLang.isEmpty())
            systemLang = "en";
    }

    // Посмотрим в настройки, не сохранен ли случайно в них язык
    QSettings settings;
    if(language.isEmpty())
        language = settings.value("Language", systemLang).toString();
    else
        settings.setValue("Language", language);

    // Удалим старый перевод, установим новый
    static QTranslator translator;
    if(!translator.isEmpty())
        qApp->removeTranslator(&translator);
    translator.load(QString(":/translations/%1").arg(language));
    qApp->installTranslator(&translator);
    m_ui->retranslateUi(this);

    // Пофиксим шрифты
    fontsFix(language);

    // Пробежим по меню и проставим галочку на нужном нам языке и снимем с остальных
    for(QList<QPair<QString, QAction *> >::Iterator it = languagesMap.begin(); it != languagesMap.end(); ++it)
        it->second->setChecked(it->first == language);

    // У кнопки старт/пауза текст зависит от состояния
    if(m_physicalController->currentState() == PhysicalController::SimulationRunning)
        m_ui->pushButtonStart->setText(tr("Stop"));
    else
        m_ui->pushButtonStart->setText(tr("Start"));

    // Перегрузим ресурсы в окнах
    setWindowTitle(trUtf8("Пружинный маятник в среде с сопротивлением"));
    m_helpWindow->loadHtml(QString(QLatin1String(":/html/help_%1.html")).arg(language));
    m_helpWindow->setTitle(trUtf8("О программе"));
    m_authorsWindow->loadHtml(QString(QLatin1String(":/html/author_%1.html")).arg(language));
    m_authorsWindow->setTitle(trUtf8("Авторы"));
    m_manualWindow->loadHtml(QString(QLatin1String(":/html/manual_%1.html")).arg(language));
    m_manualWindow->setTitle(trUtf8("Руководство пользователя"));
    m_licenseWindow->loadHtml(QString(QLatin1String(":/html/license_%1.html")).arg(language));
    m_licenseWindow->setTitle(tr("License"));
    m_splashWindow->setPixmap(":/mres/splash.png");
    m_splashWindow->setTitle(trUtf8("Пружинный маятник в среде с сопротивлением"));

    m_speedWindow->setLabels(trUtf8("Скорость"), trUtf8("t, c"), trUtf8("v, м/с"));
    m_offsetWindow->setLabels(trUtf8("Смещение"), trUtf8("t, c"), trUtf8("x, м"));
    m_energyWindow->setLabels(trUtf8("Механическая энергия"), trUtf8("t, c"), trUtf8("E, Дж"));

    // Также следует пересчитать геометрию виждетов
    QApplication::postEvent(this, new QResizeEvent(size(), size()));
}

MainWindow::~MainWindow()
{
    delete m_ui;
}

/// @brief Обработчик события закрытия окна
void MainWindow::closeEvent(QCloseEvent *)
{
    delete m_speedWindow;
    delete m_offsetWindow;
    delete m_energyWindow;
    delete m_helpWindow;
    delete m_authorsWindow;
    delete m_manualWindow;
    delete m_licenseWindow;
    delete m_splashWindow;
}

/// @brief Слот на обновление времени на дисплее
void MainWindow::updateLcdDisplay()
{
    QString str;
    str.setNum(m_physicalController->currentTime()/1000.0,10,2);
    m_ui->lcdNumber->display(str);
}

/// @brief Слот на нажатие кнопки старт/пауза
void MainWindow::on_pushButtonStart_clicked()
{
    switch(m_physicalController->currentState())
    {
    case PhysicalController::SimulationNotRunning:
        m_ui->horizontalSliderMass->setEnabled(false);
        m_ui->horizontalSliderAmplitude->setEnabled(false);
        m_ui->horizontalSliderRestitution->setEnabled(false);
        m_ui->horizontalSlider_4->setEnabled(false);
        m_ui->pushButtonStart->setText(tr("Stop"));
        m_physicalController->startSimulation();
        break;
    case PhysicalController::SimulationPaused:
        m_ui->pushButtonStart->setText(tr("Stop"));
        m_physicalController->resumeSimulation();
        break;
    case PhysicalController::SimulationRunning:
        m_ui->pushButtonStart->setText(tr("Start"));
        m_physicalController->pauseSimulation();
    }
}

/// @brief Слот на нажатие кнопки стоп
void MainWindow::on_pushButtonStop_clicked()
{
    m_physicalController->stopSimulation();
    m_ui->horizontalSliderMass->setEnabled(true);
    m_ui->horizontalSliderAmplitude->setEnabled(true);
    m_ui->horizontalSliderRestitution->setEnabled(true);
    m_ui->horizontalSlider_4->setEnabled(true);
    m_ui->pushButtonStart->setEnabled(true);
    m_ui->pushButtonStart->setText(tr("Start"));
    m_ui->lcdNumber->display(0);
    m_ui->widget->updateGL();
}

/// @brief Слот на открытие графика скорости из меню
void MainWindow::on_actionSpeed_triggered()
{
    m_speedWindow->setHidden(!m_speedWindow->isHidden());
}

/// @brief Слот на открытие графика смещения из меню
void MainWindow::on_actionOffset_triggered()
{
    m_offsetWindow->setHidden(!m_offsetWindow->isHidden());
}

/// @brief Слот на открытие графика энергии из меню
void MainWindow::on_actionEnergy_triggered()
{
    m_energyWindow->setHidden(!m_energyWindow->isHidden());
}

/// @brief Слот на изменение ползунка массы груза m
void MainWindow::on_horizontalSliderMass_valueChanged(int value)
{
    double v1 = (value / 10) / 100.0;
    m_physicalController->action().set_m(v1);
    m_ui->label_2->setText(QString::number(v1));
    m_physicalController->resetPhysicalEngine();
    m_speedWindow->update();
    m_offsetWindow->update();
    m_energyWindow->update();
    m_ui->widget->updateGL();
}

/// @brief Слот на изменение ползунка начального смещения
void MainWindow::on_horizontalSliderAmplitude_valueChanged(int value)
{
    double v1 = value / 100.0 - 0.8;
    if(std::fabs(v1) < 0.001)
        v1 = 0.0;
    m_physicalController->action().set_A0(v1);
    m_physicalController->resetPhysicalEngine();
    m_ui->label_4->setText(QString::number(v1));
    m_speedWindow->update();
    m_offsetWindow->update();
    m_energyWindow->update();
    m_ui->widget->updateGL();
}

/// @brief Слот на изменение ползунка коэффициента жесткости k
void MainWindow::on_horizontalSliderRestitution_valueChanged(int value)
{
    m_physicalController->action().set_k(value);
    m_physicalController->resetPhysicalEngine();
    m_ui->label_6->setText(QString::number(value));
    m_speedWindow->update();
    m_offsetWindow->update();
    m_energyWindow->update();
    m_ui->widget->updateGL();
}

/// @brief Слот на изменение ползунка коэффициента трения r
/// @todo Переименовать ползунок!
void MainWindow::on_horizontalSlider_4_valueChanged(int value)
{
    double n1 = std::exp(static_cast<double>(value) / 100.0 * 1.675) / std::exp(1.7) - 0.18; // magic-magic
    n1 = static_cast<int>(n1 * 100.0) / 100.0;
    m_physicalController->action().set_r(n1);
    m_physicalController->resetPhysicalEngine();
    m_ui->label_8->setText(QString::number(n1));
    m_speedWindow->update();
    m_offsetWindow->update();
    m_energyWindow->update();
    m_ui->widget->updateGL();
}

/// @brief Слот на изменение ползунка скорости эксперимента
void MainWindow::on_horizontalSliderSpeed_valueChanged(int value)
{
    m_ui->label_10->setText(QString::number(value));
    m_physicalController->setSimulationSpeed(value / 100.0);
    m_ui->widget->updateGL();
}

/// @brief Слот на изменение ползунка качества картинки
void MainWindow::on_horizontalSliderQuality_valueChanged(int value)
{
    m_physicalController->setTimerStep(value);
}

/// @brief Слот на открытие окна "О программе" из меню
void MainWindow::on_actionAbout_triggered()
{
    m_helpWindow->setHidden(!m_helpWindow->isHidden());
}

/// @brief Слот на открытие окна "Разработчики" из меню
void MainWindow::on_actionAuthors_triggered()
{
    m_authorsWindow->setHidden(!m_authorsWindow->isHidden());
}

/// @brief Слот на открытие окна "Руководство пользователя" из меню
void MainWindow::on_actionManual_triggered()
{
    m_manualWindow->setHidden(!m_manualWindow->isHidden());
}

/// @brief Слот на открытие окна "Лицензия" из меню
void MainWindow::on_actionLicense_triggered()
{
    m_licenseWindow->setHidden(!m_licenseWindow->isHidden());
}

/// @brief Слот на включение английского языка из меню
void MainWindow::on_actionEnglish_triggered()
{
    updateTranslations("en");
}

/// @brief Слот на включение русского языка из меню
void MainWindow::on_actionRussian_triggered()
{
    updateTranslations("ru");
}

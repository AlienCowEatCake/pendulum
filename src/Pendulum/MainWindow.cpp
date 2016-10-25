/* 
   Copyright (C) 2011-2016,
        Andrei V. Kurochkin     <kurochkin.andrei.v@yandex.ru>
        Mikhail E. Aleksandrov  <alexandroff.m@gmail.com>
        Peter S. Zhigalov       <peter.zhigalov@gmail.com>

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

#include <cmath>
#include <QtGlobal>
#include <QTranslator>
#include <QLibraryInfo>
#include <QPair>
#include <QLocale>
#include <QResizeEvent>
#include <QActionGroup>
#include <QMap>

#include "widgets/HtmlWindow/HtmlWindow.h"
#include "widgets/SplashScreenWindow/SplashScreenWindow.h"
#include "themes/ThemeUtils.h"
#include "utils/Workarounds.h"
#include "utils/ImageSaver.h"
#include "utils/SettingsWrapper.h"
#include "GraphWindowSpeed.h"
#include "GraphWindowDisplacement.h"
#include "GraphWindowEnergy.h"
#include "PhysicalController.h"

namespace {

const int helpWindowWidth     = 880;
const int helpWindowHeight    = 540;
const int authorsWindowWidth  = 670;
const int authorsWindowHeight = 400;
const int licenseWindowWidth  = 560;
const int licenseWindowHeight = 380;

const int sliderMassDefaultPosition           = 10;  ///< Ползунок массы груза m
const int sliderDisplacementDefaultPosition   = 50;  ///< Ползунок начального смещения
const int sliderSpringConstantDefaultPosition = 50;  ///< Ползунок коэффициента жесткости k
const int sliderDampingDefaultPosition        = 3;   ///< Ползунок коэффициента трения r
const int sliderSpeedDefaultPosition          = 100; ///< Ползунок скорости эксперимента

} // namespace

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    m_ui(new Ui::MainWindow),
    m_physicalController(new PhysicalController(this)),
    m_imageSaver(new ImageSaver(this))
{
    m_ui->setupUi(this);
    setCentralWidget(m_ui->widget);
    setAttribute(Qt::WA_DeleteOnClose);

    QActionGroup * langActions = new QActionGroup(this);
    langActions->addAction(m_ui->actionEnglish);
    langActions->addAction(m_ui->actionRussian);
    langActions->setExclusive(true);

#if defined (Q_OS_MAC)
    // Под Mac OS X из коробки выглядит настолько страшно, что приходится немного стилизовать
    QList<QGroupBox*> allGroupBoxes = findChildren<QGroupBox*>();
    for(QList<QGroupBox*>::ConstIterator it = allGroupBoxes.begin(); it != allGroupBoxes.end(); ++it)
        (*it)->setStyleSheet(QString::fromLatin1("QGroupBox::title { font-size: 12pt; margin-bottom: 0px; margin-left: 7px; margin-top: 2px; }"));
    QList<QLabel*> allLabels = findChildren<QLabel*>();
    for(QList<QLabel*>::ConstIterator it = allLabels.begin(); it != allLabels.end(); ++it)
        (*it)->setStyleSheet(QString::fromLatin1("QLabel { min-width: 35px; font-size: 12pt; }"));
#endif

    m_ui->widget->setPhysicalController(m_physicalController);
    m_physicalController->connectToTimer(m_ui->widget, SLOT(updateGL()));
    m_physicalController->connectToTimer(this, SLOT(updateLcdDisplay()));
    // =======
#if (QT_VERSION >= QT_VERSION_CHECK(4, 6, 0))
    m_ui->lcdNumber->setDigitCount(6);  // Qt 4.6+
#else
    m_ui->lcdNumber->setNumDigits(6);   // Deprecated
#endif
    m_ui->lcdNumber->setDecMode();
    m_ui->lcdNumber->setSegmentStyle(QLCDNumber::Flat);
    // =======
    m_speedWindow = new GraphWindowSpeed;
    m_speedWindow->setPhysicalController(m_physicalController);
    m_speedWindow->setHidden(true);
    // =======
    m_displacementWindow = new GraphWindowDisplacement;
    m_displacementWindow->setPhysicalController(m_physicalController);
    m_displacementWindow->setHidden(true);
    // =======
    m_energyWindow = new GraphWindowEnergy;
    m_energyWindow->setPhysicalController(m_physicalController);
    m_energyWindow->setHidden(true);
    // =======
    m_helpWindow = new HtmlWindow;
    m_helpWindow->setHidden(true);
    m_helpWindow->setPreferredSize(helpWindowWidth, helpWindowHeight);
    m_helpWindow->setScrollBarEnabled();
    // =======
    m_authorsWindow = new HtmlWindow(this);
    m_authorsWindow->setHidden(true);
    m_authorsWindow->setSize(authorsWindowWidth, authorsWindowHeight);
    // =======
    m_manualWindow = new HtmlWindow;
    m_manualWindow->setHidden(true);
    m_manualWindow->setPreferredSize(helpWindowWidth, helpWindowHeight);
    m_manualWindow->setScrollBarEnabled();
    // =======
    m_licenseWindow = new HtmlWindow(this);
    m_licenseWindow->setHidden(true);
    m_licenseWindow->setSize(licenseWindowWidth, licenseWindowHeight);
    // =======
    m_ui->horizontalSliderMass->setValue(sliderMassDefaultPosition);                     // масса груза m
    m_ui->horizontalSliderDisplacement->setValue(sliderDisplacementDefaultPosition);     // начальное смещение
    m_ui->horizontalSliderSpringConstant->setValue(sliderSpringConstantDefaultPosition); // коэффициент жесткости k
    m_ui->horizontalSliderDamping->setValue(sliderDampingDefaultPosition);               // коэффициент трения r
    m_ui->horizontalSliderSpeed->setValue(sliderSpeedDefaultPosition);                   // скорость эксперимента
    m_ui->horizontalSliderQuality->setValue(m_physicalController->timerStep());

    // О Qt
    connect(m_ui->actionAboutQt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

    // Окно-заставка
    m_splashWindow = new SplashScreenWindow(this);

#if defined (Q_OS_MAC)
    // Диалог About, используемый в проекте, ни разу не подходит на роль того About, что есть обычно под OS X.
    // Поэтому сделаем для такого случая отдельный пункт меню, который будет выполнять роль About.
    QAction * fakeAboutAction = new QAction(this);
    fakeAboutAction->setMenuRole(QAction::AboutRole);
    connect(fakeAboutAction, SIGNAL(triggered()), m_splashWindow, SLOT(show()));
    m_ui->menuHelp->addAction(fakeAboutAction);
#endif

    // Подгрузим иконки для меню
    bool darkBackground = ThemeUtils::MenuHasDarkTheme(m_ui->menuGraphs);
    const QString iconNameTemplate = QString::fromLatin1(":/menuicons/%2_%1.%3")
            .arg(darkBackground ? QString::fromLatin1("white") : QString::fromLatin1("black"));
#if defined (QT_SVG_LIB) && (QT_VERSION >= QT_VERSION_CHECK(5, 4, 0))
    const QString defaultExt = QString::fromLatin1("svg");
#else
    const QString defaultExt = QString::fromLatin1("png");
#endif
    const QString pixmapExt = QString::fromLatin1("png");
    m_ui->actionEnergy->setIcon(ThemeUtils::CreateScalableIcon(
        iconNameTemplate.arg(QString::fromLatin1("energy")).arg(defaultExt),
        QStringList(iconNameTemplate.arg(QString::fromLatin1("energy")).arg(pixmapExt))));
    m_ui->actionDisplacement->setIcon(ThemeUtils::CreateScalableIcon(
        iconNameTemplate.arg(QString::fromLatin1("displacement")).arg(defaultExt),
        QStringList(iconNameTemplate.arg(QString::fromLatin1("displacement")).arg(pixmapExt))));
    m_ui->actionSpeed->setIcon(ThemeUtils::CreateScalableIcon(
        iconNameTemplate.arg(QString::fromLatin1("speed")).arg(defaultExt),
        QStringList(iconNameTemplate.arg(QString::fromLatin1("speed")).arg(pixmapExt))));
    m_ui->actionSaveScreenshot->setIcon(ThemeUtils::GetIcon(ThemeUtils::ICON_SAVE_AS, darkBackground));
    m_ui->actionExit->setIcon(ThemeUtils::GetIcon(ThemeUtils::ICON_EXIT, darkBackground));
    m_ui->actionAbout->setIcon(ThemeUtils::GetIcon(ThemeUtils::ICON_ABOUT, darkBackground));
    m_ui->actionAuthors->setIcon(ThemeUtils::GetIcon(ThemeUtils::ICON_AUTHORS, darkBackground));
    m_ui->actionManual->setIcon(ThemeUtils::GetIcon(ThemeUtils::ICON_HELP, darkBackground));
    m_ui->actionLicense->setIcon(ThemeUtils::GetIcon(ThemeUtils::ICON_TEXT, darkBackground));
    m_ui->actionAboutQt->setIcon(ThemeUtils::GetIcon(ThemeUtils::ICON_QT, darkBackground));

    // Переводы и подгрузка ресурсов
    updateTranslations();
}

/// @brief Функция для применения локализации
void MainWindow::updateTranslations(QString language)
{
    // Отображение название языка -> соответствующая ему менюшка
    static QMap<QString, QAction *> languagesMap;
    if(languagesMap.isEmpty())
    {
        languagesMap[QString::fromLatin1("en")] = m_ui->actionEnglish;
        languagesMap[QString::fromLatin1("ru")] = m_ui->actionRussian;
    }

    // Определим системную локаль
    static QString systemLang;
    if(systemLang.isEmpty())
    {
        QString systemLocale = QLocale::system().name().toLower();
        for(QMap<QString, QAction *>::Iterator it = languagesMap.begin(); it != languagesMap.end(); ++it)
        {
            if(systemLocale.startsWith(it.key()))
            {
                systemLang = it.key();
                break;
            }
        }
        if(systemLang.isEmpty())
            systemLang = QString::fromLatin1("en");
    }

    // Посмотрим в настройки, не сохранен ли случайно в них язык
    SettingsWrapper settings;
    if(language.isEmpty())
        language = settings.value(QString::fromLatin1("Language"), systemLang).toString();
    else
        settings.setValue(QString::fromLatin1("Language"), language);

    // Удалим старый перевод, установим новый
    static QTranslator qtTranslator;
    static QTranslator appTranslator;
    if(!qtTranslator.isEmpty())
        qApp->removeTranslator(&qtTranslator);
    if(!appTranslator.isEmpty())
        qApp->removeTranslator(&appTranslator);
    qtTranslator.load(QString::fromLatin1("qt_%1").arg(language), QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    appTranslator.load(QString::fromLatin1(":/translations/%1").arg(language));
    qApp->installTranslator(&qtTranslator);
    qApp->installTranslator(&appTranslator);
    m_ui->retranslateUi(this);

    // Пофиксим шрифты
    Workarounds::FontsFix(language);

    // Проставим галочку на нужном нам языке и снимем с остальных
    languagesMap[language]->setChecked(true);

    // У кнопки старт/пауза текст зависит от состояния
    switch(m_physicalController->currentState())
    {
    case PhysicalController::SimulationNotRunning:
        m_ui->pushButtonStart->setText(tr("Start"));
        break;
    case PhysicalController::SimulationRunning:
        m_ui->pushButtonStart->setText(tr("Pause"));
        break;
    case PhysicalController::SimulationPaused:
        m_ui->pushButtonStart->setText(tr("Resume"));
        break;
    }

    // Перегрузим ресурсы в окнах
    setWindowTitle(tr("Mass Spring Damper System"));
    m_helpWindow->loadHtml(QString::fromLatin1(":/html/help_%1.html").arg(language));
    m_helpWindow->setTitle(tr("About"));
    m_authorsWindow->loadHtml(QString::fromLatin1(":/html/author_%1.html").arg(language));
    m_authorsWindow->setTitle(tr("Credits"));
    m_manualWindow->loadHtml(QString::fromLatin1(":/html/manual_%1.html").arg(language));
    m_manualWindow->setTitle(tr("User Manual"));
    m_licenseWindow->loadHtml(QString::fromLatin1(":/html/license_%1.html").arg(language));
    m_licenseWindow->setTitle(tr("License"));
#if defined (QT_SVG_LIB) && (QT_VERSION >= QT_VERSION_CHECK(5, 4, 0))
    m_splashWindow->setSVG(QString::fromLatin1(":/splash/splash_%1.svg").arg(language));
#else
    m_splashWindow->setPixmap(QString::fromLatin1(":/splash/splash_%1.png").arg(language));
#endif
    m_splashWindow->setTitle(tr("Mass Spring Damper System"));

    // Также следует пересчитать геометрию виждетов
    QApplication::postEvent(this, new QResizeEvent(size(), size()));

    // Зададим дефолтное имя файла сохранялке скриншотов
    m_imageSaver->setDefaultName(tr("Screenshot.png"));
}

MainWindow::~MainWindow()
{
    delete m_speedWindow;
    delete m_displacementWindow;
    delete m_energyWindow;
    delete m_manualWindow;
    delete m_helpWindow;
    delete m_ui;
    qApp->quit();
}

/// @brief Слот на обновление времени на дисплее
void MainWindow::updateLcdDisplay()
{
    QString str;
    str.setNum(m_physicalController->currentTime() / 1000.0, 10, 2);
    m_ui->lcdNumber->display(str);
}

/// @brief Слот на нажатие кнопки старт/пауза
void MainWindow::on_pushButtonStart_clicked()
{
    switch(m_physicalController->currentState())
    {
    case PhysicalController::SimulationNotRunning:
        m_ui->horizontalSliderMass->setEnabled(false);
        m_ui->horizontalSliderDisplacement->setEnabled(false);
        m_ui->horizontalSliderSpringConstant->setEnabled(false);
        m_ui->horizontalSliderDamping->setEnabled(false);
        m_ui->pushButtonStart->setText(tr("Pause"));
        m_physicalController->startSimulation();
        break;
    case PhysicalController::SimulationPaused:
        m_ui->pushButtonStart->setText(tr("Pause"));
        m_physicalController->resumeSimulation();
        break;
    case PhysicalController::SimulationRunning:
        m_ui->pushButtonStart->setText(tr("Resume"));
        m_physicalController->pauseSimulation();
        break;
    }
}

/// @brief Слот на нажатие кнопки стоп
void MainWindow::on_pushButtonStop_clicked()
{
    switch(m_physicalController->currentState())
    {
    case PhysicalController::SimulationRunning:
    case PhysicalController::SimulationPaused:
        m_physicalController->stopSimulation();
        m_ui->horizontalSliderMass->setEnabled(true);
        m_ui->horizontalSliderDisplacement->setEnabled(true);
        m_ui->horizontalSliderSpringConstant->setEnabled(true);
        m_ui->horizontalSliderDamping->setEnabled(true);
        m_ui->pushButtonStart->setEnabled(true);
        m_ui->pushButtonStart->setText(tr("Start"));
        m_ui->lcdNumber->display(0);
        m_ui->widget->updateGL();
        break;
    case PhysicalController::SimulationNotRunning:
        break;
    }
}

/// @brief Слот на открытие графика скорости из меню
void MainWindow::on_actionSpeed_triggered()
{
    m_speedWindow->setHidden(!m_speedWindow->isHidden());
}

/// @brief Слот на открытие графика смещения из меню
void MainWindow::on_actionDisplacement_triggered()
{
    m_displacementWindow->setHidden(!m_displacementWindow->isHidden());
}

/// @brief Слот на открытие графика энергии из меню
void MainWindow::on_actionEnergy_triggered()
{
    m_energyWindow->setHidden(!m_energyWindow->isHidden());
}

/// @brief Слот на изменение ползунка массы груза m
void MainWindow::on_horizontalSliderMass_valueChanged(int value)
{
    double v1 = value / 100.0;
    m_physicalController->action().set_m(v1);
    m_ui->labelMass->setText(QString::number(v1));
    m_physicalController->resetPhysicalEngine();
    m_speedWindow->update();
    m_displacementWindow->update();
    m_energyWindow->update();
    m_ui->widget->updateGL();
}

/// @brief Слот на изменение ползунка начального смещения
void MainWindow::on_horizontalSliderDisplacement_valueChanged(int value)
{
    double v1 = value / 100.0 - 0.8;
    if(std::fabs(v1) < 0.001)
        v1 = 0.0;
    m_physicalController->action().set_A0(v1);
    m_physicalController->resetPhysicalEngine();
    m_ui->labelAmplitude->setText(QString::number(v1));
    m_speedWindow->update();
    m_displacementWindow->update();
    m_energyWindow->update();
    m_ui->widget->updateGL();
}

/// @brief Слот на изменение ползунка коэффициента жесткости k
void MainWindow::on_horizontalSliderSpringConstant_valueChanged(int value)
{
    m_physicalController->action().set_k(value);
    m_physicalController->resetPhysicalEngine();
    m_ui->labelRestitution->setText(QString::number(value));
    m_speedWindow->update();
    m_displacementWindow->update();
    m_energyWindow->update();
    m_ui->widget->updateGL();
}

/// @brief Слот на изменение ползунка коэффициента трения r
void MainWindow::on_horizontalSliderDamping_valueChanged(int value)
{
    double n1 = std::exp(static_cast<double>(value) / 100.0 * 1.675) / std::exp(1.7) - 0.18; // magic-magic
    n1 = static_cast<int>(n1 * 100.0) / 100.0;
    m_physicalController->action().set_r(n1);
    m_physicalController->resetPhysicalEngine();
    m_ui->labelDamping->setText(QString::number(n1));
    m_speedWindow->update();
    m_displacementWindow->update();
    m_energyWindow->update();
    m_ui->widget->updateGL();
}

/// @brief Слот на изменение ползунка скорости эксперимента
void MainWindow::on_horizontalSliderSpeed_valueChanged(int value)
{
    m_ui->labelSpeed->setText(QString::number(value));
    m_physicalController->setSimulationSpeed(value / 100.0);
    m_ui->widget->updateGL();
}

/// @brief Слот на изменение ползунка качества картинки
void MainWindow::on_horizontalSliderQuality_valueChanged(int value)
{
    m_physicalController->setTimerStep(value);
}

/// @brief Слот на запрос сохранения скриншота из меню
void MainWindow::on_actionSaveScreenshot_triggered()
{
    QImage screenshot = m_ui->widget->grabFrameBuffer();
    m_imageSaver->save(screenshot);
}

/// @brief Слот на запрос выхода из меню
void MainWindow::on_actionExit_triggered()
{
    this->close();
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
    updateTranslations(QString::fromLatin1("en"));
}

/// @brief Слот на включение русского языка из меню
void MainWindow::on_actionRussian_triggered()
{
    updateTranslations(QString::fromLatin1("ru"));
}


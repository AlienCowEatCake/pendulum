#-------------------------------------------------
#
# Project created by QtCreator 2012-02-18T15:41:09
#
#-------------------------------------------------

# === Настраиваемые опции ======================================================

# Использовать высокополигональные модели
#CONFIG += use_hipoly
# Использовать софтверный растеризатор
#CONFIG += use_swrast

# ==============================================================================

TARGET = pendulum
TEMPLATE = app

INCLUDEPATH += src/Pendulum

DEFINES += QT_NO_CAST_FROM_ASCII

include(src/PhysicalLabCore/PhysicalLabCore.pri)

SOURCES += \
    src/Pendulum/Action.cpp \
    src/Pendulum/PhysicalController.cpp \
    src/Pendulum/ModelSpring.cpp \
    src/Pendulum/Scene3D.cpp \
    src/Pendulum/GraphWindowSpeed.cpp \
    src/Pendulum/GraphWindowOffset.cpp \
    src/Pendulum/GraphWindowEnergy.cpp \
    src/Pendulum/MainWindow.cpp \
    src/Pendulum/main.cpp \

HEADERS += \
    src/Pendulum/Action.h \
    src/Pendulum/PhysicalController.h \
    src/Pendulum/ModelSpring.h \
    src/Pendulum/Scene3D.h \
    src/Pendulum/GraphWindowSpeed.h \
    src/Pendulum/GraphWindowOffset.h \
    src/Pendulum/GraphWindowEnergy.h \
    src/Pendulum/MainWindow.h

FORMS += src/Pendulum/MainWindow.ui

TRANSLATIONS += \
    src/Pendulum/resources/translations/en.ts \
    src/Pendulum/resources/translations/ru.ts

win32 {
    RESOURCES += \
        src/Pendulum/resources-old/manual/win32/manual.qrc \
        src/Pendulum/resources/html/stylesheet/stylesheet-windows.qrc
    RC_FILE += src/Pendulum/resources-old/icon.rc
}

unix:!macx {
    RESOURCES += \
        src/Pendulum/resources-old/manual/linux/manual.qrc \
        src/Pendulum/resources/html/stylesheet/stylesheet-linux.qrc
}

macx {
    RESOURCES += src/Pendulum/resources-old/manual/linux/manual.qrc
    lessThan(QT_MAJOR_VERSION, 5) {
        RESOURCES += src/Pendulum/resources/html/stylesheet/stylesheet-macosx-qt4.qrc
    } else {
        RESOURCES += src/Pendulum/resources/html/stylesheet/stylesheet-macosx.qrc
    }
}

RESOURCES += \
    src/Pendulum/resources-old/mres.qrc \
    src/Pendulum/resources/menuicons/menuicons.qrc \
    src/Pendulum/resources/models/m_cyllinder.qrc \
    src/Pendulum/resources/html/help/help.qrc \
    src/Pendulum/resources/html/html.qrc \
    src/Pendulum/resources/translations/translations.qrc

use_hipoly {
    RESOURCES += \
        src/Pendulum/resources/models/hipoly/m_mass.qrc \
        src/Pendulum/resources/models/hipoly/m_scale.qrc \
        src/Pendulum/resources/models/hipoly/m_spring_end.qrc \
        src/Pendulum/resources/models/hipoly/m_spring_start.qrc \
        src/Pendulum/resources/models/hipoly/m_string.qrc \
        src/Pendulum/resources/models/hipoly/m_tripod.qrc \
        src/Pendulum/resources/models/hipoly/t_black_metall.qrc \
        src/Pendulum/resources/models/hipoly/t_dk_gray_metall.qrc \
        src/Pendulum/resources/models/hipoly/t_lt_gray_metall.qrc \
        src/Pendulum/resources/models/hipoly/t_red.qrc \
        src/Pendulum/resources/models/hipoly/t_wood.qrc \
        src/Pendulum/resources/models/hipoly/t_yellow.qrc
}
else {
    RESOURCES += \
        src/Pendulum/resources/models/lowpoly/m_mass.qrc \
        src/Pendulum/resources/models/lowpoly/m_scale.qrc \
        src/Pendulum/resources/models/lowpoly/m_spring_end.qrc \
        src/Pendulum/resources/models/lowpoly/m_spring_start.qrc \
        src/Pendulum/resources/models/lowpoly/m_string.qrc \
        src/Pendulum/resources/models/lowpoly/m_tripod.qrc \
        src/Pendulum/resources/models/lowpoly/t_black_metall.qrc \
        src/Pendulum/resources/models/lowpoly/t_dk_gray_metall.qrc \
        src/Pendulum/resources/models/lowpoly/t_lt_gray_metall.qrc \
        src/Pendulum/resources/models/lowpoly/t_red.qrc \
        src/Pendulum/resources/models/lowpoly/t_wood.qrc \
        src/Pendulum/resources/models/lowpoly/t_yellow.qrc
}

# === Сборочные директории =====================================================

DESTDIR = .
OBJECTS_DIR = build
MOC_DIR = build
RCC_DIR = build
UI_DIR = build

# === Опции компиляции =========================================================

QMAKE_RESOURCE_FLAGS += -threshold 0 -compress 9

*g++*|*clang* {
    QMAKE_CXXFLAGS_RELEASE *= -ffast-math
    QMAKE_CXXFLAGS_RELEASE *= -fno-math-errno
}


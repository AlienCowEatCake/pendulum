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

DEFINES += VERSION_NUMBER=\\\"v0.40\\\"

TARGET = pendulum
TEMPLATE = app

INCLUDEPATH += src/Pendulum

include(src/PhysicalLabCore/PhysicalLabCore.pri)

SOURCES += src/Pendulum/main.cpp \
           src/Pendulum/mainwindow.cpp \
           src/Pendulum/cscene3d.cpp \
           src/Pendulum/caction.cpp \
           src/Pendulum/cmilkshapemodel_old.cpp \
           src/Pendulum/gframe.cpp \
           src/Pendulum/gframe1.cpp \
           src/Pendulum/gframe2.cpp

HEADERS += src/Pendulum/mainwindow.h \
           src/Pendulum/cscene3d.h \
           src/Pendulum/caction.h \
           src/Pendulum/cmodel.h \
           src/Pendulum/main.h \
           src/Pendulum/gframe.h \
           src/Pendulum/gframe1.h \
           src/Pendulum/gframe2.h

FORMS   += src/Pendulum/mainwindow.ui

win32 {
  RESOURCES += src/Pendulum/resources/manual/win32/manual.qrc \
               src/Pendulum-new/resources/html/stylesheet/stylesheet-windows.qrc
  RC_FILE += src/Pendulum/resources/icon.rc
}

unix {
  RESOURCES += src/Pendulum/resources/manual/linux/manual.qrc \
               src/Pendulum-new/resources/html/stylesheet/stylesheet-linux.qrc
}

RESOURCES += src/Pendulum/resources/mres.qrc \
             src/Pendulum/resources/menuicons/menuicons.qrc \
             src/Pendulum/resources/models/m_cyllinder.qrc \
             src/Pendulum-new/resources/html/help/help.qrc \
             src/Pendulum-new/resources/html/html.qrc

use_hipoly {
  RESOURCES += src/Pendulum/resources/models/hipoly/m_mass.qrc \
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
  RESOURCES += src/Pendulum/resources/models/lowpoly/m_mass.qrc \
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

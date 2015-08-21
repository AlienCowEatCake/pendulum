#-------------------------------------------------
#
# Project created by QtCreator 2012-02-18T15:41:09
#
#-------------------------------------------------

# === Настраиваемые опции ======================================================

# Использовать высокополигональные модели
#CONFIG += use_hipoly

# ==============================================================================

DEFINES += VERSION_NUMBER=\\\"v0.39\\\"

TARGET = pendulum
TEMPLATE = app

INCLUDEPATH += src

QT += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4) {
    QT += widgets
    DEFINES += HAVE_QT5
    contains(QT_CONFIG, dynamicgl) {
        win32-g++* {
            QMAKE_LIBS += -lopengl32
        } else {
            QMAKE_LIBS += opengl32.lib
        }
        DEFINES += USE_FORCE_GL
    } else {
        contains(QT_CONFIG, opengles.) | contains(QT_CONFIG, angle) {
            error("This program requires Qt to be configured with -opengl desktop (recommended) or -opengl dynamic")
        }
    }
}

win32:lessThan(QT_VERSION, 4.5.0) {
    win32-g++*|win32-msvc|win32-msvc.net|win32-msvc200* {
        DEFINES += HAVE_OLD_QT
    }
}

SOURCES += src/main.cpp \
           src/mainwindow.cpp \
           src/frame.cpp \
           src/cscene3d.cpp \
           src/caction.cpp \
           src/cmodel.cpp \
           src/cmilkshapemodel.cpp \
           src/gframe.cpp \
           src/gframe1.cpp \
           src/gframe2.cpp \
           src/cscene2dn.cpp \
           src/help.cpp \
           src/author.cpp \
           src/manual.cpp \
           src/license.cpp

HEADERS += src/mainwindow.h \
           src/frame.h \
           src/cscene3d.h \
           src/caction.h \
           src/cmodel.h \
           src/main.h \
           src/gframe.h \
           src/gframe1.h \
           src/gframe2.h \
           src/cscene2dn.h \
           src/help.h \
           src/author.h \
           src/manual.h \
           src/license.h

FORMS   += src/frame.ui \
           src/gframe.ui \
           src/gframe1.ui \
           src/gframe2.ui \
           src/mainwindow.ui

win32 {
  FORMS += src/win32/help.ui \
           src/win32/author.ui \
           src/win32/manual.ui \
           src/win32/license.ui
  RESOURCES += src/resources/manual/win32/manual.qrc
  RC_FILE += src/resources/icon.rc
}

unix {
  FORMS += src/linux/help.ui \
           src/linux/author.ui \
           src/linux/manual.ui \
           src/linux/license.ui
  RESOURCES += src/resources/manual/linux/manual.qrc
}

RESOURCES += src/resources/mres.qrc \
             src/resources/help/help.qrc \
             src/resources/menuicons/menuicons.qrc \
             src/resources/models/m_cyllinder.qrc

use_hipoly {
  RESOURCES += src/resources/models/hipoly/m_mass.qrc \
               src/resources/models/hipoly/m_scale.qrc \
               src/resources/models/hipoly/m_spring_end.qrc \
               src/resources/models/hipoly/m_spring_start.qrc \
               src/resources/models/hipoly/m_string.qrc \
               src/resources/models/hipoly/m_tripod.qrc \
               src/resources/models/hipoly/t_black_metall.qrc \
               src/resources/models/hipoly/t_dk_gray_metall.qrc \
               src/resources/models/hipoly/t_lt_gray_metall.qrc \
               src/resources/models/hipoly/t_red.qrc \
               src/resources/models/hipoly/t_wood.qrc \
               src/resources/models/hipoly/t_yellow.qrc
}
else {
  RESOURCES += src/resources/models/lowpoly/m_mass.qrc \
               src/resources/models/lowpoly/m_scale.qrc \
               src/resources/models/lowpoly/m_spring_end.qrc \
               src/resources/models/lowpoly/m_spring_start.qrc \
               src/resources/models/lowpoly/m_string.qrc \
               src/resources/models/lowpoly/m_tripod.qrc \
               src/resources/models/lowpoly/t_black_metall.qrc \
               src/resources/models/lowpoly/t_dk_gray_metall.qrc \
               src/resources/models/lowpoly/t_lt_gray_metall.qrc \
               src/resources/models/lowpoly/t_red.qrc \
               src/resources/models/lowpoly/t_wood.qrc \
               src/resources/models/lowpoly/t_yellow.qrc
}

# === Сборочные директории =====================================================

DESTDIR = .
OBJECTS_DIR = build
MOC_DIR = build
RCC_DIR = build
UI_DIR = build

# === Опции компиляции =========================================================

QMAKE_RESOURCE_FLAGS += -threshold 0 -compress 9
CONFIG += warn_on

*g++*|*clang* {
    QMAKE_CXXFLAGS_WARN_ON *= -Wextra
    QMAKE_CXXFLAGS_RELEASE -= -O2
    QMAKE_CXXFLAGS_RELEASE *= -O3
    QMAKE_CXXFLAGS_RELEASE *= -ffast-math
    QMAKE_CXXFLAGS_RELEASE *= -fno-math-errno
    QMAKE_CXXFLAGS_RELEASE *= -DNDEBUG
}

*msvc* {
    QMAKE_CXXFLAGS_RELEASE -= -O2
    QMAKE_CXXFLAGS_RELEASE *= -Ox
    DEFINES += _CRT_SECURE_NO_WARNINGS
    DEFINES += _USE_MATH_DEFINES
}


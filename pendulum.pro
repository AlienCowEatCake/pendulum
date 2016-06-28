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

INCLUDEPATH += src/Pendulum src/PhysicalLabCore

QT += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4) {
    QT += widgets
    DEFINES += HAVE_QT5
    !use_swrast {
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
    } else {
        win32:message("Config option use_swrast may be incompatible with Qt 5 and above")
    }
}

win32:lessThan(QT_VERSION, 4.5.0) {
    win32-g++*|win32-msvc|win32-msvc.net|win32-msvc200* {
        DEFINES += HAVE_OLD_QT
    }
}

SOURCES += src/PhysicalLabCore/models/CModel.cpp \
           src/PhysicalLabCore/models/CMilkshapeModel.cpp \
           src/PhysicalLabCore/widgets/GLWidgetImpl/software/SWRastWidget.cpp \

HEADERS += src/PhysicalLabCore/models/CModel.h \
           src/PhysicalLabCore/models/CMilkshapeModel.h \
           src/PhysicalLabCore/widgets/GLWidgetImpl/GLWidgetImpl.h \
           src/PhysicalLabCore/widgets/GLWidgetImpl/native/GLFunctions.h \
           src/PhysicalLabCore/widgets/GLWidgetImpl/software/GLFunctions.h \
           src/PhysicalLabCore/widgets/GLWidgetImpl/software/GLDefines.h \
           src/PhysicalLabCore/widgets/GLWidgetImpl/software/SWRastVector.h \
           src/PhysicalLabCore/widgets/GLWidgetImpl/software/SWRastMatrix.h \
           src/PhysicalLabCore/widgets/GLWidgetImpl/software/SWRastWidget.h \


SOURCES += src/Pendulum/main.cpp \
           src/Pendulum/mainwindow.cpp \
           src/Pendulum/frame.cpp \
           src/Pendulum/cscene3d.cpp \
           src/Pendulum/caction.cpp \
           src/Pendulum/cmilkshapemodel_old.cpp \
           src/Pendulum/gframe.cpp \
           src/Pendulum/gframe1.cpp \
           src/Pendulum/gframe2.cpp \
           src/Pendulum/cscene2dn.cpp \
           src/Pendulum/help.cpp \
           src/Pendulum/author.cpp \
           src/Pendulum/manual.cpp \
           src/Pendulum/license.cpp

HEADERS += src/Pendulum/mainwindow.h \
           src/Pendulum/frame.h \
           src/Pendulum/cscene3d.h \
           src/Pendulum/caction.h \
           src/Pendulum/cmodel.h \
           src/Pendulum/main.h \
           src/Pendulum/gframe.h \
           src/Pendulum/gframe1.h \
           src/Pendulum/gframe2.h \
           src/Pendulum/cscene2dn.h \
           src/Pendulum/help.h \
           src/Pendulum/author.h \
           src/Pendulum/manual.h \
           src/Pendulum/license.h

FORMS   += src/Pendulum/frame.ui \
           src/Pendulum/gframe.ui \
           src/Pendulum/gframe1.ui \
           src/Pendulum/gframe2.ui \
           src/Pendulum/mainwindow.ui

win32 {
  FORMS += src/Pendulum/win32/help.ui \
           src/Pendulum/win32/author.ui \
           src/Pendulum/win32/manual.ui \
           src/Pendulum/win32/license.ui
  RESOURCES += src/Pendulum/resources/manual/win32/manual.qrc
  RC_FILE += src/Pendulum/resources/icon.rc
}

unix {
  FORMS += src/Pendulum/linux/help.ui \
           src/Pendulum/linux/author.ui \
           src/Pendulum/linux/manual.ui \
           src/Pendulum/linux/license.ui
  RESOURCES += src/Pendulum/resources/manual/linux/manual.qrc
}

RESOURCES += src/Pendulum/resources/mres.qrc \
             src/Pendulum/resources/help/help.qrc \
             src/Pendulum/resources/menuicons/menuicons.qrc \
             src/Pendulum/resources/models/m_cyllinder.qrc

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

use_swrast {
    QT -= opengl
    DEFINES += USE_SWRAST
#    SOURCES += \
#        src/Pendulum/swrast/swrast_widget.cpp
#    HEADERS += \
#        src/Pendulum/swrast/swrast_common.h \
#        src/Pendulum/swrast/swrast_geometry.h \
#        src/Pendulum/swrast/swrast_widget.h
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


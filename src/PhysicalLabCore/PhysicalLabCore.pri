# Использовать софтверный растеризатор
#CONFIG += use_swrast

INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD
CONFIG += object_with_source object_parallel_to_source no_batch warn_on

QT += core gui opengl

#DEFINES += QT_NO_CAST_FROM_ASCII

greaterThan(QT_MAJOR_VERSION, 4) {
    QT += widgets
    DEFINES += HAVE_QT5
    contains(QT_CONFIG, opengles.) | contains(QT_CONFIG, angle) {
        !contains(QT_CONFIG, dynamicgl) : CONFIG += use_swrast
    }
    !use_swrast {
        contains(QT_CONFIG, dynamicgl) {
            win32-g++*|win32-clang* {
                QMAKE_LIBS += -lopengl32
            } else {
                QMAKE_LIBS += opengl32.lib
            }
            DEFINES += USE_FORCE_GL
        }
    }
}

lessThan(QT_VERSION, 4.5.0) {
    DEFINES += HAVE_LESS_THAN_QT45
    win32-g++*|win32-msvc|win32-msvc.net|win32-msvc200* {
        DEFINES += USE_WIN98_WORKAROUNDS
    }
}

*g++*|*clang* {
    QMAKE_CXXFLAGS_RELEASE -= -O2
    QMAKE_CXXFLAGS_RELEASE *= -O3
    QMAKE_CXXFLAGS_RELEASE *= -DNDEBUG
}

*msvc* {
    QMAKE_CXXFLAGS_RELEASE -= -O2
    QMAKE_CXXFLAGS_RELEASE *= -Ox
    QMAKE_CXXFLAGS_RELEASE -= -GS
    QMAKE_CXXFLAGS_RELEASE *= -GS-
    DEFINES += _CRT_SECURE_NO_WARNINGS
    DEFINES += _CRT_SECURE_NO_DEPRECATE
    DEFINES += _USE_MATH_DEFINES
}

HEADERS += \
    $$files($$PWD/models/*.h) \
    $$files($$PWD/widgets/CScene3DAbstract/*.h) \
    $$files($$PWD/widgets/CGLWidgetImpl/*.h) \
    $$files($$PWD/widgets/CGLWidgetImpl/native/*.h) \
    $$files($$PWD/widgets/CGraphWindowAbstract/*.h) \
    $$files($$PWD/widgets/CScene2D/*.h) \
    $$files($$PWD/widgets/CSplashScreenWindow/*.h) \
    $$files($$PWD/widgets/CHtmlWindow/*.h)

SOURCES += \
    $$files($$PWD/models/*.cpp) \
    $$files($$PWD/widgets/CScene3DAbstract/*.cpp) \
    $$files($$PWD/widgets/CGraphWindowAbstract/*.cpp) \
    $$files($$PWD/widgets/CScene2D/*.cpp) \
    $$files($$PWD/widgets/CSplashScreenWindow/*.cpp) \
    $$files($$PWD/widgets/CHtmlWindow/*.cpp)

FORMS += \
    $$files($$PWD/widgets/CGraphWindowAbstract/*.ui) \
    $$files($$PWD/widgets/CSplashScreenWindow/*.ui) \
    $$files($$PWD/widgets/CHtmlWindow/*.ui)

use_swrast {
    QT -= opengl
    DEFINES += USE_SWRAST
    SOURCES += $$files($$PWD/widgets/CGLWidgetImpl/software/*.cpp)
    HEADERS += $$files($$PWD/widgets/CGLWidgetImpl/software/*.h)
}


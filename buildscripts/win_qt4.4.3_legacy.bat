@echo off
set V_PROJECT=pendulum
set V_VERS=0.39
set QTDIR=C:\dev\Qt\4.4.3-static
set MINGWDIR=C:\dev\MinGW\MinGW-3.4.2
set V_BUILDDIR=build_win_qt4.4.3_legacy

set PATH=%QTDIR%\bin;%MINGWDIR%\bin;%WINDIR%;%WINDIR%\System32
set INCLUDE=%QTDIR%\include\;%MINGWDIR%\include
set LIB=%QTDIR%\lib\;%MINGWDIR%\lib
set QDIR=%QTDIR%
set QMAKESPEC=win32-g++

cd "%~dp0"
cd ..
rmdir /S /Q %V_BUILDDIR% 2>nul >nul
mkdir %V_BUILDDIR%
cd %V_BUILDDIR%
qmake CONFIG+="release" ..\%V_PROJECT%.pro
mingw32-make
strip --strip-all release\%V_PROJECT%.exe
copy release\%V_PROJECT%.exe ..\%V_PROJECT%-%V_VERS%_qt4.4.3_legacy.exe

pause


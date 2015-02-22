@echo off
set V_PROJECT=pendulum
set V_VERS=0.39
set V_ARCH=x64
set V_VCVARS="C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\vcvarsall.bat"
set V_QTDIR=C:\Qt\5.4.0-msvc2013-static-x64
set V_BUILDDIR=build_win_qt5.4.0_%V_ARCH%

call %V_VCVARS% %V_ARCH%
set PATH=%V_QTDIR%\bin;%PATH%

cd "%~dp0"
cd ..
rmdir /S /Q %V_BUILDDIR% 2>nul >nul
mkdir %V_BUILDDIR%
cd %V_BUILDDIR%
qmake CONFIG+="release" ..\%V_PROJECT%.pro
nmake
copy release\%V_PROJECT%.exe ..\%V_PROJECT%-%V_VERS%_qt5.4.0_%V_ARCH%.exe
cd ..

pause


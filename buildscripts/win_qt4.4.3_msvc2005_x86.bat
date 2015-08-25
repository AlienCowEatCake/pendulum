@echo off
set V_PROJECT=pendulum
set V_VERS=0.39
set V_ARCH=x86
set V_VCVARS="C:\Program Files (x86)\Microsoft Visual Studio 8\VC\vcvarsall.bat"
set V_QTDIR=C:\Qt\qt4.4.3-msvc2005-x86-static
set V_JOM=C:\Qt\jom.exe
set V_BUILDDIR=build_win_qt4.4.3_msvc2005_%V_ARCH%

call %V_VCVARS% %V_ARCH%
set PATH=%V_QTDIR%\bin;%PATH%

cd "%~dp0"
cd ..
rmdir /S /Q %V_BUILDDIR% 2>nul >nul
mkdir %V_BUILDDIR%
cd %V_BUILDDIR%
qmake CONFIG+="release" ..\%V_PROJECT%.pro
%V_JOM% -j1
copy release\%V_PROJECT%.exe ..\%V_PROJECT%-%V_VERS%_qt4.4.3_msvc2005_%V_ARCH%.exe
cd ..

pause

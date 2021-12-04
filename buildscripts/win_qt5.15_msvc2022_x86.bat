@echo off
set PROJECT=pendulum
set ARCH=x86
set VCVARS_ARCH=x64_x86
set VCVARS="C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat"
set QTDIR=C:\Qt\5.15.2\msvc2022_static
set BUILDDIR=build_win_qt5.15_msvc2022_%ARCH%
set SUFFIX=_qt5.15_msvc2022_%ARCH%
set APP_PATH=.
set ZIP_CMD=buildscripts\helpers\zip.exe

call %VCVARS% %VCVARS_ARCH%
set PATH=%QTDIR%\bin;%PATH%

cd "%~dp0"
cd ..
rmdir /S /Q %BUILDDIR% 2>nul >nul
mkdir %BUILDDIR%
cd %BUILDDIR%
qmake -r CONFIG+="release" QTPLUGIN.imageformats="qico qsvg qtiff" ..\%PROJECT%.pro
nmake
copy %APP_PATH%\release\%PROJECT%.exe ..\%PROJECT%%SUFFIX%.exe
cd ..
%ZIP_CMD% -9r %PROJECT%%SUFFIX%.zip %PROJECT%%SUFFIX%.exe

pause

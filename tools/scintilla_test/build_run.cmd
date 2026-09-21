@echo off
chcp 65001 >nul
setlocal
set HERE=%~dp0
set QMAKE=C:\Qt\5.15.2\msvc2019_64\bin\qmake.exe
set NDD=C:\dev\code\ndd_plugins\desensitive_processor\test\Notepad--v3.8.2-win10-portable
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat" >nul
if not exist "%HERE%build" mkdir "%HERE%build"
cd /d "%HERE%build"
"%QMAKE%" "%HERE%scintilla_test.pro" -o Makefile
if errorlevel 1 exit /b 1
nmake
if errorlevel 1 exit /b 1
set PATH=C:\Qt\5.15.2\msvc2019_64\bin;%NDD%;%PATH%
"%HERE%out\scintilla_test.exe"
exit /b %errorlevel%

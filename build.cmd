@echo off
chcp 65001 >nul
setlocal
set HERE=%~dp0
set VCVARS="C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat"
set QMAKE=C:\Qt\5.15.2\msvc2019_64\bin\qmake.exe
set NDD_TEST=%~dp0test\Notepad--v3.8.2-win10-portable

if not exist %QMAKE% (
  echo [ERROR] Qt not found: %QMAKE%
  exit /b 1
)

call %VCVARS%
if errorlevel 1 (
  echo [ERROR] vcvars64.bat failed
  exit /b 1
)

if not exist "%HERE%build" mkdir "%HERE%build"
cd /d "%HERE%build"
"%QMAKE%" "%HERE%desensitive_processor.pro" -o Makefile
if errorlevel 1 (
  echo [ERROR] qmake failed
  exit /b 1
)
nmake
if errorlevel 1 (
  echo [ERROR] nmake failed
  exit /b 1
)

if exist "%NDD_TEST%\plugin" (
  copy /y "%HERE%build\out\desensitive_processor.dll" "%NDD_TEST%\plugin\" >nul
  echo [OK] built and copied to test portable NDD plugin dir
) else (
  echo [OK] built: %HERE%build\out\desensitive_processor.dll
)

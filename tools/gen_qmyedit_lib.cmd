@echo off
chcp 65001 >nul
setlocal
set HERE=%~dp0
set VCVARS="C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat"
call %VCVARS%
if errorlevel 1 (
  echo [ERROR] vcvars64.bat failed
  exit /b 1
)
set NDD_DLL=C:\Program Files\Notepad--\qmyedit_qt5.dll
if not exist "%NDD_DLL%" (
  echo [ERROR] qmyedit_qt5.dll not found: %NDD_DLL%
  exit /b 1
)
dumpbin /exports "%NDD_DLL%" > "%HERE%exports.txt"
if errorlevel 1 (
  echo [ERROR] dumpbin failed
  exit /b 1
)
py -3 "%HERE%make_def.py" "%HERE%exports.txt" "%HERE%qmyedit_qt5.def" qmyedit_qt5
if errorlevel 1 (
  echo [ERROR] make_def.py failed
  exit /b 1
)
if not exist "%HERE%..\sdk\lib" mkdir "%HERE%..\sdk\lib"
lib /def:"%HERE%qmyedit_qt5.def" /machine:x64 /out:"%HERE%..\sdk\lib\qmyedit_qt5.lib"
if errorlevel 1 (
  echo [ERROR] lib.exe failed
  exit /b 1
)
echo [OK] sdk\lib\qmyedit_qt5.lib generated

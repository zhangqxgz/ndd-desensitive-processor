@echo off
chcp 65001 >nul
set LOG=%~dp0qt_install.log
echo [START] %date% %time% > "%LOG%"
py -3 -m pip install -U aqtinstall >> "%LOG%" 2>&1
echo [PIP_EXITCODE] %errorlevel% >> "%LOG%"
py -3 -m aqt install-qt windows desktop 5.15.2 win64_msvc2019_64 -O C:\Qt --base https://mirrors.ustc.edu.cn/qtproject/ >> "%LOG%" 2>&1
echo [AQT_EXITCODE] %errorlevel% >> "%LOG%"
echo [DONE] %date% %time% >> "%LOG%"

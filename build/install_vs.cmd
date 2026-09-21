@echo off
chcp 65001 >nul
set LOG=%~dp0vs_install.log
echo [START] %date% %time% > "%LOG%"
winget install --id Microsoft.VisualStudio.2022.BuildTools --exact --source winget --accept-package-agreements --accept-source-agreements --override "--quiet --wait --norestart --add Microsoft.VisualStudio.Workload.VCTools --includeRecommended --add Microsoft.VisualStudio.Component.VC.v142.x86.x64" >> "%LOG%" 2>&1
echo [EXITCODE] %errorlevel% >> "%LOG%"
echo [DONE] %date% %time% >> "%LOG%"

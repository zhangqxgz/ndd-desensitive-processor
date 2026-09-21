@echo off
chcp 65001 >nul
setlocal
set HERE=%~dp0
set NAME=desensitive_processor-v1.0
set DIST=%HERE%dist\%NAME%

if not exist "%HERE%build\out\desensitive_processor.dll" (
  echo [ERROR] 请先运行 build.cmd 编译插件
  exit /b 1
)
if exist "%HERE%dist" rmdir /s /q "%HERE%dist"
mkdir "%DIST%"
copy /y "%HERE%build\out\desensitive_processor.dll" "%DIST%\" >nul
copy /y "%HERE%dist-template\*" "%DIST%\" >nul
powershell -NoProfile -Command "Compress-Archive -Path '%DIST%\*' -DestinationPath '%HERE%dist\%NAME%.zip' -Force"
if errorlevel 1 (
  echo [ERROR] 打包失败
  exit /b 1
)
echo [OK] 发布包: %HERE%dist\%NAME%.zip

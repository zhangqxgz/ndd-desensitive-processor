@echo off
chcp 65001 >nul
setlocal
set SRC=%~dp0build\out\desensitive_processor.dll
set DST=C:\Program Files\Notepad--\plugin
if not exist "%SRC%" (
  echo [ERROR] 未找到 %SRC%，请先运行 build.cmd 编译。
  pause
  exit /b 1
)
if not exist "%DST%" (
  echo [ERROR] 未找到 NDD 插件目录：%DST%
  pause
  exit /b 1
)
powershell -NoProfile -Command "Start-Process -Verb RunAs -Wait -FilePath cmd.exe -ArgumentList '/c copy /y \"%SRC%\" \"%DST%\\\"'"
if errorlevel 1 (
  echo [ERROR] 安装失败（可能取消了 UAC 授权）。
  pause
  exit /b 1
)
echo [OK] 已安装到 %DST%
echo 请重启 Notepad-- 后在“插件”菜单中使用“脱敏处理器”。
pause

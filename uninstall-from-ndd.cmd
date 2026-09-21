@echo off
chcp 65001 >nul
setlocal
set DST=C:\Program Files\Notepad--\plugin\desensitive_processor.dll
powershell -NoProfile -Command "Start-Process -Verb RunAs -Wait -FilePath cmd.exe -ArgumentList '/c del /f /q \"%DST%\"'"
echo [OK] 已请求卸载（如已删除，重启 Notepad-- 生效）
pause

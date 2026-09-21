param(
    [string]$TargetDir = "",
    [switch]$NoPause
)

$ErrorActionPreference = 'Stop'
$DllName = 'desensitive_processor.dll'
$SrcDll = Join-Path $PSScriptRoot $DllName

function Exit-WithPause([int]$Code) {
    if (-not $NoPause) {
        Write-Host ""
        Read-Host "按回车键退出"
    }
    exit $Code
}

if (-not (Test-Path -LiteralPath $SrcDll)) {
    Write-Host "错误：安装包内缺少 $DllName"
    Exit-WithPause 1
}

function Find-NddDir {
    $candidates = @()

    $keys = @(
        'HKLM:\SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\*',
        'HKLM:\SOFTWARE\WOW6432Node\Microsoft\Windows\CurrentVersion\Uninstall\*',
        'HKCU:\SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\*'
    )
    foreach ($key in $keys) {
        Get-ItemProperty $key -ErrorAction SilentlyContinue |
            Where-Object { $_.DisplayName -match 'notepad--|ndd' } |
            ForEach-Object {
                if ($_.InstallLocation) { $candidates += $_.InstallLocation }
            }
    }

    $candidates += 'C:\Program Files\Notepad--'
    $candidates += 'C:\Program Files (x86)\Notepad--'

    foreach ($dir in $candidates) {
        if ($dir -and (Test-Path -LiteralPath (Join-Path $dir 'Notepad--.exe'))) {
            return (Resolve-Path -LiteralPath $dir).Path
        }
    }
    return $null
}

$nddDir = $TargetDir
if (-not $nddDir) {
    Write-Host "正在查找已安装的 Notepad-- ..."
    $nddDir = Find-NddDir
}

if (-not $nddDir) {
    Write-Host "未自动找到 Notepad--，请手动选择它的主程序 Notepad--.exe"
    Add-Type -AssemblyName System.Windows.Forms
    $dialog = New-Object System.Windows.Forms.OpenFileDialog
    $dialog.Title = '请选择 Notepad--.exe（在 NDD 安装目录内）'
    $dialog.Filter = 'Notepad-- (Notepad--.exe)|Notepad--.exe'
    if ($dialog.ShowDialog() -ne [System.Windows.Forms.DialogResult]::OK) {
        Write-Host "已取消安装"
        Exit-WithPause 1
    }
    $nddDir = Split-Path -Parent $dialog.FileName
}

$nddDir = (Resolve-Path -LiteralPath $nddDir).Path
Write-Host "NDD 目录：$nddDir"

if (Test-Path -LiteralPath (Join-Path $nddDir 'Qt6Core.dll')) {
    Write-Host "错误：该 NDD 是 Qt6 版本，本插件为 Qt5 版本，无法通用。"
    Write-Host "需要在 Qt6 环境下重新编译插件。"
    Exit-WithPause 1
}

$pluginDir = Join-Path $nddDir 'plugin'
if (-not (Test-Path -LiteralPath $pluginDir)) {
    New-Item -ItemType Directory -Path $pluginDir | Out-Null
}

$dstDll = Join-Path $pluginDir $DllName
$needElevate = $false
try {
    Copy-Item -LiteralPath $SrcDll -Destination $dstDll -Force
} catch {
    $needElevate = $true
}

if ($needElevate) {
    Write-Host "需要管理员权限，请在弹出的 UAC 窗口点“是”..."
    $copyCmd = 'copy /y "' + $SrcDll + '" "' + $dstDll + '"'
    $proc = Start-Process -Verb RunAs -Wait -PassThru -FilePath cmd.exe -ArgumentList '/c', $copyCmd
    if ($proc.ExitCode -ne 0) {
        Write-Host "错误：复制失败，请尝试右键以管理员身份运行本安装脚本。"
        Exit-WithPause 1
    }
}

Write-Host ""
Write-Host "安装成功：$dstDll"
Write-Host "请重启 Notepad--，然后在“插件”菜单中点击“脱敏处理器”。"
Write-Host "映射配置保存在：%APPDATA%\ndd-desensitive\mappings.json"
Write-Host "如需跨电脑迁移映射，可在插件窗口里用“导出/导入”。"
Exit-WithPause 0

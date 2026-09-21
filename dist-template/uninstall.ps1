param(
    [string]$TargetDir = "",
    [switch]$NoPause
)

$ErrorActionPreference = 'Stop'
$DllName = 'desensitive_processor.dll'

function Exit-WithPause([int]$Code) {
    if (-not $NoPause) {
        Write-Host ""
        Read-Host "按回车键退出"
    }
    exit $Code
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
        if ($dir -and (Test-Path -LiteralPath (Join-Path $dir $DllName))) {
            return (Resolve-Path -LiteralPath $dir).Path
        }
    }
    return $null
}

$nddDir = $TargetDir
if (-not $nddDir) {
    $nddDir = Find-NddDir
}
if (-not $nddDir) {
    Add-Type -AssemblyName System.Windows.Forms
    $dialog = New-Object System.Windows.Forms.OpenFileDialog
    $dialog.Title = '请选择 Notepad--.exe（在 NDD 安装目录内）'
    $dialog.Filter = 'Notepad-- (Notepad--.exe)|Notepad--.exe'
    if ($dialog.ShowDialog() -ne [System.Windows.Forms.DialogResult]::OK) {
        Exit-WithPause 1
    }
    $nddDir = Split-Path -Parent $dialog.FileName
}

$dstDll = Join-Path (Join-Path $nddDir 'plugin') $DllName
if (-not (Test-Path -LiteralPath $dstDll)) {
    Write-Host "未找到插件：$dstDll（可能已经卸载）"
    Exit-WithPause 0
}

try {
    Remove-Item -LiteralPath $dstDll -Force
} catch {
    Write-Host "需要管理员权限，请在弹出的 UAC 窗口点“是”..."
    $delCmd = 'del /f /q "' + $dstDll + '"'
    Start-Process -Verb RunAs -Wait -FilePath cmd.exe -ArgumentList '/c', $delCmd | Out-Null
}

if (Test-Path -LiteralPath $dstDll) {
    Write-Host "卸载失败，请右键以管理员身份运行本脚本重试。"
    Exit-WithPause 1
}
Write-Host "已卸载。请重启 Notepad--。"
Write-Host "映射配置仍保留在：%APPDATA%\ndd-desensitive\mappings.json（如需删除可手动删该目录）"
Exit-WithPause 0

@echo off
setlocal enabledelayedexpansion

echo Installing GameBooster to %ProgramFiles%\GameBooster
set "dest=%ProgramFiles%\GameBooster"
if not exist "%dest%" mkdir "%dest%"

echo Copying binaries to %dest% ...
xcopy /y /i "bin\*" "%dest%\" >nul

echo Creating desktop shortcut...
powershell -NoProfile -ExecutionPolicy Bypass -Command "Try {$s=(New-Object -COM WScript.Shell).CreateShortcut([Environment]::GetFolderPath('Desktop') + '\\GameBooster.lnk'); $s.TargetPath = Join-Path $env:ProgramFiles 'GameBooster\\ConfigUI.exe'; $s.WorkingDirectory = Join-Path $env:ProgramFiles 'GameBooster'; $s.Save(); Write-Output 'OK'} Catch {Write-Error $_; exit 1}"

echo Creating scheduled task to run MonitorService at system startup (runs as SYSTEM)...
schtasks /Create /SC ONSTART /TN "GameBoosterMonitor" /TR "\"%ProgramFiles%\\GameBooster\\MonitorService.exe\"" /RL HIGHEST /F /RU SYSTEM >nul 2>&1
if %errorlevel% neq 0 (
	echo [WARN] Konnte geplanten Task nicht anlegen. Versuche Autostart im HKCU Run-Key.
	reg add "HKCU\Software\Microsoft\Windows\CurrentVersion\Run" /v "GameBooster" /d "\"%ProgramFiles%\\GameBooster\\MonitorService.exe\"" /f >nul 2>&1
	if %errorlevel% neq 0 echo [ERROR] Konnte weder Task noch Run-Key anlegen. Starte als Administrator und versuche erneut.
)

echo Installation complete.
pause

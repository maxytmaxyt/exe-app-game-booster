@echo off
setlocal

REM 1. Sicherstellen, dass wir im richtigen Ordner sind
if not exist "MonitorService" (
    echo [FEHLER] MonitorService Ordner nicht gefunden.
    echo Bitte sicherstellen, dass du im Projekt-Root bist.
    pause
    exit /b
)

REM 2. Bereinigen und neu erstellen (erzwungen)
if not exist bin mkdir bin
if exist bin\MonitorService.exe del /f /q bin\MonitorService.exe
if exist bin\ConfigUI.exe del /f /q bin\ConfigUI.exe

REM 3. Test ob wir in den bin Ordner schreiben können
echo test > bin\write_test.txt
if errorlevel 1 (
    echo [FEHLER] Kann nicht in den bin Ordner schreiben. 
    echo Ist eine Datei in 'bin' noch offen oder blockiert?
    pause
    exit /b
)
del bin\write_test.txt

REM 4. Kompilierung erzwingen
echo Kompiliere MonitorService...
g++ "MonitorService\src\main.cpp" -o "bin\MonitorService.exe" -static -luser32 -lkernel32 -O3
if %errorlevel% neq 0 goto error

echo Kompiliere ConfigUI...
g++ "ConfigUI\src\main.cpp" -o "bin\ConfigUI.exe" -static -luser32 -lgdi32 -O3
if %errorlevel% neq 0 goto error

echo.
echo ==========================================
echo ERFOLG: Dateien liegen im Ordner 'bin'
echo ==========================================
set /p RUNINSTALL=Starte Installation jetzt? (Y/N) 
if /I "%RUNINSTALL%"=="Y" (
    call install.bat
)

REM Optional: Build Inno Setup installer if ISCC is available
set "ISCCPATH="
where ISCC.exe >nul 2>&1
if %errorlevel%==0 set "ISCCPATH=ISCC.exe"
if exist "%ProgramFiles(x86)%\Inno Setup 6\ISCC.exe" if not defined ISCCPATH set "ISCCPATH=%ProgramFiles(x86)%\Inno Setup 6\ISCC.exe"
if defined ISCCPATH (
    set /p BUILD_ISS=Inno Setup Compiler gefunden. Installer bauen? (Y/N) 
    if /I "%BUILD_ISS%"=="Y" (
        echo Baue Installer mit %ISCCPATH%...
        if not exist dist mkdir dist
        %ISCCPATH% /O"%CD%\dist" "GameBoosterInstaller.iss"
        if %errorlevel% neq 0 echo [FEHLER] Installer-Build fehlgeschlagen.
    )
) else (
    REM Fallback: use IExpress (built-in on Windows) to create a self-extracting EXE so no Inno Setup required
    if not exist dist mkdir dist
    set "TARGET_EXE=%CD%\dist\GameBoosterInstaller.exe"
    set "SED=%TEMP%\gamebooster.sed"
    echo Erstelle IExpress SED: %SED%
    > "%SED%" echo [Version]
    >> "%SED%" echo Class=IEXPRESS
    >> "%SED%" echo SEDVersion=3
    >> "%SED%" echo.
    >> "%SED%" echo [Options]
    >> "%SED%" echo PackagePurpose=InstallApp
    >> "%SED%" echo ShowInstallProgramWindow=1
    >> "%SED%" echo HideExtractAnimation=0
    >> "%SED%" echo UseLongFileName=1
    >> "%SED%" echo PromptUser=0
    >> "%SED%" echo AdminQuietInst=0
    >> "%SED%" echo OverwritePrompt=YES
    >> "%SED%" echo RebootMode=NoRestart
    >> "%SED%" echo TargetName=%TARGET_EXE%
    >> "%SED%" echo InstallCommand=install.bat
    >> "%SED%" echo.
    >> "%SED%" echo [SourceFiles]
    >> "%SED%" echo SourceFiles0=bin
    >> "%SED%" echo.
    >> "%SED%" echo [SourceFiles0]
    >> "%SED%" echo MonitorService.exe=.
    >> "%SED%" echo ConfigUI.exe=.
    >> "%SED%" echo install.bat=.

    set "IEXPRESS=%windir%\system32\iexpress.exe"
    if exist "%IEXPRESS%" (
        echo Baue Selbstentpackendes EXE mit IExpress...
        "%IEXPRESS%" /N "%SED%"
        if exist "%TARGET_EXE%" (
            echo Installer erstellt: %TARGET_EXE%
        ) else (
            echo [FEHLER] IExpress hat das Paket nicht erstellt.
            echo Bitte Inno Setup installieren oder IExpress prüfen.
        )
    ) else (
        echo Weder ISCC noch IExpress gefunden. Zum Erstellen des Installers Inno Setup installieren oder IExpress nutzen.
    )
)
pause
exit

:error
echo [FEHLER] Kompilierung fehlgeschlagen.
pause
@echo off
setlocal
cd /d "%~dp0\.."
set "SRC=%CD%\build\PHASER_artefacts\Release\VST3\PHASER.vst3"
set "DEST=%CommonProgramFiles%\VST3\PHASER.vst3"
if not exist "%SRC%" (
  echo No encuentro el plugin compilado.
  echo Ejecuta BUILD.bat primero y revisa la carpeta build.
  pause
  exit /b 1
)
if not exist "%CommonProgramFiles%\VST3" mkdir "%CommonProgramFiles%\VST3"
if exist "%DEST%" rmdir /s /q "%DEST%"
xcopy "%SRC%" "%DEST%" /E /I /Y
if errorlevel 1 (echo Error copiando el plugin.& pause & exit /b 1)
echo PHASER instalado en: %DEST%
echo Ahora escanea plugins VST3 desde FL Studio.
pause

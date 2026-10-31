@echo off
cd /d "%~dp0\.."
if exist build rmdir /s /q build
echo Archivos de compilacion eliminados. Codigo fuente conservado.
pause

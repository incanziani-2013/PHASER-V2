@echo off
setlocal
cd /d "%~dp0\.."
echo ==========================================
echo              PHASER V1 PRO
echo             BUILD - RELEASE
echo ==========================================
where cmake >nul 2>nul || (echo ERROR: CMake no esta instalado.& pause & exit /b 1)
where git >nul 2>nul || (echo ERROR: Git no esta instalado.& pause & exit /b 1)
if not exist build mkdir build
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
if errorlevel 1 (echo ERROR configurando CMake.& pause & exit /b 1)
cmake --build build --config Release --parallel
if errorlevel 1 (echo ERROR compilando PHASER.& pause & exit /b 1)
echo.
echo BUILD COMPLETADO. Busca PHASER.vst3 dentro de build.
pause

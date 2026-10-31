# PHASER V1 PRO

Plugin VST3/Standalone para Windows, desarrollado con C++ y JUCE.

## Qué incluye esta primera base funcional
- Cuatro módulos seleccionables: COLOR, SPECTRUM, MOTION y GLITCH.
- Saturación waveshaping (tanh), filtros, chorus, width estéreo y bit/sample reduction.
- Controles automatizables mediante AudioProcessorValueTreeState.
- Guardado/restauración del estado del plugin.
- Interfaz oscura personalizada con esfera central animada.
- Mix global y bypass.

## Requisitos
- Windows 10/11 de 64 bits.
- Git.
- CMake 3.22 o superior.
- Visual Studio 2022 con Desktop development with C++.
- Conexión a Internet en la primera configuración para descargar JUCE 8.0.6.

## Compilar
1. Extraé el ZIP.
2. Abrí `Scripts\BUILD.bat`.
3. Esperá a que CMake descargue JUCE y compile.
4. El VST3 quedará dentro de `build\PHASER_artefacts\Release\VST3\PHASER.vst3` (la ruta exacta puede variar según CMake).

También podés abrir una terminal en la carpeta del proyecto:
```bat
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

## Instalar
Ejecutá `Scripts\INSTALL.bat` como usuario normal. Copia el VST3 a:
`%CommonProgramFiles%\VST3`

Después, en FL Studio, abrí Plugin Manager y escaneá la carpeta VST3.

## Importante
Esta es una primera versión funcional de desarrollo, no un producto comercial terminado. Los controles implementados procesan audio, pero algunos parámetros y comportamientos avanzados aún requieren ampliación y pruebas auditivas. El visualizador central es una animación gráfica y todavía no representa un análisis FFT real. Probá primero con volumen bajo y compará niveles al activar/desactivar efectos.

@echo off
REM Script de compilación para Windows

echo ===============================================
echo   Sistema de Gestión de Tareas - Compilación
echo ===============================================
echo.

REM Verificar si existe Qt
if not exist "C:\Qt" (
    echo [ERROR] Qt no encontrado en C:\Qt
    echo Por favor instala Qt6 desde: https://www.qt.io/download
    pause
    exit /b 1
)

REM Configurar variables de Qt (ajusta la versión según tu instalación)
set QT_PATH=C:\Qt\6.5.3\mingw_64
set CMAKE_PREFIX_PATH=%QT_PATH%

echo [INFO] Usando Qt desde: %QT_PATH%
echo.

REM Crear directorio build si no existe
if not exist "build" (
    echo [INFO] Creando directorio build...
    mkdir build
)

cd build

echo [INFO] Configurando proyecto con CMake...
cmake .. -G "MinGW Makefiles" -DCMAKE_PREFIX_PATH="%QT_PATH%"

if %errorlevel% neq 0 (
    echo [ERROR] Configuración de CMake falló
    cd ..
    pause
    exit /b 1
)

echo.
echo [INFO] Compilando proyecto...
cmake --build .

if %errorlevel% neq 0 (
    echo [ERROR] Compilación falló
    cd ..
    pause
    exit /b 1
)

cd ..

echo.
echo ===============================================
echo   Compilación exitosa!
echo ===============================================
echo.
echo El ejecutable está en: build\TaskManagementSystem.exe
echo.
echo Para ejecutar:
echo   cd build
echo   TaskManagementSystem.exe
echo.
pause


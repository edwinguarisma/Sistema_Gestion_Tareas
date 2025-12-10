@echo off
REM Script para limpiar y recompilar desde cero

echo ===============================================
echo   Limpieza y Recompilación Completa
echo ===============================================
echo.

REM Eliminar directorio build si existe
if exist "build" (
    echo [INFO] Eliminando directorio build anterior...
    rmdir /s /q build
)

echo [INFO] Directorio build limpio
echo.

REM Llamar al script de compilación normal
call build.bat


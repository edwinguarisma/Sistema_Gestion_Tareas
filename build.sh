#!/bin/bash

# Script de compilación para Linux/macOS

echo "==============================================="
echo "  Sistema de Gestión de Tareas - Compilación"
echo "==============================================="
echo ""

# Colores para output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Verificar si CMake está instalado
if ! command -v cmake &> /dev/null; then
    echo -e "${RED}[ERROR]${NC} CMake no está instalado"
    echo "Instala CMake con:"
    echo "  Ubuntu/Debian: sudo apt-get install cmake"
    echo "  Fedora/RHEL:   sudo dnf install cmake"
    echo "  macOS:         brew install cmake"
    exit 1
fi

# Verificar si Qt6 está instalado
if ! command -v qmake6 &> /dev/null && ! command -v qmake &> /dev/null; then
    echo -e "${YELLOW}[ADVERTENCIA]${NC} Qt6 no encontrado en PATH"
    echo "Instala Qt6 con:"
    echo "  Ubuntu/Debian: sudo apt-get install qt6-base-dev"
    echo "  Fedora/RHEL:   sudo dnf install qt6-qtbase-devel"
    echo "  macOS:         brew install qt@6"
    echo ""
    echo "O establece CMAKE_PREFIX_PATH manualmente"
fi

# Crear directorio build
echo -e "${GREEN}[INFO]${NC} Creando directorio build..."
mkdir -p build
cd build

# Configurar con CMake
echo ""
echo -e "${GREEN}[INFO]${NC} Configurando proyecto con CMake..."
cmake ..

if [ $? -ne 0 ]; then
    echo -e "${RED}[ERROR]${NC} Configuración de CMake falló"
    exit 1
fi

# Compilar
echo ""
echo -e "${GREEN}[INFO]${NC} Compilando proyecto..."
make -j$(nproc 2>/dev/null || sysctl -n hw.ncpu)

if [ $? -ne 0 ]; then
    echo -e "${RED}[ERROR]${NC} Compilación falló"
    exit 1
fi

cd ..

echo ""
echo "==============================================="
echo -e "${GREEN}  Compilación exitosa!${NC}"
echo "==============================================="
echo ""
echo "El ejecutable está en: build/TaskManagementSystem"
echo ""
echo "Para ejecutar:"
echo "  cd build"
echo "  ./TaskManagementSystem"
echo ""


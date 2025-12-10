# Sistema de Gestión de Tareas y Proyectos Colaborativos

## Descripción

Aplicación de escritorio desarrollada en C++ que implementa un sistema visual e intuitivo para la gestión de proyectos basado en el método Kanban, similar a Trello o Jira.

## Características Principales

### Funcionalidades Básicas
- ✅ Creación y gestión de proyectos
- ✅ Tableros Kanban con estados personalizables (Pendiente, En Progreso, Terminado)
- ✅ Asignación de tareas a usuarios
- ✅ Fechas de vencimiento y notificaciones
- ✅ Interfaz gráfica intuitiva optimizada para monitores táctiles

### Funcionalidades Avanzadas
- ✅ **Subtareas anidadas**: Permite crear jerarquías de tareas complejas
- ✅ **Sistema de dependencias**: Las tareas pueden depender de otras tareas
- ✅ **Registro de actividad completo**: Historial detallado de todos los cambios
- ✅ **Control de versiones**: Patrón Memento para ver versiones anteriores de tareas
- ✅ **Drag & Drop**: Mueve tareas entre estados de forma visual

## Conceptos Técnicos Implementados

### Patrones de Diseño
- **Memento Pattern**: Control de versiones de tareas
- **Observer Pattern**: Sistema de notificaciones
- **Singleton Pattern**: Gestión centralizada de proyectos

### Estructuras de Datos
- Listas enlazadas para gestión de tareas
- Árboles para subtareas anidadas
- Grafos para dependencias entre tareas
- Mapas hash para búsquedas eficientes

### Programación Avanzada
- Templates y STL (Standard Template Library)
- Smart pointers (shared_ptr, unique_ptr)
- Manejo de excepciones
- Programación orientada a objetos avanzada

## Requisitos del Sistema

### Software Necesario
- **CMake** 3.16 o superior
- **Compilador C++17** compatible (MSVC, GCC, Clang)
- **Qt6** (Core, Widgets, Gui)

### Instalación de Qt6 en Windows
1. Descargar Qt Online Installer desde: https://www.qt.io/download-qt-installer
2. Instalar Qt 6.5 o superior con los componentes:
   - Desktop development with C++
   - CMake
3. Agregar Qt al PATH o configurar CMAKE_PREFIX_PATH

## Compilación

⚠️ **IMPORTANTE**: Si encuentras errores de compilación, lee `CORRECCIONES_COMPILACION.md`

### Método Rápido (Recomendado)

**Windows**:
```bash
build.bat
```

**Linux/macOS**:
```bash
chmod +x build.sh
./build.sh
```

### Recompilación Limpia

Si tienes problemas, limpia y recompila desde cero:
```bash
rebuild.bat                    # Windows
rm -rf build && ./build.sh     # Linux/macOS
```

### Compilación Manual

**Windows con MinGW**:
```bash
mkdir build
cd build
cmake .. -G "MinGW Makefiles" -DCMAKE_PREFIX_PATH="C:/Qt/6.5.3/mingw_64"
cmake --build .
```

**Windows con Visual Studio**:
```bash
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022" -DCMAKE_PREFIX_PATH="C:/Qt/6.5.3/msvc2019_64"
cmake --build . --config Release
```

## Ejecución

Después de compilar, ejecutar:
```bash
./build/TaskManagementSystem.exe  # Windows
```

## Estructura del Proyecto

```
TaskManagementSystem/
├── CMakeLists.txt
├── README.md
├── include/
│   ├── models/           # Modelos de datos
│   ├── managers/         # Gestores de lógica de negocio
│   ├── ui/              # Interfaz gráfica
│   └── utils/           # Utilidades
├── src/
│   ├── main.cpp
│   ├── models/
│   ├── managers/
│   ├── ui/
│   └── utils/
└── data/                # Archivos de persistencia

```

## Uso de la Aplicación

### Crear un Proyecto
1. Menú **Archivo** → **Nuevo Proyecto**
2. Ingresar nombre y descripción
3. El proyecto se crea con un tablero predeterminado

### Gestionar Tareas
1. Click en **"+ Nueva Tarea"** en cualquier columna
2. Completar información (título, descripción, asignado, fecha límite)
3. Agregar subtareas si es necesario
4. Establecer dependencias con otras tareas

### Mover Tareas
- **Drag & Drop**: Arrastrar tarjetas entre columnas
- **Doble click**: Editar detalles de la tarea

### Ver Historial
- Click derecho en una tarea → **Ver Historial**
- Navegar entre versiones anteriores
- Restaurar versión anterior si es necesario

## Características del Código

### Validaciones Implementadas
- ✅ No se pueden crear dependencias circulares
- ✅ Las tareas no pueden empezar si sus dependencias no están terminadas
- ✅ Las fechas de vencimiento deben ser futuras
- ✅ Los usuarios deben estar registrados para ser asignados

### Sistema de Notificaciones
- Notificaciones de tareas próximas a vencer (24h antes)
- Alertas de dependencias resueltas
- Avisos de cambios en tareas asignadas

### Persistencia de Datos
- Guardado automático cada 5 minutos
- Formato TXT para fácil lectura
- Backup automático antes de sobrescribir

## Autor

Proyecto desarrollado como trabajo final para Programación III - 4to Semestre

## Licencia

Este proyecto es de código abierto para fines educativos.


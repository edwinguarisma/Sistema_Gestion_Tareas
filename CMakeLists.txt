cmake_minimum_required(VERSION 3.16)
project(TaskManagementSystem VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Buscar Qt6
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Gui)

# Incluir directorios
include_directories(${CMAKE_SOURCE_DIR}/include)

# Archivos de código fuente
set(SOURCES
    src/main.cpp
    src/models/User.cpp
    src/models/Task.cpp
    src/models/Subtask.cpp
    src/models/Board.cpp
    src/models/Project.cpp
    src/models/TaskMemento.cpp
    src/models/ActivityLog.cpp
    src/managers/ProjectManager.cpp
    src/managers/NotificationManager.cpp
    src/ui/MainWindow.cpp
    src/ui/BoardWidget.cpp
    src/ui/ColumnWidget.cpp
    src/ui/TaskCard.cpp
    src/ui/TaskDialog.cpp
    src/ui/ProjectDialog.cpp
    src/utils/DateUtils.cpp
    src/utils/DataPersistence.cpp
)

# Archivos de encabezado
set(HEADERS
    include/models/User.h
    include/models/Task.h
    include/models/Subtask.h
    include/models/Board.h
    include/models/Project.h
    include/models/TaskMemento.h
    include/models/ActivityLog.h
    include/managers/ProjectManager.h
    include/managers/NotificationManager.h
    include/ui/MainWindow.h
    include/ui/BoardWidget.h
    include/ui/ColumnWidget.h
    include/ui/TaskCard.h
    include/ui/TaskDialog.h
    include/ui/ProjectDialog.h
    include/utils/DateUtils.h
    include/utils/DataPersistence.h
)

# Crear ejecutable
if(WIN32 AND MINGW)
    # Para MinGW, no usar WIN32 subsystem para evitar conflicto con Qt6EntryPoint
    add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})
else()
    add_executable(${PROJECT_NAME} WIN32 ${SOURCES} ${HEADERS})
endif()

# Enlazar con Qt
target_link_libraries(${PROJECT_NAME}
    Qt6::Core
    Qt6::Widgets
    Qt6::Gui
)


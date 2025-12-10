#include "ui/MainWindow.h"
#include "ui/ProjectDialog.h"
#include "ui/TaskDialog.h"
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QMessageBox>
#include <QCloseEvent>
#include <QFileDialog>
#include <QInputDialog>

using namespace std;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), currentUserId(1), currentUserName("Usuario Principal") {
    
    // Inicializar managers
    projectManager = ProjectManager::getInstance();
    notificationManager = make_shared<NotificationManager>();
    dataPersistence = make_shared<DataPersistence>("data");
    
    setupUI();
    createMenus();
    createToolBar();
    setupConnections();
    
    // Configurar timer de autoguardado (cada 5 minutos)
    autoSaveTimer = new QTimer(this);
    connect(autoSaveTimer, &QTimer::timeout, this, &MainWindow::onAutoSave);
    autoSaveTimer->start(300000);  // 5 minutos en milisegundos
    
    updateWindowTitle();
    
    // Intentar cargar proyecto guardado
    auto projectIds = dataPersistence->getAvailableProjectIds();
    shared_ptr<Project> project = nullptr;
    
    if (!projectIds.empty()) {
        // Cargar el primer proyecto encontrado
        project = dataPersistence->loadProject(projectIds[0]);
        if (project) {
            projectManager->addProject(project);
            statusLabel->setText("✓ Proyecto cargado desde archivo");
        }
    }
    
    // Si no hay proyecto cargado, crear uno nuevo
    if (!project) {
        project = projectManager->createProject("Proyecto por Defecto", 
                                               "Mi primer proyecto");
        auto board = project->createBoard("Tablero Principal");
        
        // Crear algunos usuarios de ejemplo
        project->createUser("Admin", "admin@example.com", "admin");
        project->createUser("Developer 1", "dev1@example.com", "developer");
        
        // Guardar inmediatamente
        dataPersistence->saveProject(project);
    }
    
    loadProject(project);
}

MainWindow::~MainWindow() {}

void MainWindow::setupUI() {
    setWindowTitle("Sistema de Gestión de Tareas y Proyectos");
    setMinimumSize(1280, 850);
    
    // Establecer estilo global para la aplicación
    setStyleSheet(
        "QMainWindow {"
        "  background-color: #fafafa;"
        "}"
        "QMenuBar {"
        "  background-color: #ffffff;"
        "  color: #1a1a1a;"
        "  border-bottom: 1px solid #e5e5e5;"
        "  padding: 4px;"
        "}"
        "QMenuBar::item {"
        "  background-color: transparent;"
        "  color: #1a1a1a;"
        "  padding: 8px 12px;"
        "  border-radius: 4px;"
        "}"
        "QMenuBar::item:selected {"
        "  background-color: #e5e5e5;"
        "}"
        "QMenu {"
        "  background-color: #ffffff;"
        "  color: #1a1a1a;"
        "  border: 1px solid #d0d0d0;"
        "  border-radius: 4px;"
        "}"
        "QMenu::item {"
        "  padding: 8px 30px 8px 20px;"
        "  background-color: transparent;"
        "}"
        "QMenu::item:selected {"
        "  background-color: #e5e5e5;"
        "}"
    );
    
    // Widget central con pestañas minimalistas
    tabWidget = new QTabWidget(this);
    tabWidget->setTabsClosable(false);
    tabWidget->setMovable(true);
    tabWidget->setStyleSheet(
        "QTabWidget::pane {"
        "  border: none;"
        "  background: #ffffff;"
        "}"
        "QTabBar::tab {"
        "  background: #f7f8fa;"
        "  color: #4a4a4a;"
        "  padding: 12px 20px;"
        "  margin-right: 4px;"
        "  border: none;"
        "  border-top-left-radius: 6px;"
        "  border-top-right-radius: 6px;"
        "  font-size: 11pt;"
        "}"
        "QTabBar::tab:selected {"
        "  background: #ffffff;"
        "  color: #1a1a1a;"
        "  font-weight: bold;"
        "}"
        "QTabBar::tab:hover {"
        "  background: #e5e5e5;"
        "  color: #1a1a1a;"
        "}"
    );
    
    setCentralWidget(tabWidget);
    
    // Barra de estado minimalista
    statusBar = new QStatusBar(this);
    statusBar->setStyleSheet(
        "QStatusBar {"
        "  background: #ffffff;"
        "  color: #4a4a4a;"
        "  padding: 8px;"
        "  border-top: 1px solid #e5e5e5;"
        "}"
    );
    setStatusBar(statusBar);
    
    statusLabel = new QLabel("Listo");
    statusLabel->setStyleSheet("color: #4a4a4a; padding: 4px; font-size: 10pt;");
    statusBar->addWidget(statusLabel);
    
    notificationLabel = new QLabel("🔔 0");
    notificationLabel->setStyleSheet(
        "background-color: #E53935; "
        "color: white; "
        "padding: 4px 10px; "
        "border-radius: 12px; "
        "font-size: 9pt; "
        "font-weight: bold;"
    );
    statusBar->addPermanentWidget(notificationLabel);
}

void MainWindow::createMenus() {
    QMenuBar* menuBar = new QMenuBar(this);
    setMenuBar(menuBar);
    
    // Menú Archivo
    QMenu* fileMenu = menuBar->addMenu("&Archivo");
    
    QAction* newProjectAction = fileMenu->addAction("&Nuevo Proyecto");
    newProjectAction->setShortcut(QKeySequence::New);
    connect(newProjectAction, &QAction::triggered, this, &MainWindow::onNewProject);
    
    QAction* openProjectAction = fileMenu->addAction("&Abrir Proyecto");
    openProjectAction->setShortcut(QKeySequence::Open);
    connect(openProjectAction, &QAction::triggered, this, &MainWindow::onOpenProject);
    
    QAction* saveProjectAction = fileMenu->addAction("&Guardar Proyecto");
    saveProjectAction->setShortcut(QKeySequence::Save);
    connect(saveProjectAction, &QAction::triggered, this, &MainWindow::onSaveProject);
    
    fileMenu->addSeparator();
    
    QAction* closeProjectAction = fileMenu->addAction("&Cerrar Proyecto");
    connect(closeProjectAction, &QAction::triggered, this, &MainWindow::onCloseProject);
    
    fileMenu->addSeparator();
    
    QAction* exitAction = fileMenu->addAction("&Salir");
    exitAction->setShortcut(QKeySequence::Quit);
    connect(exitAction, &QAction::triggered, this, &MainWindow::onExit);
    
    // Menú Proyecto
    QMenu* projectMenu = menuBar->addMenu("&Proyecto");
    
    QAction* newBoardAction = projectMenu->addAction("Nuevo &Tablero");
    connect(newBoardAction, &QAction::triggered, this, &MainWindow::onNewBoard);
    
    QAction* newTaskAction = projectMenu->addAction("Nueva &Tarea");
    newTaskAction->setShortcut(Qt::CTRL | Qt::Key_T);
    connect(newTaskAction, &QAction::triggered, this, &MainWindow::onNewTask);
    
    projectMenu->addSeparator();
    
    QAction* searchAction = projectMenu->addAction("&Buscar Tareas");
    searchAction->setShortcut(QKeySequence::Find);
    connect(searchAction, &QAction::triggered, this, &MainWindow::onSearchTasks);
    
    // Menú Ver
    QMenu* viewMenu = menuBar->addMenu("&Ver");
    
    QAction* statsAction = viewMenu->addAction("&Estadísticas");
    connect(statsAction, &QAction::triggered, this, &MainWindow::onShowStatistics);
    
    QAction* notificationsAction = viewMenu->addAction("&Notificaciones");
    connect(notificationsAction, &QAction::triggered, this, &MainWindow::onShowNotifications);
    
    QAction* usersAction = viewMenu->addAction("&Usuarios");
    connect(usersAction, &QAction::triggered, this, &MainWindow::onShowUsers);
    
    // Menú Ayuda
    QMenu* helpMenu = menuBar->addMenu("A&yuda");
    
    QAction* aboutAction = helpMenu->addAction("&Acerca de");
    connect(aboutAction, &QAction::triggered, this, &MainWindow::onAbout);
}

void MainWindow::createToolBar() {
    QToolBar* toolBar = new QToolBar("Principal", this);
    toolBar->setMovable(false);
    toolBar->setIconSize(QSize(24, 24));
    toolBar->setStyleSheet(
        "QToolBar {"
        "  background: #ffffff;"
        "  border: none;"
        "  border-bottom: 1px solid #e5e5e5;"
        "  padding: 8px 16px;"
        "  spacing: 12px;"
        "}"
        "QToolButton {"
        "  background: #0078d4;"
        "  color: white;"
        "  border: none;"
        "  border-radius: 6px;"
        "  padding: 8px 16px;"
        "  font-size: 10pt;"
        "  font-weight: 500;"
        "}"
        "QToolButton:hover {"
        "  background: #106ebe;"
        "}"
        "QToolButton:pressed {"
        "  background: #005a9e;"
        "}"
    );
    addToolBar(toolBar);
    
    QAction* newProjectToolAction = toolBar->addAction("📁 Nuevo Proyecto");
    connect(newProjectToolAction, &QAction::triggered, this, &MainWindow::onNewProject);
    
    QAction* newBoardToolAction = toolBar->addAction("📋 Nuevo Tablero");
    connect(newBoardToolAction, &QAction::triggered, this, &MainWindow::onNewBoard);
    
    QAction* newTaskToolAction = toolBar->addAction("➕ Nueva Tarea");
    connect(newTaskToolAction, &QAction::triggered, this, &MainWindow::onNewTask);
    
    toolBar->addSeparator();
    
    QAction* saveToolAction = toolBar->addAction("💾 Guardar");
    connect(saveToolAction, &QAction::triggered, this, &MainWindow::onSaveProject);
    
    QAction* refreshToolAction = toolBar->addAction("🔄 Actualizar");
    connect(refreshToolAction, &QAction::triggered, this, &MainWindow::refreshCurrentView);
}

void MainWindow::setupConnections() {
    connect(tabWidget, &QTabWidget::currentChanged, this, &MainWindow::onTabChanged);
}

void MainWindow::loadCurrentProject() {
    auto project = projectManager->getCurrentProject();
    if (project) {
        loadProject(project);
    }
}

void MainWindow::updateWindowTitle() {
    auto project = projectManager->getCurrentProject();
    if (project) {
        setWindowTitle("Sistema de Gestión - " + 
                      QString::fromStdString(project->getName()));
    } else {
        setWindowTitle("Sistema de Gestión de Tareas y Proyectos");
    }
}

void MainWindow::updateNotificationBadge() {
    int unreadCount = notificationManager->getUnreadCount(currentUserId);
    notificationLabel->setText("🔔 " + QString::number(unreadCount));
}

void MainWindow::onNewProject() {
    ProjectDialog dialog(nullptr, this);
    
    if (dialog.exec() == QDialog::Accepted) {
        auto project = projectManager->createProject(
            dialog.getName().toStdString(),
            dialog.getDescription().toStdString()
        );
        
        // Crear un tablero por defecto
        project->createBoard("Tablero Principal");
        
        loadProject(project);
        statusLabel->setText("Proyecto creado exitosamente");
    }
}

void MainWindow::onOpenProject() {
    // Mostrar lista de proyectos disponibles
    auto projects = projectManager->getAllProjects();
    
    if (projects.empty()) {
        QMessageBox::information(this, "Información", 
                                "No hay proyectos disponibles");
        return;
    }
    
    QStringList projectNames;
    for (const auto& project : projects) {
        projectNames << QString::fromStdString(project->getName());
    }
    
    bool ok;
    QString selected = QInputDialog::getItem(this, "Abrir Proyecto",
                                            "Seleccionar proyecto:",
                                            projectNames, 0, false, &ok);
    
    if (ok && !selected.isEmpty()) {
        for (const auto& project : projects) {
            if (QString::fromStdString(project->getName()) == selected) {
                loadProject(project);
                break;
            }
        }
    }
}

void MainWindow::onSaveProject() {
    auto project = projectManager->getCurrentProject();
    
    if (project) {
        if (dataPersistence->saveProject(project)) {
            statusLabel->setText("Proyecto guardado");
            statusLabel->setStyleSheet("color: #61bd4f; padding: 4px; font-size: 9pt;");
            
            // Crear respaldo
            dataPersistence->createBackup(project->getId());
        } else {
            statusLabel->setText("Error al guardar");
            statusLabel->setStyleSheet("color: #eb5a46; padding: 4px; font-size: 9pt;");
            QMessageBox::warning(this, "Error", "No se pudo guardar el proyecto");
        }
    }
}

void MainWindow::onCloseProject() {
    // Guardar antes de cerrar
    onSaveProject();
    
    // Limpiar tabs
    tabWidget->clear();
    boardWidgets.clear();
    
    statusLabel->setText("Proyecto cerrado");
    updateWindowTitle();
}

void MainWindow::onExit() {
    close();
}

void MainWindow::onNewBoard() {
    auto project = projectManager->getCurrentProject();
    
    if (!project) {
        QMessageBox::warning(this, "Error", "No hay proyecto abierto");
        return;
    }
    
    bool ok;
    QString name = QInputDialog::getText(this, "Nuevo Tablero",
                                        "Nombre del tablero:",
                                        QLineEdit::Normal, "", &ok);
    
    if (ok && !name.isEmpty()) {
        auto board = project->createBoard(name.toStdString());
        
        // Crear widget y tab
        BoardWidget* boardWidget = new BoardWidget(board, currentUserName, this);
        tabWidget->addTab(boardWidget, name);
        boardWidgets[board->getId()] = boardWidget;
        
        // Conectar señales
        connect(boardWidget, &BoardWidget::taskSelected, 
                [this, board, project](int taskId) {
                    auto task = board->findTaskById(taskId);
                    if (task) {
                        TaskDialog dialog(board, project, task, this);
                        if (dialog.exec() == QDialog::Accepted) {
                            refreshCurrentView();
                        }
                    }
                });
        
        connect(boardWidget, &BoardWidget::newTaskRequested,
                [this, board, project](const string& state) {
                    TaskDialog dialog(board, project, nullptr, this);
                    if (dialog.exec() == QDialog::Accepted) {
                        auto task = dialog.getTask();
                        if (task) {
                            task->setState(state, currentUserName);
                            board->addTask(task, state);
                            refreshCurrentView();
                        }
                    }
                });
        
        statusLabel->setText("Tablero creado: " + name);
    }
}

void MainWindow::onNewTask() {
    auto project = projectManager->getCurrentProject();
    
    if (!project) {
        QMessageBox::warning(this, "Error", "No hay proyecto abierto");
        return;
    }
    
    // Obtener el board actual
    int currentIndex = tabWidget->currentIndex();
    if (currentIndex < 0) {
        QMessageBox::warning(this, "Error", "No hay tablero seleccionado");
        return;
    }
    
    BoardWidget* boardWidget = qobject_cast<BoardWidget*>(tabWidget->widget(currentIndex));
    if (boardWidget) {
        auto board = boardWidget->getBoard();
        TaskDialog dialog(board, project, nullptr, this);
        
        if (dialog.exec() == QDialog::Accepted) {
            auto task = dialog.getTask();
            if (task) {
                boardWidget->addTask(task);
                statusLabel->setText("Tarea creada exitosamente");
            }
        }
    }
}

void MainWindow::onSearchTasks() {
    auto project = projectManager->getCurrentProject();
    
    if (!project) {
        QMessageBox::warning(this, "Error", "No hay proyecto abierto");
        return;
    }
    
    bool ok;
    QString searchText = QInputDialog::getText(this, "Buscar Tareas",
                                              "Texto a buscar:",
                                              QLineEdit::Normal, "", &ok);
    
    if (ok && !searchText.isEmpty()) {
        auto tasks = project->findTasksByTitle(searchText.toStdString());
        
        QString result = "Tareas encontradas: " + QString::number(tasks.size()) + "\n\n";
        for (const auto& task : tasks) {
            result += QString::fromStdString(task->toString()) + "\n";
        }
        
        QMessageBox::information(this, "Resultados de Búsqueda", result);
    }
}

void MainWindow::onShowStatistics() {
    auto project = projectManager->getCurrentProject();
    
    if (!project) {
        QMessageBox::warning(this, "Error", "No hay proyecto abierto");
        return;
    }
    
    QString stats;
    stats += "=== Estadísticas del Proyecto ===\n\n";
    stats += "Nombre: " + QString::fromStdString(project->getName()) + "\n";
    stats += "Tableros: " + QString::number(project->getTotalBoardCount()) + "\n";
    stats += "Tareas Totales: " + QString::number(project->getTotalTaskCount()) + "\n";
    stats += "Usuarios: " + QString::number(project->getTotalUserCount()) + "\n";
    stats += "Completitud: " + QString::number(project->getOverallCompletionPercentage(), 'f', 1) + "%\n\n";
    
    stats += "=== Por Tablero ===\n";
    for (const auto& board : project->getBoards()) {
        stats += "\n" + QString::fromStdString(board->getName()) + ":\n";
        stats += "  Tareas: " + QString::number(board->getTotalTaskCount()) + "\n";
        stats += "  Completitud: " + QString::number(board->getCompletionPercentage(), 'f', 1) + "%\n";
        
        for (const auto& state : board->getStates()) {
            stats += "    " + QString::fromStdString(state) + ": " + 
                    QString::number(board->getTaskCountByState(state)) + "\n";
        }
    }
    
    QMessageBox::information(this, "Estadísticas", stats);
}

void MainWindow::onShowNotifications() {
    auto notifications = notificationManager->getNotificationsByUser(currentUserId);
    
    QString text = "=== Notificaciones ===\n\n";
    
    if (notifications.empty()) {
        text += "No hay notificaciones";
    } else {
        for (const auto& notif : notifications) {
            text += QString::fromStdString(notif.toString()) + "\n\n";
        }
    }
    
    QMessageBox::information(this, "Notificaciones", text);
    notificationManager->markAllAsRead(currentUserId);
    updateNotificationBadge();
}

void MainWindow::onShowUsers() {
    auto project = projectManager->getCurrentProject();
    
    if (!project) {
        QMessageBox::warning(this, "Error", "No hay proyecto abierto");
        return;
    }
    
    QString text = "=== Usuarios del Proyecto ===\n\n";
    
    for (const auto& user : project->getAllUsers()) {
        text += QString::fromStdString(user->toString()) + "\n";
        
        // Contar tareas asignadas
        auto tasks = project->findTasksByUser(user->getId());
        text += "  Tareas asignadas: " + QString::number(tasks.size()) + "\n\n";
    }
    
    QMessageBox::information(this, "Usuarios", text);
}

void MainWindow::onShowSettings() {
    QMessageBox::information(this, "Configuración", 
                            "Funcionalidad de configuración en desarrollo");
}

void MainWindow::onAbout() {
    QString about;
    about += "<h2>Sistema de Gestión de Tareas y Proyectos</h2>";
    about += "<p><b>Versión:</b> 1.0.0</p>";
    about += "<p><b>Desarrollado con:</b> C++ y Qt6</p>";
    about += "<h3>Características:</h3>";
    about += "<ul>";
    about += "<li>Gestión de proyectos y tableros Kanban</li>";
    about += "<li>Subtareas anidadas</li>";
    about += "<li>Sistema de dependencias</li>";
    about += "<li>Control de versiones (Patrón Memento)</li>";
    about += "<li>Registro de actividad completo</li>";
    about += "<li>Notificaciones de tareas</li>";
    about += "<li>Drag & Drop para mover tareas</li>";
    about += "</ul>";
    about += "<p>Proyecto Final - Programación III</p>";
    
    QMessageBox::about(this, "Acerca de", about);
}

void MainWindow::onAutoSave() {
    auto project = projectManager->getCurrentProject();
    if (project) {
        if (dataPersistence->saveProject(project)) {
            statusLabel->setText("Guardado automático");
            statusLabel->setStyleSheet("color: #5e6c84; padding: 4px; font-size: 9pt;");
        }
    }
}

void MainWindow::onTabChanged(int index) {
    if (index >= 0) {
        statusLabel->setText("Vista cambiada");
    }
}

void MainWindow::setCurrentUser(int userId, const string& userName) {
    currentUserId = userId;
    currentUserName = userName;
}

void MainWindow::loadProject(shared_ptr<Project> project) {
    if (!project) return;
    
    projectManager->setCurrentProject(project);
    
    // Limpiar tabs existentes
    tabWidget->clear();
    boardWidgets.clear();
    
    // Agregar tabs para cada tablero
    for (const auto& board : project->getBoards()) {
        BoardWidget* boardWidget = new BoardWidget(board, currentUserName, this);
        tabWidget->addTab(boardWidget, QString::fromStdString(board->getName()));
        boardWidgets[board->getId()] = boardWidget;
        
        // Conectar señales
        connect(boardWidget, &BoardWidget::taskSelected,
                [this, board, project](int taskId) {
                    auto task = board->findTaskById(taskId);
                    if (task) {
                        TaskDialog dialog(board, project, task, this);
                        if (dialog.exec() == QDialog::Accepted) {
                            refreshCurrentView();
                        }
                    }
                });
        
        connect(boardWidget, &BoardWidget::newTaskRequested,
                [this, board, project](const string& state) {
                    TaskDialog dialog(board, project, nullptr, this);
                    if (dialog.exec() == QDialog::Accepted) {
                        auto task = dialog.getTask();
                        if (task) {
                            task->setState(state, currentUserName);
                            board->addTask(task, state);
                            refreshCurrentView();
                        }
                    }
                });
    }
    
    updateWindowTitle();
    statusLabel->setText("Proyecto cargado: " + QString::fromStdString(project->getName()));
}

void MainWindow::refreshCurrentView() {
    int currentIndex = tabWidget->currentIndex();
    if (currentIndex >= 0) {
        BoardWidget* boardWidget = qobject_cast<BoardWidget*>(tabWidget->widget(currentIndex));
        if (boardWidget) {
            boardWidget->refresh();
        }
    }
}

void MainWindow::closeEvent(QCloseEvent *event) {
    // Guardar automáticamente antes de cerrar
    auto project = projectManager->getCurrentProject();
    
    if (project) {
        auto reply = QMessageBox::question(this, "Confirmar Salida",
                                           "¿Desea guardar los cambios antes de salir?",
                                           QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
        
        if (reply == QMessageBox::Yes) {
            if (dataPersistence->saveProject(project)) {
                statusLabel->setText("✓ Proyecto guardado");
            }
            event->accept();
        } else if (reply == QMessageBox::No) {
            event->accept();
        } else {
            event->ignore();
        }
    } else {
        event->accept();
    }
}


#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>
#include <QTabWidget>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QLabel>
#include <QTimer>
#include <memory>
#include <map>
#include "managers/ProjectManager.h"
#include "managers/NotificationManager.h"
#include "utils/DataPersistence.h"
#include "BoardWidget.h"

using namespace std;

/**
 * @brief Ventana principal de la aplicación
 */
class MainWindow : public QMainWindow {
    Q_OBJECT

private:
    // Managers
    shared_ptr<ProjectManager> projectManager;
    shared_ptr<NotificationManager> notificationManager;
    shared_ptr<DataPersistence> dataPersistence;
    
    // UI Components
    QTabWidget* tabWidget;
    QMenuBar* menuBar;
    QToolBar* toolBar;
    QStatusBar* statusBar;
    QLabel* statusLabel;
    QLabel* notificationLabel;
    
    // Timer para autoguardado
    QTimer* autoSaveTimer;
    
    // Widgets de tableros actuales
    map<int, BoardWidget*> boardWidgets;
    
    // Usuario actual (simulado)
    int currentUserId;
    string currentUserName;
    
    void setupUI();
    void createMenus();
    void createToolBar();
    void setupConnections();
    void loadCurrentProject();
    void updateWindowTitle();
    void updateNotificationBadge();

private slots:
    void onNewProject();
    void onOpenProject();
    void onSaveProject();
    void onCloseProject();
    void onExit();
    
    void onNewBoard();
    void onNewTask();
    void onSearchTasks();
    
    void onShowStatistics();
    void onShowNotifications();
    void onShowUsers();
    void onShowSettings();
    
    void onAbout();
    
    void onAutoSave();
    void onTabChanged(int index);

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    
    void setCurrentUser(int userId, const string& userName);
    void loadProject(shared_ptr<Project> project);
    void refreshCurrentView();
    
protected:
    void closeEvent(QCloseEvent *event) override;
};

#endif // MAIN_WINDOW_H


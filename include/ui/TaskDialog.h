#ifndef TASK_DIALOG_H
#define TASK_DIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QTextEdit>
#include <QComboBox>
#include <QDateTimeEdit>
#include <QSpinBox>
#include <QPushButton>
#include <QTabWidget>
#include <QListWidget>
#include <QTextBrowser>
#include <memory>
#include "models/Task.h"
#include "models/Board.h"
#include "models/Project.h"

using namespace std;

/**
 * @brief Diálogo para crear/editar tareas
 */
class TaskDialog : public QDialog {
    Q_OBJECT

private:
    shared_ptr<Task> task;
    shared_ptr<Board> board;
    shared_ptr<Project> project;
    bool isNewTask;
    
    // Pestaña principal
    QLineEdit* titleEdit;
    QTextEdit* descriptionEdit;
    QComboBox* stateCombo;
    QComboBox* userCombo;
    QDateTimeEdit* dueDateEdit;
    QSpinBox* prioritySpinBox;
    QLineEdit* tagsEdit;
    
    // Pestaña de subtareas
    QListWidget* subtasksList;
    QPushButton* addSubtaskButton;
    QPushButton* removeSubtaskButton;
    
    // Pestaña de dependencias
    QListWidget* dependenciesList;
    QComboBox* availableTasksCombo;
    QPushButton* addDependencyButton;
    QPushButton* removeDependencyButton;
    
    // Pestaña de historial
    QTextBrowser* historyBrowser;
    QListWidget* versionsList;
    QPushButton* restoreVersionButton;
    
    // Pestaña de registro de actividad
    QTextBrowser* activityLogBrowser;
    
    // Botones
    QPushButton* saveButton;
    QPushButton* cancelButton;
    QPushButton* deleteButton;
    
    void setupUI();
    void loadTaskData();
    void populateUsers();
    void populateAvailableTasks();
    void loadSubtasks();
    void loadDependencies();
    void loadHistory();
    void loadActivityLog();
    
private slots:
    void onSave();
    void onCancel();
    void onDelete();
    void onAddSubtask();
    void onRemoveSubtask();
    void onAddDependency();
    void onRemoveDependency();
    void onVersionSelected(int index);
    void onRestoreVersion();

public:
    explicit TaskDialog(shared_ptr<Board> board,
                       shared_ptr<Project> project,
                       shared_ptr<Task> task = nullptr,
                       QWidget *parent = nullptr);
    ~TaskDialog();
    
    shared_ptr<Task> getTask() const;
    bool wasAccepted() const;
};

#endif // TASK_DIALOG_H


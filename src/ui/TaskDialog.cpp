#include "ui/TaskDialog.h"
#include "utils/DateUtils.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QMessageBox>
#include <QInputDialog>
#include <QLabel>

using namespace std;

TaskDialog::TaskDialog(shared_ptr<Board> board,
                      shared_ptr<Project> project,
                      shared_ptr<Task> task,
                      QWidget *parent)
    : QDialog(parent), task(task), board(board), project(project),
      isNewTask(task == nullptr) {
    
    if (isNewTask && board) {
        // Crear nueva tarea
        this->task = make_shared<Task>(0, "", "");
    }
    
    setupUI();
    
    if (!isNewTask) {
        loadTaskData();
    }
}

TaskDialog::~TaskDialog() {}

void TaskDialog::setupUI() {
    setWindowTitle(isNewTask ? "Nueva Tarea" : "Editar Tarea");
    setMinimumSize(700, 600);
    
    // Establecer estilo claro para el diálogo
    setStyleSheet(
        "QDialog {"
        "  background-color: #ffffff;"
        "}"
        "QLabel {"
        "  color: #1a1a1a;"
        "}"
        "QLineEdit, QTextEdit, QComboBox, QSpinBox, QDateTimeEdit {"
        "  background-color: #ffffff;"
        "  color: #1a1a1a;"
        "  border: 1px solid #d0d0d0;"
        "  border-radius: 4px;"
        "  padding: 6px;"
        "}"
        "QLineEdit:focus, QTextEdit:focus, QComboBox:focus {"
        "  border: 2px solid #0078d4;"
        "}"
        "QListWidget {"
        "  background-color: #ffffff;"
        "  color: #1a1a1a;"
        "  border: 1px solid #d0d0d0;"
        "  border-radius: 4px;"
        "}"
        "QTextBrowser {"
        "  background-color: #fafafa;"
        "  color: #1a1a1a;"
        "  border: 1px solid #d0d0d0;"
        "  border-radius: 4px;"
        "}"
        "QPushButton {"
        "  background-color: #f0f0f0;"
        "  color: #1a1a1a;"
        "  border: 1px solid #d0d0d0;"
        "  border-radius: 4px;"
        "  padding: 8px 16px;"
        "  font-weight: 500;"
        "}"
        "QPushButton:hover {"
        "  background-color: #e5e5e5;"
        "  border: 1px solid #b0b0b0;"
        "}"
        "QPushButton:pressed {"
        "  background-color: #d0d0d0;"
        "}"
        "QTabWidget::pane {"
        "  background-color: #ffffff;"
        "  border: 1px solid #d0d0d0;"
        "  border-radius: 4px;"
        "}"
        "QTabBar::tab {"
        "  background-color: #f0f0f0;"
        "  color: #4a4a4a;"
        "  padding: 8px 16px;"
        "  border: 1px solid #d0d0d0;"
        "  border-bottom: none;"
        "  border-top-left-radius: 4px;"
        "  border-top-right-radius: 4px;"
        "}"
        "QTabBar::tab:selected {"
        "  background-color: #ffffff;"
        "  color: #1a1a1a;"
        "  font-weight: bold;"
        "}"
    );
    
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    // Pestañas
    QTabWidget* tabWidget = new QTabWidget(this);
    
    // Pestaña principal
    QWidget* mainTab = new QWidget();
    QFormLayout* mainForm = new QFormLayout(mainTab);
    
    titleEdit = new QLineEdit();
    titleEdit->setPlaceholderText("Título de la tarea");
    mainForm->addRow("Título:", titleEdit);
    
    descriptionEdit = new QTextEdit();
    descriptionEdit->setPlaceholderText("Descripción detallada");
    descriptionEdit->setMaximumHeight(150);
    mainForm->addRow("Descripción:", descriptionEdit);
    
    stateCombo = new QComboBox();
    if (board) {
        for (const auto& state : board->getStates()) {
            stateCombo->addItem(QString::fromStdString(state));
        }
    }
    mainForm->addRow("Estado:", stateCombo);
    
    userCombo = new QComboBox();
    userCombo->addItem("Sin asignar", -1);
    populateUsers();
    mainForm->addRow("Asignado a:", userCombo);
    
    dueDateEdit = new QDateTimeEdit();
    dueDateEdit->setDateTime(QDateTime::currentDateTime().addDays(7));
    dueDateEdit->setCalendarPopup(true);
    mainForm->addRow("Fecha límite:", dueDateEdit);
    
    prioritySpinBox = new QSpinBox();
    prioritySpinBox->setRange(1, 5);
    prioritySpinBox->setValue(3);
    mainForm->addRow("Prioridad (1-5):", prioritySpinBox);
    
    tagsEdit = new QLineEdit();
    tagsEdit->setPlaceholderText("etiqueta1, etiqueta2, etiqueta3");
    mainForm->addRow("Etiquetas:", tagsEdit);
    
    mainTab->setLayout(mainForm);
    tabWidget->addTab(mainTab, "Información General");
    
    // Pestaña de subtareas
    QWidget* subtasksTab = new QWidget();
    QVBoxLayout* subtasksLayout = new QVBoxLayout(subtasksTab);
    
    subtasksList = new QListWidget();
    subtasksLayout->addWidget(subtasksList);
    
    QHBoxLayout* subtasksButtons = new QHBoxLayout();
    addSubtaskButton = new QPushButton("+ Agregar Subtarea");
    removeSubtaskButton = new QPushButton("- Eliminar");
    subtasksButtons->addWidget(addSubtaskButton);
    subtasksButtons->addWidget(removeSubtaskButton);
    subtasksButtons->addStretch();
    subtasksLayout->addLayout(subtasksButtons);
    
    subtasksTab->setLayout(subtasksLayout);
    tabWidget->addTab(subtasksTab, "Subtareas");
    
    // Pestaña de dependencias
    QWidget* dependenciesTab = new QWidget();
    QVBoxLayout* dependenciesLayout = new QVBoxLayout(dependenciesTab);
    
    dependenciesList = new QListWidget();
    dependenciesLayout->addWidget(dependenciesList);
    
    QHBoxLayout* depButtons = new QHBoxLayout();
    availableTasksCombo = new QComboBox();
    populateAvailableTasks();
    addDependencyButton = new QPushButton("+ Agregar");
    removeDependencyButton = new QPushButton("- Eliminar");
    
    depButtons->addWidget(new QLabel("Tarea:"));
    depButtons->addWidget(availableTasksCombo);
    depButtons->addWidget(addDependencyButton);
    depButtons->addWidget(removeDependencyButton);
    depButtons->addStretch();
    dependenciesLayout->addLayout(depButtons);
    
    dependenciesTab->setLayout(dependenciesLayout);
    tabWidget->addTab(dependenciesTab, "Dependencias");
    
    // Pestaña de historial
    if (!isNewTask) {
        QWidget* historyTab = new QWidget();
        QVBoxLayout* historyLayout = new QVBoxLayout(historyTab);
        
        versionsList = new QListWidget();
        historyLayout->addWidget(new QLabel("Versiones anteriores:"));
        historyLayout->addWidget(versionsList);
        
        historyBrowser = new QTextBrowser();
        historyLayout->addWidget(historyBrowser);
        
        restoreVersionButton = new QPushButton("Restaurar Versión Seleccionada");
        historyLayout->addWidget(restoreVersionButton);
        
        historyTab->setLayout(historyLayout);
        tabWidget->addTab(historyTab, "Historial");
    }
    
    // Pestaña de registro de actividad
    if (!isNewTask) {
        QWidget* activityTab = new QWidget();
        QVBoxLayout* activityLayout = new QVBoxLayout(activityTab);
        
        activityLogBrowser = new QTextBrowser();
        activityLayout->addWidget(activityLogBrowser);
        
        activityTab->setLayout(activityLayout);
        tabWidget->addTab(activityTab, "Registro de Actividad");
    }
    
    mainLayout->addWidget(tabWidget);
    
    // Botones de acción
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    
    if (!isNewTask) {
        deleteButton = new QPushButton("Eliminar");
        deleteButton->setStyleSheet("background-color: #d32f2f; color: white;");
        buttonLayout->addWidget(deleteButton);
        connect(deleteButton, &QPushButton::clicked, this, &TaskDialog::onDelete);
    }
    
    buttonLayout->addStretch();
    
    saveButton = new QPushButton("Guardar");
    saveButton->setStyleSheet("background-color: #0078d4; color: white; padding: 8px 16px;");
    cancelButton = new QPushButton("Cancelar");
    
    buttonLayout->addWidget(saveButton);
    buttonLayout->addWidget(cancelButton);
    
    mainLayout->addLayout(buttonLayout);
    
    // Conexiones
    connect(saveButton, &QPushButton::clicked, this, &TaskDialog::onSave);
    connect(cancelButton, &QPushButton::clicked, this, &TaskDialog::onCancel);
    connect(addSubtaskButton, &QPushButton::clicked, this, &TaskDialog::onAddSubtask);
    connect(removeSubtaskButton, &QPushButton::clicked, this, &TaskDialog::onRemoveSubtask);
    connect(addDependencyButton, &QPushButton::clicked, this, &TaskDialog::onAddDependency);
    connect(removeDependencyButton, &QPushButton::clicked, this, &TaskDialog::onRemoveDependency);
    
    if (!isNewTask) {
        connect(versionsList, &QListWidget::currentRowChanged, this, &TaskDialog::onVersionSelected);
        connect(restoreVersionButton, &QPushButton::clicked, this, &TaskDialog::onRestoreVersion);
    }
    
    setLayout(mainLayout);
}

void TaskDialog::loadTaskData() {
    if (!task) return;
    
    titleEdit->setText(QString::fromStdString(task->getTitle()));
    descriptionEdit->setPlainText(QString::fromStdString(task->getDescription()));
    
    int stateIndex = stateCombo->findText(QString::fromStdString(task->getState()));
    if (stateIndex >= 0) {
        stateCombo->setCurrentIndex(stateIndex);
    }
    
    int userIndex = userCombo->findData(task->getAssignedUserId());
    if (userIndex >= 0) {
        userCombo->setCurrentIndex(userIndex);
    }
    
    auto dueDate = task->getDueDate();
    time_t time = chrono::system_clock::to_time_t(dueDate);
    dueDateEdit->setDateTime(QDateTime::fromSecsSinceEpoch(time));
    
    prioritySpinBox->setValue(task->getPriority());
    
    QString tags;
    for (const auto& tag : task->getTags()) {
        if (!tags.isEmpty()) tags += ", ";
        tags += QString::fromStdString(tag);
    }
    tagsEdit->setText(tags);
    
    loadSubtasks();
    loadDependencies();
    loadHistory();
    loadActivityLog();
}

void TaskDialog::populateUsers() {
    if (!project) return;
    
    auto users = project->getAllUsers();
    for (const auto& user : users) {
        userCombo->addItem(QString::fromStdString(user->getName()), user->getId());
    }
}

void TaskDialog::populateAvailableTasks() {
    if (!board) return;
    
    auto tasks = board->getAllTasks();
    for (const auto& t : tasks) {
        if (!task || t->getId() != task->getId()) {
            availableTasksCombo->addItem(
                QString::fromStdString(t->getTitle()),
                t->getId()
            );
        }
    }
}

void TaskDialog::loadSubtasks() {
    subtasksList->clear();
    
    if (!task) return;
    
    for (const auto& subtask : task->getSubtasks()) {
        QString item = QString::fromStdString(subtask->getTitle());
        if (subtask->isCompleted()) {
            item = "✓ " + item;
        }
        subtasksList->addItem(item);
    }
}

void TaskDialog::loadDependencies() {
    dependenciesList->clear();
    
    if (!task || !board) return;
    
    for (int depId : task->getDependencies()) {
        auto depTask = board->findTaskById(depId);
        if (depTask) {
            dependenciesList->addItem(
                QString::number(depId) + ": " + 
                QString::fromStdString(depTask->getTitle())
            );
        }
    }
}

void TaskDialog::loadHistory() {
    if (!task) return;
    
    versionsList->clear();
    
    const auto& history = task->getHistory();
    for (size_t i = 0; i < history.size(); ++i) {
        auto memento = history[i];
        QString item = "Versión " + QString::number(i + 1) + " - " +
                      QString::fromStdString(DateUtils::toDateTimeString(memento->getTimestamp())) +
                      " por " + QString::fromStdString(memento->getModifiedBy());
        versionsList->addItem(item);
    }
}

void TaskDialog::loadActivityLog() {
    if (!task) return;
    
    auto log = task->getActivityLog();
    if (!log) return;
    
    QString html = "<html><body style='font-family: monospace; font-size: 10pt;'>";
    html += "<h3>Registro de Actividad</h3>";
    
    for (const auto& entry : log->getEntries()) {
        html += "<p>" + QString::fromStdString(entry.toString()) + "</p>";
    }
    
    html += "</body></html>";
    
    activityLogBrowser->setHtml(html);
}

void TaskDialog::onSave() {
    if (titleEdit->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Error", "El título no puede estar vacío");
        return;
    }
    
    // Actualizar tarea
    task->setTitle(titleEdit->text().toStdString(), "Usuario Actual");
    task->setDescription(descriptionEdit->toPlainText().toStdString(), "Usuario Actual");
    task->setState(stateCombo->currentText().toStdString(), "Usuario Actual");
    
    int userId = userCombo->currentData().toInt();
    task->setAssignedUserId(userId, "Usuario Actual");
    
    time_t time = dueDateEdit->dateTime().toSecsSinceEpoch();
    task->setDueDate(chrono::system_clock::from_time_t(time));
    
    task->setPriority(prioritySpinBox->value());
    
    // Tags
    QStringList tagsList = tagsEdit->text().split(",", Qt::SkipEmptyParts);
    for (const QString& tag : tagsList) {
        task->addTag(tag.trimmed().toStdString());
    }
    
    accept();
}

void TaskDialog::onCancel() {
    reject();
}

void TaskDialog::onDelete() {
    auto reply = QMessageBox::question(this, "Confirmar",
                                      "¿Estás seguro de eliminar esta tarea?",
                                      QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        task = nullptr;  // Marcar para eliminación
        accept();
    }
}

void TaskDialog::onAddSubtask() {
    bool ok;
    QString title = QInputDialog::getText(this, "Nueva Subtarea",
                                         "Título de la subtarea:",
                                         QLineEdit::Normal, "", &ok);
    if (ok && !title.isEmpty()) {
        // Agregar subtarea
        auto subtask = make_shared<Subtask>(0, title.toStdString());
        task->addSubtask(subtask);
        loadSubtasks();
    }
}

void TaskDialog::onRemoveSubtask() {
    int row = subtasksList->currentRow();
    if (row >= 0) {
        // Eliminar subtarea
        // Implementación simplificada
        loadSubtasks();
    }
}

void TaskDialog::onAddDependency() {
    int taskId = availableTasksCombo->currentData().toInt();
    if (taskId > 0) {
        task->addDependency(taskId);
        loadDependencies();
    }
}

void TaskDialog::onRemoveDependency() {
    int row = dependenciesList->currentRow();
    if (row >= 0) {
        // Extraer ID y eliminar dependencia
        QString text = dependenciesList->currentItem()->text();
        int taskId = text.split(":")[0].toInt();
        task->removeDependency(taskId);
        loadDependencies();
    }
}

void TaskDialog::onVersionSelected(int index) {
    if (!task || index < 0) return;
    
    const auto& history = task->getHistory();
    if (index < static_cast<int>(history.size())) {
        auto memento = history[index];
        historyBrowser->setPlainText(QString::fromStdString(memento->toString()));
    }
}

void TaskDialog::onRestoreVersion() {
    int row = versionsList->currentRow();
    if (row >= 0) {
        auto reply = QMessageBox::question(this, "Confirmar",
                                          "¿Restaurar esta versión?",
                                          QMessageBox::Yes | QMessageBox::No);
        if (reply == QMessageBox::Yes) {
            const auto& history = task->getHistory();
            if (row < static_cast<int>(history.size())) {
                task->restoreFromMemento(history[row]);
                loadTaskData();
            }
        }
    }
}

shared_ptr<Task> TaskDialog::getTask() const {
    return task;
}

bool TaskDialog::wasAccepted() const {
    return result() == QDialog::Accepted;
}


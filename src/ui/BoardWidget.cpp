#include "ui/BoardWidget.h"
#include "ui/ColumnWidget.h"
#include <QScrollBar>
#include <QMessageBox>
#include <QGraphicsDropShadowEffect>

BoardWidget::BoardWidget(std::shared_ptr<Board> board,
                        const std::string& currentUserName,
                        QWidget *parent)
    : QWidget(parent), board(board), currentUserName(currentUserName) {
    setupUI();
}

BoardWidget::~BoardWidget() {}

void BoardWidget::setupUI() {
    mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    
    // Título del tablero - estilo Trello profesional
    QWidget* titleWidget = new QWidget();
    QHBoxLayout* titleLayout = new QHBoxLayout(titleWidget);
    titleLayout->setContentsMargins(16, 12, 16, 12);
    
    QLabel* titleLabel = new QLabel(QString::fromStdString(board->getName()));
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(16);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setStyleSheet("color: #172b4d;");
    titleLayout->addWidget(titleLabel);
    
    titleLayout->addStretch();
    
    titleWidget->setLayout(titleLayout);
    titleWidget->setStyleSheet("background: #fafbfc; border-bottom: 1px solid #dfe1e6;");
    
    mainLayout->addWidget(titleWidget);
    
    // Área de scroll para columnas
    scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    
    columnsContainer = new QWidget();
    columnsLayout = new QHBoxLayout(columnsContainer);
    columnsLayout->setSpacing(16);
    columnsLayout->setContentsMargins(20, 20, 20, 20);
    columnsLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    
    createColumns();
    populateTasks();
    
    columnsContainer->setLayout(columnsLayout);
    scrollArea->setWidget(columnsContainer);
    
    mainLayout->addWidget(scrollArea);
    setLayout(mainLayout);
    
    setStyleSheet(
        "BoardWidget {"
        "  background: qlineargradient(x1:0, y1:0, x2:1, y2:1, "
        "    stop:0 #5f1eb8, stop:1 #7c49ff);"
        "}"
        "QScrollArea {"
        "  background: transparent;"
        "  border: none;"
        "}"
        "QScrollBar:horizontal {"
        "  border: none;"
        "  background: rgba(255, 255, 255, 0.2);"
        "  height: 12px;"
        "}"
        "QScrollBar::handle:horizontal {"
        "  background: rgba(255, 255, 255, 0.5);"
        "  border-radius: 6px;"
        "  min-width: 40px;"
        "}"
        "QScrollBar::handle:horizontal:hover {"
        "  background: rgba(255, 255, 255, 0.8);"
        "}"
    );
}

void BoardWidget::createColumns() {
    if (!board) return;
    
    const auto& states = board->getStates();
    
    for (const auto& state : states) {
        ColumnWidget* column = new ColumnWidget(state, this);
        
        // Conectar señales
        connect(column, &ColumnWidget::taskDropped, this, &BoardWidget::onTaskCardMoved);
        connect(column, &ColumnWidget::addTaskRequested, this, &BoardWidget::onAddTaskClicked);
        
        columnsLayout->addWidget(column);
        stateColumns[state] = column;
    }
    
    columnsLayout->addStretch();
}

void BoardWidget::populateTasks() {
    if (!board) return;
    
    const auto& states = board->getStates();
    
    for (const auto& state : states) {
        auto tasks = board->getTasksByState(state);
        auto column = stateColumns[state];
        
        if (!column) continue;
        
        auto layout = column->getTasksLayout();
        
        // Actualizar contador
        column->setTaskCount(tasks.size());
        
        // Agregar tarjetas de tareas
        for (const auto& task : tasks) {
            TaskCard* card = new TaskCard(task, this);
            
            connect(card, &TaskCard::doubleClicked, this, &BoardWidget::onTaskCardClicked);
            connect(card, &TaskCard::clicked, this, &BoardWidget::onTaskCardClicked);
            
            layout->insertWidget(layout->count() - 1, card);  // Antes del stretch
            taskCards[task->getId()] = card;
        }
    }
}

void BoardWidget::onAddTaskClicked(const std::string& state) {
    emit newTaskRequested(state);
}

void BoardWidget::onTaskCardClicked(int taskId) {
    emit taskSelected(taskId);
}

void BoardWidget::onTaskCardMoved(int taskId, const std::string& newState) {
    if (board) {
        board->moveTask(taskId, newState, currentUserName);
        refresh();
        emit taskMoved(taskId, newState);
    }
}

void BoardWidget::onRefresh() {
    refresh();
}

std::shared_ptr<Board> BoardWidget::getBoard() const {
    return board;
}

void BoardWidget::refresh() {
    // Limpiar todas las tarjetas
    for (auto& pair : taskCards) {
        delete pair.second;
    }
    taskCards.clear();
    
    // Volver a poblar
    populateTasks();
}

void BoardWidget::addTask(std::shared_ptr<Task> task) {
    if (!task || !board) return;
    
    board->addTask(task, task->getState());
    refresh();
}

void BoardWidget::updateTask(std::shared_ptr<Task> task) {
    if (!task) return;
    
    auto it = taskCards.find(task->getId());
    if (it != taskCards.end()) {
        it->second->updateTask(task);
    }
}

void BoardWidget::removeTask(int taskId) {
    if (!board) return;
    
    board->removeTask(taskId);
    
    auto it = taskCards.find(taskId);
    if (it != taskCards.end()) {
        delete it->second;
        taskCards.erase(it);
    }
    
    refresh();
}


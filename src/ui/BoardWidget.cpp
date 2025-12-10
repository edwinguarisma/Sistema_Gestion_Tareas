#include "ui/BoardWidget.h"
#include "ui/ColumnWidget.h"
#include <QScrollBar>
#include <QMessageBox>
#include <QGraphicsDropShadowEffect>

using namespace std;

BoardWidget::BoardWidget(shared_ptr<Board> board,
                        const string& currentUserName,
                        QWidget *parent)
    : QWidget(parent), board(board), currentUserName(currentUserName) {
    setupUI();
}

BoardWidget::~BoardWidget() {}

void BoardWidget::setupUI() {
    mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    
    // Título del tablero - minimalista moderno
    QWidget* titleWidget = new QWidget();
    QHBoxLayout* titleLayout = new QHBoxLayout(titleWidget);
    titleLayout->setContentsMargins(24, 16, 24, 16);
    
    QLabel* titleLabel = new QLabel(QString::fromStdString(board->getName()));
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(18);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setStyleSheet("color: #1a1a1a;");
    titleLayout->addWidget(titleLabel);
    
    titleLayout->addStretch();
    
    titleWidget->setLayout(titleLayout);
    titleWidget->setStyleSheet(
        "background: #ffffff; "
        "border-bottom: 1px solid #e5e5e5;"
    );
    
    mainLayout->addWidget(titleWidget);
    
    // Área de scroll para columnas
    scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    
    columnsContainer = new QWidget();
    columnsContainer->setStyleSheet("QWidget { background: #f7f8fa; }");
    
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
        "  background-color: #f7f8fa;"
        "}"
        "QScrollArea {"
        "  background-color: #f7f8fa;"
        "  border: none;"
        "}"
        "QScrollBar:horizontal {"
        "  border: none;"
        "  background: #e5e5e5;"
        "  height: 10px;"
        "}"
        "QScrollBar::handle:horizontal {"
        "  background: #c0c0c0;"
        "  border-radius: 5px;"
        "  min-width: 40px;"
        "}"
        "QScrollBar::handle:horizontal:hover {"
        "  background: #a0a0a0;"
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

void BoardWidget::onAddTaskClicked(const string& state) {
    emit newTaskRequested(state);
}

void BoardWidget::onTaskCardClicked(int taskId) {
    emit taskSelected(taskId);
}

void BoardWidget::onTaskCardMoved(int taskId, const string& newState) {
    if (board) {
        board->moveTask(taskId, newState, currentUserName);
        refresh();
        emit taskMoved(taskId, newState);
    }
}

void BoardWidget::onRefresh() {
    refresh();
}

shared_ptr<Board> BoardWidget::getBoard() const {
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

void BoardWidget::addTask(shared_ptr<Task> task) {
    if (!task || !board) return;
    
    board->addTask(task, task->getState());
    refresh();
}

void BoardWidget::updateTask(shared_ptr<Task> task) {
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


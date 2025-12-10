#ifndef BOARD_WIDGET_H
#define BOARD_WIDGET_H

#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QScrollArea>
#include <QPushButton>
#include <QFrame>
#include <memory>
#include <map>
#include <string>
#include "models/Board.h"
#include "TaskCard.h"
#include "ColumnWidget.h"

/**
 * @brief Widget que muestra un tablero Kanban
 */
class BoardWidget : public QWidget {
    Q_OBJECT

private:
    std::shared_ptr<Board> board;
    std::string currentUserName;
    
    QVBoxLayout* mainLayout;
    QScrollArea* scrollArea;
    QWidget* columnsContainer;
    QHBoxLayout* columnsLayout;
    
    // Mapa de columnas por estado
    std::map<std::string, ColumnWidget*> stateColumns;
    
    // Mapa de tarjetas de tareas
    std::map<int, TaskCard*> taskCards;
    
    void setupUI();
    void createColumns();
    void populateTasks();
    
private slots:
    void onAddTaskClicked(const std::string& state);
    void onTaskCardClicked(int taskId);
    void onTaskCardMoved(int taskId, const std::string& newState);
    void onRefresh();

signals:
    void taskSelected(int taskId);
    void taskMoved(int taskId, const std::string& newState);
    void newTaskRequested(const std::string& state);

public:
    explicit BoardWidget(std::shared_ptr<Board> board, 
                        const std::string& currentUserName,
                        QWidget *parent = nullptr);
    ~BoardWidget();
    
    std::shared_ptr<Board> getBoard() const;
    void refresh();
    void addTask(std::shared_ptr<Task> task);
    void updateTask(std::shared_ptr<Task> task);
    void removeTask(int taskId);
};

#endif // BOARD_WIDGET_H


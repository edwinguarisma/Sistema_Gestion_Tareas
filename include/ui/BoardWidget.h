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

using namespace std;

/**
 * @brief Widget que muestra un tablero Kanban
 */
class BoardWidget : public QWidget {
    Q_OBJECT

private:
    shared_ptr<Board> board;
    string currentUserName;
    
    QVBoxLayout* mainLayout;
    QScrollArea* scrollArea;
    QWidget* columnsContainer;
    QHBoxLayout* columnsLayout;
    
    // Mapa de columnas por estado
    map<string, ColumnWidget*> stateColumns;
    
    // Mapa de tarjetas de tareas
    map<int, TaskCard*> taskCards;
    
    void setupUI();
    void createColumns();
    void populateTasks();
    
private slots:
    void onAddTaskClicked(const string& state);
    void onTaskCardClicked(int taskId);
    void onTaskCardMoved(int taskId, const string& newState);
    void onRefresh();

signals:
    void taskSelected(int taskId);
    void taskMoved(int taskId, const string& newState);
    void newTaskRequested(const string& state);

public:
    explicit BoardWidget(shared_ptr<Board> board, 
                        const string& currentUserName,
                        QWidget *parent = nullptr);
    ~BoardWidget();
    
    shared_ptr<Board> getBoard() const;
    void refresh();
    void addTask(shared_ptr<Task> task);
    void updateTask(shared_ptr<Task> task);
    void removeTask(int taskId);
};

#endif // BOARD_WIDGET_H


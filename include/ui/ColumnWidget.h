#ifndef COLUMN_WIDGET_H
#define COLUMN_WIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QScrollArea>
#include <QPushButton>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <string>

/**
 * @brief Widget de columna que acepta drops de tareas
 */
class ColumnWidget : public QWidget {
    Q_OBJECT

private:
    std::string stateName;
    QVBoxLayout* mainLayout;
    QWidget* header;
    QLabel* titleLabel;
    QLabel* countLabel;
    QScrollArea* scrollArea;
    QWidget* tasksContainer;
    QVBoxLayout* tasksLayout;
    QPushButton* addButton;
    
    bool isHighlighted;
    
    void setupUI();
    void updateHighlight(bool highlight);

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dragLeaveEvent(QDragLeaveEvent *event) override;
    void dropEvent(QDropEvent *event) override;

signals:
    void taskDropped(int taskId, const std::string& targetState);
    void addTaskRequested(const std::string& state);

public:
    explicit ColumnWidget(const std::string& stateName, QWidget *parent = nullptr);
    ~ColumnWidget();
    
    std::string getStateName() const { return stateName; }
    QVBoxLayout* getTasksLayout() { return tasksLayout; }
    void setTaskCount(int count);
};

#endif // COLUMN_WIDGET_H


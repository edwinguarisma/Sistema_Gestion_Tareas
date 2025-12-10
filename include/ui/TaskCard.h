#ifndef TASK_CARD_H
#define TASK_CARD_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QMouseEvent>
#include <QDrag>
#include <QMimeData>
#include <QEvent>
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <QEnterEvent>
#endif
#include <memory>
#include "models/Task.h"

/**
 * @brief Widget que representa una tarjeta de tarea
 * Implementa drag and drop para mover tareas
 */
class TaskCard : public QWidget {
    Q_OBJECT

private:
    std::shared_ptr<Task> task;
    
    QVBoxLayout* layout;
    QLabel* titleLabel;
    QLabel* infoLabel;
    QLabel* tagsLabel;
    QLabel* priorityLabel;
    
    QPoint dragStartPosition;
    bool isDragging;
    
    void setupUI();
    void updateDisplay();
    QString getPriorityColor() const;
    QString getDueDateText() const;
    QString getAssignedUserText() const;

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    void enterEvent(QEnterEvent *event) override;
#else
    void enterEvent(QEvent *event) override;
#endif
    void leaveEvent(QEvent *event) override;

signals:
    void clicked(int taskId);
    void doubleClicked(int taskId);
    void dragStarted(int taskId);
    void dropped(int taskId, const QString& targetState);

public:
    explicit TaskCard(std::shared_ptr<Task> task, QWidget *parent = nullptr);
    ~TaskCard();
    
    std::shared_ptr<Task> getTask() const;
    void updateTask(std::shared_ptr<Task> task);
    void setHighlighted(bool highlighted);
};

#endif // TASK_CARD_H


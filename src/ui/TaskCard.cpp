#include "ui/TaskCard.h"
#include "utils/DateUtils.h"
#include <QFrame>
#include <QApplication>
#include <QPainter>
#include <QStyleOption>
#include <QGraphicsDropShadowEffect>
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <QEnterEvent>
#endif

TaskCard::TaskCard(std::shared_ptr<Task> task, QWidget *parent)
    : QWidget(parent), task(task), isDragging(false) {
    setupUI();
    setAcceptDrops(true);
}

TaskCard::~TaskCard() {}

void TaskCard::setupUI() {
    layout = new QVBoxLayout(this);
    layout->setContentsMargins(10, 10, 10, 10);
    layout->setSpacing(6);
    
    // Título - estilo Trello
    titleLabel = new QLabel(this);
    titleLabel->setWordWrap(true);
    QFont titleFont = titleLabel->font();
    titleFont.setBold(false);
    titleFont.setPointSize(10);
    titleLabel->setFont(titleFont);
    titleLabel->setStyleSheet("color: #172b4d;");
    layout->addWidget(titleLabel);
    
    // Información
    infoLabel = new QLabel(this);
    QFont infoFont = infoLabel->font();
    infoFont.setPointSize(8);
    infoLabel->setFont(infoFont);
    infoLabel->setStyleSheet("color: #5e6c84;");
    layout->addWidget(infoLabel);
    
    // Tags
    tagsLabel = new QLabel(this);
    QFont tagsFont = tagsLabel->font();
    tagsFont.setPointSize(8);
    tagsLabel->setFont(tagsFont);
    tagsLabel->setStyleSheet(
        "background-color: #dfe1e6; "
        "padding: 3px 6px; "
        "border-radius: 3px; "
        "color: #5e6c84;"
    );
    layout->addWidget(tagsLabel);
    
    // Prioridad (badge discreto en esquina)
    priorityLabel = new QLabel(this);
    priorityLabel->setFixedHeight(4);
    priorityLabel->setStyleSheet("border-radius: 2px;");
    layout->addWidget(priorityLabel);
    
    setLayout(layout);
    
    setStyleSheet(
        "TaskCard {"
        "  background: #fbfbff;"
        "  border: 1px solid rgba(0, 0, 0, 0.08);"
        "  border-radius: 4px;"
        "}"
    );
    
    QGraphicsDropShadowEffect* shadow = new QGraphicsDropShadowEffect();
    shadow->setBlurRadius(6);
    shadow->setColor(QColor(0, 0, 0, 30));
    shadow->setOffset(0, 2);
    setGraphicsEffect(shadow);
    
    setMinimumHeight(90);
    setMaximumWidth(300);
    setCursor(Qt::PointingHandCursor);
    
    updateDisplay();
}

void TaskCard::updateDisplay() {
    if (!task) return;
    
    // Actualizar título
    titleLabel->setText(QString::fromStdString(task->getTitle()));
    
    // Actualizar información
    QString info;
    info += "ID: " + QString::number(task->getId()) + "\n";
    info += getDueDateText() + "\n";
    info += getAssignedUserText();
    
    if (task->hasDependencies()) {
        info += "\n⚠ Tiene dependencias (" + 
                QString::number(task->getDependencies().size()) + ")";
    }
    
    if (!task->getSubtasks().empty()) {
        info += "\n☑ Subtareas: " + 
                QString::number(static_cast<int>(task->getSubtaskCompletionPercentage())) + "%";
    }
    
    infoLabel->setText(info);
    
    // Actualizar tags
    QString tagsText;
    const auto& tags = task->getTags();
    for (const auto& tag : tags) {
        tagsText += "🏷 " + QString::fromStdString(tag) + " ";
    }
    tagsLabel->setText(tagsText);
    tagsLabel->setVisible(!tags.empty());
    
    // Actualizar indicador de prioridad
    priorityLabel->setStyleSheet("background-color: " + getPriorityColor() + ";");
}

QString TaskCard::getPriorityColor() const {
    if (!task) return "#c1c7d0";
    
    switch (task->getPriority()) {
        case 5: return "#eb5a46";  // Rojo - Muy alta (Trello red)
        case 4: return "#ff9f1a";  // Naranja - Alta (Trello orange)
        case 3: return "#f2d600";  // Amarillo - Media (Trello yellow)
        case 2: return "#61bd4f";  // Verde - Baja (Trello green)
        case 1: return "#0079bf";  // Azul - Muy baja (Trello blue)
        default: return "#c1c7d0";
    }
}

QString TaskCard::getDueDateText() const {
    if (!task) return "";
    
    auto dueDate = task->getDueDate();
    auto now = std::chrono::system_clock::now();
    
    if (dueDate <= now) {
        return "📅 Sin fecha límite";
    }
    
    std::string dateStr = DateUtils::toDateString(dueDate);
    int daysLeft = task->getDaysUntilDue();
    
    QString text = "📅 Vence: " + QString::fromStdString(dateStr);
    
    if (task->isOverdue()) {
        text += " (¡VENCIDA!)";
    } else if (daysLeft <= 1) {
        text += " (¡HOY/MAÑANA!)";
    } else if (daysLeft <= 7) {
        text += " (" + QString::number(daysLeft) + " días)";
    }
    
    return text;
}

QString TaskCard::getAssignedUserText() const {
    if (!task) return "";
    
    int userId = task->getAssignedUserId();
    if (userId < 0) {
        return "👤 Sin asignar";
    }
    
    return "👤 Usuario ID: " + QString::number(userId);
}

void TaskCard::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        dragStartPosition = event->pos();
        isDragging = false;
    }
    QWidget::mousePressEvent(event);
}

void TaskCard::mouseMoveEvent(QMouseEvent *event) {
    if (!(event->buttons() & Qt::LeftButton)) {
        return;
    }
    
    if ((event->pos() - dragStartPosition).manhattanLength() < QApplication::startDragDistance()) {
        return;
    }
    
    // Iniciar drag
    QDrag *drag = new QDrag(this);
    QMimeData *mimeData = new QMimeData;
    
    mimeData->setText(QString::number(task->getId()));
    mimeData->setData("application/x-task-id", QString::number(task->getId()).toUtf8());
    drag->setMimeData(mimeData);
    
    // Crear pixmap del widget para mostrar durante el drag
    QPixmap pixmap(size());
    render(&pixmap);
    drag->setPixmap(pixmap.scaled(250, 150, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    drag->setHotSpot(QPoint(pixmap.width() / 2, pixmap.height() / 2));
    
    isDragging = true;
    emit dragStarted(task->getId());
    
    Qt::DropAction dropAction = drag->exec(Qt::MoveAction);
    
    isDragging = false;
}

void TaskCard::mouseReleaseEvent(QMouseEvent *event) {
    if (!isDragging && event->button() == Qt::LeftButton) {
        emit clicked(task->getId());
    }
    QWidget::mouseReleaseEvent(event);
}

void TaskCard::mouseDoubleClickEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        emit doubleClicked(task->getId());
    }
    QWidget::mouseDoubleClickEvent(event);
}

void TaskCard::dragEnterEvent(QDragEnterEvent *event) {
    if (event->mimeData()->hasFormat("application/x-task-id")) {
        event->acceptProposedAction();
    }
}

void TaskCard::dragMoveEvent(QDragMoveEvent *event) {
    if (event->mimeData()->hasFormat("application/x-task-id")) {
        event->acceptProposedAction();
    }
}

void TaskCard::dropEvent(QDropEvent *event) {
    if (event->mimeData()->hasFormat("application/x-task-id")) {
        int taskId = event->mimeData()->data("application/x-task-id").toInt();
        emit dropped(taskId, QString::fromStdString(task->getState()));
        event->acceptProposedAction();
    }
}

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
void TaskCard::enterEvent(QEnterEvent *event) {
#else
void TaskCard::enterEvent(QEvent *event) {
#endif
    setStyleSheet(
        "TaskCard {"
        "  background: #f4f5f7;"
        "  border: none;"
        "  border-radius: 3px;"
        "}"
    );
    
    // Sombra más pronunciada en hover
    QGraphicsDropShadowEffect* shadow = new QGraphicsDropShadowEffect();
    shadow->setBlurRadius(8);
    shadow->setColor(QColor(0, 0, 0, 40));
    shadow->setOffset(0, 2);
    setGraphicsEffect(shadow);
    
    QWidget::enterEvent(event);
}

void TaskCard::leaveEvent(QEvent *event) {
    setStyleSheet(
        "TaskCard {"
        "  background: #ffffff;"
        "  border: none;"
        "  border-radius: 3px;"
        "}"
    );
    
    // Volver a sombra sutil
    QGraphicsDropShadowEffect* shadow = new QGraphicsDropShadowEffect();
    shadow->setBlurRadius(1);
    shadow->setColor(QColor(0, 0, 0, 20));
    shadow->setOffset(0, 1);
    setGraphicsEffect(shadow);
    
    QWidget::leaveEvent(event);
}

std::shared_ptr<Task> TaskCard::getTask() const {
    return task;
}

void TaskCard::updateTask(std::shared_ptr<Task> newTask) {
    task = newTask;
    updateDisplay();
}

void TaskCard::setHighlighted(bool highlighted) {
    if (highlighted) {
        setStyleSheet(
            "TaskCard {"
            "  background: #fff4e6;"
            "  border: none;"
            "  border-radius: 3px;"
            "}"
        );
        
        QGraphicsDropShadowEffect* shadow = new QGraphicsDropShadowEffect();
        shadow->setBlurRadius(8);
        shadow->setColor(QColor(255, 153, 31, 100));
        shadow->setOffset(0, 2);
        setGraphicsEffect(shadow);
    } else {
        setStyleSheet(
            "TaskCard {"
            "  background: #ffffff;"
            "  border: none;"
            "  border-radius: 3px;"
            "}"
        );
        
        QGraphicsDropShadowEffect* shadow = new QGraphicsDropShadowEffect();
        shadow->setBlurRadius(1);
        shadow->setColor(QColor(0, 0, 0, 20));
        shadow->setOffset(0, 1);
        setGraphicsEffect(shadow);
    }
}


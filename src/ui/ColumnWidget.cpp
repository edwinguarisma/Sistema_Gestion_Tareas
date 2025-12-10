#include "ui/ColumnWidget.h"
#include <QGraphicsDropShadowEffect>
#include <QMimeData>

ColumnWidget::ColumnWidget(const std::string& stateName, QWidget *parent)
    : QWidget(parent), stateName(stateName), isHighlighted(false) {
    setupUI();
    setAcceptDrops(true);
}

ColumnWidget::~ColumnWidget() {}

void ColumnWidget::setupUI() {
    mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(8);
    mainLayout->setContentsMargins(8, 8, 8, 8);
    
    // Header con título y contador
    header = new QWidget();
    QHBoxLayout* headerLayout = new QHBoxLayout(header);
    headerLayout->setContentsMargins(0, 0, 0, 0);
    
    titleLabel = new QLabel(QString::fromStdString(stateName));
    QFont titleFont = titleLabel->font();
    titleFont.setBold(false);
    titleFont.setPointSize(12);
    titleLabel->setFont(titleFont);
    titleLabel->setStyleSheet("color: #5e6c84;");
    headerLayout->addWidget(titleLabel);
    
    countLabel = new QLabel("0");
    countLabel->setStyleSheet(
        "background-color: #dfe1e6; "
        "color: #5e6c84; "
        "border-radius: 8px; "
        "padding: 1px 6px; "
        "font-size: 9pt;"
    );
    headerLayout->addWidget(countLabel);
    
    headerLayout->addStretch();
    header->setLayout(headerLayout);
    mainLayout->addWidget(header);
    
    // Área de scroll para tareas
    scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setStyleSheet(
        "QScrollArea { "
        "  border: none; "
        "  background: transparent; "
        "}"
        "QScrollBar:vertical {"
        "  border: none;"
        "  background: transparent;"
        "  width: 8px;"
        "}"
        "QScrollBar::handle:vertical {"
        "  background: #c1c7d0;"
        "  border-radius: 4px;"
        "  min-height: 20px;"
        "}"
        "QScrollBar::handle:vertical:hover {"
        "  background: #a5adba;"
        "}"
    );
    
    tasksContainer = new QWidget();
    tasksLayout = new QVBoxLayout(tasksContainer);
    tasksLayout->setSpacing(8);
    tasksLayout->setContentsMargins(4, 4, 4, 4);
    tasksLayout->addStretch();
    
    tasksContainer->setLayout(tasksLayout);
    scrollArea->setWidget(tasksContainer);
    
    mainLayout->addWidget(scrollArea, 1);
    
    // Botón para agregar tarea - estilo Trello
    addButton = new QPushButton("+ Añadir tarea");
    addButton->setCursor(Qt::PointingHandCursor);
    addButton->setStyleSheet(
        "QPushButton {"
        "  background: transparent;"
        "  color: #5e6c84;"
        "  border: none;"
        "  padding: 8px;"
        "  border-radius: 3px;"
        "  text-align: left;"
        "  font-size: 10pt;"
        "}"
        "QPushButton:hover {"
        "  background: #dfe1e6;"
        "  color: #172b4d;"
        "}"
    );
    
    connect(addButton, &QPushButton::clicked, [this]() {
        emit addTaskRequested(stateName);
    });
    
    mainLayout->addWidget(addButton);
    
    setLayout(mainLayout);
    
    // Estilo de la columna
    setMinimumWidth(272);
    setMaximumWidth(272);
    
    updateHighlight(false);

    QGraphicsDropShadowEffect* shadow = new QGraphicsDropShadowEffect(this);
    shadow->setBlurRadius(12);
    shadow->setOffset(0, 4);
    shadow->setColor(QColor(0, 0, 0, 40));
    setGraphicsEffect(shadow);
}

void ColumnWidget::updateHighlight(bool highlight) {
    isHighlighted = highlight;
    
    if (highlight) {
        setStyleSheet(
            "ColumnWidget {"
            "  background: #ebecf0;"
            "  border: 3px solid #5aac44;"
            "  border-radius: 3px;"
            "}"
        );
    } else {
        setStyleSheet(
            "ColumnWidget {"
            "  background: #ebecf0;"
            "  border: 1px solid rgba(0, 0, 0, 0.1);"
            "  border-radius: 3px;"
            "}"
        );
    }
}

void ColumnWidget::dragEnterEvent(QDragEnterEvent *event) {
    if (event->mimeData()->hasFormat("application/x-task-id")) {
        event->acceptProposedAction();
        updateHighlight(true);
    }
}

void ColumnWidget::dragMoveEvent(QDragMoveEvent *event) {
    if (event->mimeData()->hasFormat("application/x-task-id")) {
        event->acceptProposedAction();
    }
}

void ColumnWidget::dragLeaveEvent(QDragLeaveEvent *event) {
    updateHighlight(false);
    QWidget::dragLeaveEvent(event);
}

void ColumnWidget::dropEvent(QDropEvent *event) {
    if (event->mimeData()->hasFormat("application/x-task-id")) {
        int taskId = event->mimeData()->data("application/x-task-id").toInt();
        emit taskDropped(taskId, stateName);
        event->acceptProposedAction();
        updateHighlight(false);
    }
}

void ColumnWidget::setTaskCount(int count) {
    countLabel->setText(QString::number(count));
}


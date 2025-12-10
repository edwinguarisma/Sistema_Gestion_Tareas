#include "ui/ColumnWidget.h"
#include <QGraphicsDropShadowEffect>
#include <QMimeData>

using namespace std;

ColumnWidget::ColumnWidget(const string& stateName, QWidget *parent)
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
    titleFont.setBold(true);
    titleFont.setPointSize(12);
    titleLabel->setFont(titleFont);
    titleLabel->setStyleSheet("color: #1a1a1a;");
    headerLayout->addWidget(titleLabel);
    
    countLabel = new QLabel("0");
    countLabel->setStyleSheet(
        "background-color: #e5e5e5; "
        "color: #4a4a4a; "
        "border-radius: 10px; "
        "padding: 2px 8px; "
        "font-size: 10pt; "
        "font-weight: bold;"
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
        "  background: #f0f0f0;"
        "  width: 6px;"
        "  border-radius: 3px;"
        "}"
        "QScrollBar::handle:vertical {"
        "  background: #c0c0c0;"
        "  border-radius: 3px;"
        "  min-height: 30px;"
        "}"
        "QScrollBar::handle:vertical:hover {"
        "  background: #a0a0a0;"
        "}"
    );
    
    tasksContainer = new QWidget();
    tasksContainer->setStyleSheet("QWidget { background-color: transparent; }");
    
    tasksLayout = new QVBoxLayout(tasksContainer);
    tasksLayout->setSpacing(8);
    tasksLayout->setContentsMargins(4, 4, 4, 4);
    tasksLayout->addStretch();
    
    tasksContainer->setLayout(tasksLayout);
    scrollArea->setWidget(tasksContainer);
    
    mainLayout->addWidget(scrollArea, 1);
    
    // Botón para agregar tarea - minimalista
    addButton = new QPushButton("+ Añadir tarea");
    addButton->setCursor(Qt::PointingHandCursor);
    addButton->setStyleSheet(
        "QPushButton {"
        "  background: #ffffff;"
        "  color: #4a4a4a;"
        "  border: 2px dashed #d0d0d0;"
        "  padding: 10px;"
        "  border-radius: 6px;"
        "  text-align: center;"
        "  font-size: 10pt;"
        "  font-weight: 500;"
        "}"
        "QPushButton:hover {"
        "  background: #f7f8fa;"
        "  border: 2px dashed #1a1a1a;"
        "  color: #1a1a1a;"
        "}"
    );
    
    connect(addButton, &QPushButton::clicked, [this]() {
        emit addTaskRequested(stateName);
    });
    
    mainLayout->addWidget(addButton);
    
    setLayout(mainLayout);
    
    // Estilo de la columna
    setMinimumWidth(300);
    setMaximumWidth(320);
    
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
            "  background-color: #ffffff;"
            "  border: 3px solid #4CAF50;"
            "  border-radius: 8px;"
            "}"
        );
    } else {
        setStyleSheet(
            "ColumnWidget {"
            "  background-color: #ffffff;"
            "  border: 1px solid #d0d0d0;"
            "  border-radius: 8px;"
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


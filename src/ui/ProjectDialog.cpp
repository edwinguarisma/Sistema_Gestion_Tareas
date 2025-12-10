#include "ui/ProjectDialog.h"
#include <QLabel>
#include <QFormLayout>
#include <QMessageBox>

using namespace std;

ProjectDialog::ProjectDialog(shared_ptr<Project> project, QWidget *parent)
    : QDialog(parent), project(project), 
      isNewProject(project == nullptr), accepted(false) {
    setupUI();
    if (!isNewProject) {
        loadProjectData();
    }
}

ProjectDialog::~ProjectDialog() {}

void ProjectDialog::setupUI() {
    setWindowTitle(isNewProject ? "Nuevo Proyecto" : "Editar Proyecto");
    setMinimumWidth(500);
    setMinimumHeight(300);
    
    // Establecer estilo claro para el diálogo
    setStyleSheet(
        "QDialog {"
        "  background-color: #ffffff;"
        "}"
        "QLabel {"
        "  color: #1a1a1a;"
        "  font-size: 10pt;"
        "}"
        "QLineEdit, QTextEdit {"
        "  background-color: #ffffff;"
        "  color: #1a1a1a;"
        "  border: 1px solid #d0d0d0;"
        "  border-radius: 4px;"
        "  padding: 8px;"
        "  font-size: 10pt;"
        "}"
        "QLineEdit:focus, QTextEdit:focus {"
        "  border: 2px solid #0078d4;"
        "}"
        "QPushButton {"
        "  background-color: #f0f0f0;"
        "  color: #1a1a1a;"
        "  border: 1px solid #d0d0d0;"
        "  border-radius: 4px;"
        "  padding: 10px 20px;"
        "  font-size: 10pt;"
        "  font-weight: 500;"
        "}"
        "QPushButton:hover {"
        "  background-color: #e5e5e5;"
        "  border: 1px solid #b0b0b0;"
        "}"
        "QPushButton:pressed {"
        "  background-color: #d0d0d0;"
        "}"
        "QPushButton:disabled {"
        "  background-color: #f5f5f5;"
        "  color: #a0a0a0;"
        "}"
    );
    
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    // Formulario
    QFormLayout* formLayout = new QFormLayout();
    
    nameEdit = new QLineEdit(this);
    nameEdit->setPlaceholderText("Nombre del proyecto");
    formLayout->addRow("Nombre:", nameEdit);
    
    descriptionEdit = new QTextEdit(this);
    descriptionEdit->setPlaceholderText("Descripción del proyecto");
    descriptionEdit->setMaximumHeight(150);
    formLayout->addRow("Descripción:", descriptionEdit);
    
    mainLayout->addLayout(formLayout);
    
    // Botones
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    
    saveButton = new QPushButton("Guardar", this);
    saveButton->setEnabled(false);
    cancelButton = new QPushButton("Cancelar", this);
    
    buttonLayout->addWidget(saveButton);
    buttonLayout->addWidget(cancelButton);
    
    mainLayout->addLayout(buttonLayout);
    
    // Conexiones
    connect(nameEdit, &QLineEdit::textChanged, this, &ProjectDialog::validateInput);
    connect(saveButton, &QPushButton::clicked, this, &ProjectDialog::onSave);
    connect(cancelButton, &QPushButton::clicked, this, &ProjectDialog::onCancel);
    
    setLayout(mainLayout);
}

void ProjectDialog::loadProjectData() {
    if (project) {
        nameEdit->setText(QString::fromStdString(project->getName()));
        descriptionEdit->setPlainText(QString::fromStdString(project->getDescription()));
    }
}

void ProjectDialog::onSave() {
    if (nameEdit->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Error", "El nombre del proyecto no puede estar vacío");
        return;
    }
    
    accepted = true;
    accept();
}

void ProjectDialog::onCancel() {
    accepted = false;
    reject();
}

void ProjectDialog::validateInput() {
    saveButton->setEnabled(!nameEdit->text().trimmed().isEmpty());
}

shared_ptr<Project> ProjectDialog::getProject() const {
    return project;
}

bool ProjectDialog::wasAccepted() const {
    return accepted;
}

QString ProjectDialog::getName() const {
    return nameEdit->text().trimmed();
}

QString ProjectDialog::getDescription() const {
    return descriptionEdit->toPlainText().trimmed();
}


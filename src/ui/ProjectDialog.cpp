#include "ui/ProjectDialog.h"
#include <QLabel>
#include <QFormLayout>
#include <QMessageBox>

ProjectDialog::ProjectDialog(std::shared_ptr<Project> project, QWidget *parent)
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

std::shared_ptr<Project> ProjectDialog::getProject() const {
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


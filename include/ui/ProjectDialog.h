#ifndef PROJECT_DIALOG_H
#define PROJECT_DIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QTextEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <memory>
#include "models/Project.h"

using namespace std;

/**
 * @brief Diálogo para crear/editar proyectos
 */
class ProjectDialog : public QDialog {
    Q_OBJECT

private:
    shared_ptr<Project> project;
    bool isNewProject;
    bool accepted;
    
    QLineEdit* nameEdit;
    QTextEdit* descriptionEdit;
    QPushButton* saveButton;
    QPushButton* cancelButton;
    
    void setupUI();
    void loadProjectData();

private slots:
    void onSave();
    void onCancel();
    void validateInput();

public:
    explicit ProjectDialog(shared_ptr<Project> project = nullptr,
                          QWidget *parent = nullptr);
    ~ProjectDialog();
    
    shared_ptr<Project> getProject() const;
    bool wasAccepted() const;
    
    QString getName() const;
    QString getDescription() const;
};

#endif // PROJECT_DIALOG_H


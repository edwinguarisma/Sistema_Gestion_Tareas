#ifndef PROJECT_MANAGER_H
#define PROJECT_MANAGER_H

#include <memory>
#include <vector>
#include <string>
#include "models/Project.h"
using namespace std;

/**
 * @brief Gestor centralizado de proyectos (Patrón Singleton)
 * Administra todos los proyectos de la aplicación
 */
class ProjectManager {
private:
    static shared_ptr<ProjectManager> instance;
    vector<shared_ptr<Project>> projects;
    shared_ptr<Project> currentProject;
    int nextProjectId;
    
    // Constructor privado (Singleton)
    ProjectManager();

public:
    // Destructor
    ~ProjectManager();
    
    // Singleton
    static shared_ptr<ProjectManager> getInstance();
    
    // Gestión de proyectos
    shared_ptr<Project> createProject(const string& name, 
                                           const string& description = "");
    void addProject(shared_ptr<Project> project);
    void removeProject(int projectId);
    shared_ptr<Project> findProjectById(int id) const;
    const vector<shared_ptr<Project>>& getAllProjects() const;
    
    // Proyecto actual
    void setCurrentProject(shared_ptr<Project> project);
    shared_ptr<Project> getCurrentProject() const;
    
    // Validaciones de dependencias circulares
    bool hasCircularDependency(int taskId, int dependencyId, 
                              shared_ptr<Board> board) const;
    bool canAddDependency(int taskId, int dependencyId, 
                         shared_ptr<Board> board) const;
    
    // Búsqueda global
    shared_ptr<Task> findTaskGlobally(int taskId) const;
    vector<shared_ptr<Task>> findTasksByUserGlobally(int userId) const;
    
    // Estadísticas globales
    int getTotalProjectCount() const;
    int getTotalTaskCountGlobal() const;
    
    // Métodos de utilidad
    void clearAllProjects();
};

#endif // PROJECT_MANAGER_H


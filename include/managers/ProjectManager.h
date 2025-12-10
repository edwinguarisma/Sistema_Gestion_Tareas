#ifndef PROJECT_MANAGER_H
#define PROJECT_MANAGER_H

#include <memory>
#include <vector>
#include <string>
#include "models/Project.h"

/**
 * @brief Gestor centralizado de proyectos (Patrón Singleton)
 * Administra todos los proyectos de la aplicación
 */
class ProjectManager {
private:
    static std::shared_ptr<ProjectManager> instance;
    std::vector<std::shared_ptr<Project>> projects;
    std::shared_ptr<Project> currentProject;
    int nextProjectId;
    
    // Constructor privado (Singleton)
    ProjectManager();

public:
    // Destructor
    ~ProjectManager();
    
    // Singleton
    static std::shared_ptr<ProjectManager> getInstance();
    
    // Gestión de proyectos
    std::shared_ptr<Project> createProject(const std::string& name, 
                                           const std::string& description = "");
    void addProject(std::shared_ptr<Project> project);
    void removeProject(int projectId);
    std::shared_ptr<Project> findProjectById(int id) const;
    const std::vector<std::shared_ptr<Project>>& getAllProjects() const;
    
    // Proyecto actual
    void setCurrentProject(std::shared_ptr<Project> project);
    std::shared_ptr<Project> getCurrentProject() const;
    
    // Validaciones de dependencias circulares
    bool hasCircularDependency(int taskId, int dependencyId, 
                              std::shared_ptr<Board> board) const;
    bool canAddDependency(int taskId, int dependencyId, 
                         std::shared_ptr<Board> board) const;
    
    // Búsqueda global
    std::shared_ptr<Task> findTaskGlobally(int taskId) const;
    std::vector<std::shared_ptr<Task>> findTasksByUserGlobally(int userId) const;
    
    // Estadísticas globales
    int getTotalProjectCount() const;
    int getTotalTaskCountGlobal() const;
    
    // Métodos de utilidad
    void clearAllProjects();
};

#endif // PROJECT_MANAGER_H


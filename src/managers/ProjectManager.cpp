#include "managers/ProjectManager.h"
#include <algorithm>
#include <functional>
#include <set>

// Inicializar el singleton
std::shared_ptr<ProjectManager> ProjectManager::instance = nullptr;

// Constructor
ProjectManager::ProjectManager() : nextProjectId(1) {}

// Destructor
ProjectManager::~ProjectManager() {}

// Singleton
std::shared_ptr<ProjectManager> ProjectManager::getInstance() {
    if (!instance) {
        instance = std::shared_ptr<ProjectManager>(new ProjectManager());
    }
    return instance;
}

// Gestión de proyectos
std::shared_ptr<Project> ProjectManager::createProject(const std::string& name,
                                                        const std::string& description) {
    auto project = std::make_shared<Project>(nextProjectId++, name, description);
    projects.push_back(project);
    
    // Si es el primer proyecto, establecerlo como actual
    if (!currentProject) {
        currentProject = project;
    }
    
    return project;
}

void ProjectManager::addProject(std::shared_ptr<Project> project) {
    if (project) {
        projects.push_back(project);
        
        if (!currentProject) {
            currentProject = project;
        }
    }
}

void ProjectManager::removeProject(int projectId) {
    projects.erase(
        std::remove_if(projects.begin(), projects.end(),
            [projectId](const std::shared_ptr<Project>& p) {
                return p->getId() == projectId;
            }),
        projects.end()
    );
    
    // Si se eliminó el proyecto actual, seleccionar otro
    if (currentProject && currentProject->getId() == projectId) {
        currentProject = projects.empty() ? nullptr : projects[0];
    }
}

std::shared_ptr<Project> ProjectManager::findProjectById(int id) const {
    for (const auto& project : projects) {
        if (project->getId() == id) {
            return project;
        }
    }
    return nullptr;
}

const std::vector<std::shared_ptr<Project>>& ProjectManager::getAllProjects() const {
    return projects;
}

// Proyecto actual
void ProjectManager::setCurrentProject(std::shared_ptr<Project> project) {
    if (project) {
        currentProject = project;
    }
}

std::shared_ptr<Project> ProjectManager::getCurrentProject() const {
    return currentProject;
}

// Validaciones de dependencias circulares
bool ProjectManager::hasCircularDependency(int taskId, int dependencyId,
                                          std::shared_ptr<Board> board) const {
    if (!board || taskId == dependencyId) {
        return true;  // Una tarea no puede depender de sí misma
    }
    
    // Realizar búsqueda en profundidad para detectar ciclos
    std::set<int> visited;
    std::set<int> recursionStack;
    
    std::function<bool(int)> detectCycle = [&](int currentId) -> bool {
        if (recursionStack.find(currentId) != recursionStack.end()) {
            return true;  // Ciclo detectado
        }
        
        if (visited.find(currentId) != visited.end()) {
            return false;  // Ya visitado, no hay ciclo
        }
        
        visited.insert(currentId);
        recursionStack.insert(currentId);
        
        auto task = board->findTaskById(currentId);
        if (task) {
            for (int depId : task->getDependencies()) {
                if (detectCycle(depId)) {
                    return true;
                }
            }
        }
        
        recursionStack.erase(currentId);
        return false;
    };
    
    // Simular la adición de la dependencia
    auto task = board->findTaskById(dependencyId);
    if (!task) {
        return false;
    }
    
    // Verificar si agregar taskId como dependencia de dependencyId crearía un ciclo
    for (int depId : task->getDependencies()) {
        if (depId == taskId) {
            return true;
        }
    }
    
    // Verificar desde dependencyId si llegaríamos a taskId
    return detectCycle(dependencyId);
}

bool ProjectManager::canAddDependency(int taskId, int dependencyId,
                                      std::shared_ptr<Board> board) const {
    if (!board) {
        return false;
    }
    
    auto task = board->findTaskById(taskId);
    auto depTask = board->findTaskById(dependencyId);
    
    if (!task || !depTask) {
        return false;  // Tareas no encontradas
    }
    
    if (taskId == dependencyId) {
        return false;  // No puede depender de sí misma
    }
    
    // Verificar dependencias circulares
    if (hasCircularDependency(taskId, dependencyId, board)) {
        return false;
    }
    
    return true;
}

// Búsqueda global
std::shared_ptr<Task> ProjectManager::findTaskGlobally(int taskId) const {
    for (const auto& project : projects) {
        auto task = project->findTaskById(taskId);
        if (task) {
            return task;
        }
    }
    return nullptr;
}

std::vector<std::shared_ptr<Task>> ProjectManager::findTasksByUserGlobally(int userId) const {
    std::vector<std::shared_ptr<Task>> result;
    
    for (const auto& project : projects) {
        auto tasks = project->findTasksByUser(userId);
        result.insert(result.end(), tasks.begin(), tasks.end());
    }
    
    return result;
}

// Estadísticas globales
int ProjectManager::getTotalProjectCount() const {
    return static_cast<int>(projects.size());
}

int ProjectManager::getTotalTaskCountGlobal() const {
    int count = 0;
    for (const auto& project : projects) {
        count += project->getTotalTaskCount();
    }
    return count;
}

// Métodos de utilidad
void ProjectManager::clearAllProjects() {
    projects.clear();
    currentProject = nullptr;
    nextProjectId = 1;
}


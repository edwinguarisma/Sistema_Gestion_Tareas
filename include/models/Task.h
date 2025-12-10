#ifndef TASK_H
#define TASK_H

#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <set>
#include "Subtask.h"
#include "TaskMemento.h"
#include "ActivityLog.h"

/**
 * @brief Clase que representa una tarea en el sistema
 * Implementa el patrón Memento para control de versiones
 */
class Task {
private:
    int id;
    std::string title;
    std::string description;
    std::string state;  // "Pendiente", "En Progreso", "Terminado"
    int assignedUserId;  // ID del usuario asignado
    std::chrono::system_clock::time_point dueDate;
    std::chrono::system_clock::time_point createdDate;
    int priority;  // 1 (baja) - 5 (alta)
    
    // Subtareas
    std::vector<std::shared_ptr<Subtask>> subtasks;
    
    // Dependencias (IDs de tareas de las que depende esta tarea)
    std::set<int> dependencies;
    
    // Control de versiones (Patrón Memento)
    std::vector<std::shared_ptr<TaskMemento>> history;
    
    // Registro de actividad
    std::shared_ptr<ActivityLog> activityLog;
    
    // Tags/Etiquetas
    std::vector<std::string> tags;

public:
    // Constructores
    Task();
    Task(int id, const std::string& title, const std::string& description = "");
    
    // Destructor
    ~Task();
    
    // Getters
    int getId() const;
    std::string getTitle() const;
    std::string getDescription() const;
    std::string getState() const;
    int getAssignedUserId() const;
    std::chrono::system_clock::time_point getDueDate() const;
    std::chrono::system_clock::time_point getCreatedDate() const;
    int getPriority() const;
    const std::vector<std::shared_ptr<Subtask>>& getSubtasks() const;
    const std::set<int>& getDependencies() const;
    const std::vector<std::string>& getTags() const;
    std::shared_ptr<ActivityLog> getActivityLog() const;
    
    // Setters
    void setTitle(const std::string& title, const std::string& modifiedBy);
    void setDescription(const std::string& description, const std::string& modifiedBy);
    void setState(const std::string& state, const std::string& modifiedBy);
    void setAssignedUserId(int userId, const std::string& modifiedBy);
    void setDueDate(const std::chrono::system_clock::time_point& date);
    void setPriority(int priority);
    
    // Gestión de subtareas
    void addSubtask(std::shared_ptr<Subtask> subtask);
    void removeSubtask(int subtaskId);
    std::shared_ptr<Subtask> findSubtaskById(int id);
    double getSubtaskCompletionPercentage() const;
    
    // Gestión de dependencias
    void addDependency(int taskId);
    void removeDependency(int taskId);
    bool hasDependency(int taskId) const;
    bool hasDependencies() const;
    
    // Gestión de tags
    void addTag(const std::string& tag);
    void removeTag(const std::string& tag);
    bool hasTag(const std::string& tag) const;
    
    // Patrón Memento - Control de versiones
    std::shared_ptr<TaskMemento> createMemento(const std::string& modifiedBy);
    void restoreFromMemento(std::shared_ptr<TaskMemento> memento);
    const std::vector<std::shared_ptr<TaskMemento>>& getHistory() const;
    
    // Validaciones
    bool canStart() const;  // Verifica si puede iniciarse (dependencias completadas)
    bool isOverdue() const;
    int getDaysUntilDue() const;
    
    // Métodos de utilidad
    std::string toString() const;
    bool operator<(const Task& other) const;  // Para ordenar por prioridad
};

#endif // TASK_H


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


using namespace std;

/**
 * @brief Clase que representa una tarea en el sistema
 * Implementa el patrón Memento para control de versiones
 */
class Task {
private:
    int id;
    string title;
    string description;
    string state;  // "Pendiente", "En Progreso", "Terminado"
    int assignedUserId;  // ID del usuario asignado
    chrono::system_clock::time_point dueDate;
    chrono::system_clock::time_point createdDate;
    int priority;  // 1 (baja) - 5 (alta)
    
    // Subtareas
    vector<shared_ptr<Subtask>> subtasks;
    
    // Dependencias (IDs de tareas de las que depende esta tarea)
    set<int> dependencies;
    
    // Control de versiones (Patrón Memento)
    vector<shared_ptr<TaskMemento>> history;
    
    // Registro de actividad
    shared_ptr<ActivityLog> activityLog;
    
    // Tags/Etiquetas
    vector<string> tags;

public:
    // Constructores
    Task();
    Task(int id, const string& title, const string& description = "");
    
    // Destructor
    ~Task();
    
    // Getters
    int getId() const;
    string getTitle() const;
    string getDescription() const;
    string getState() const;
    int getAssignedUserId() const;
    chrono::system_clock::time_point getDueDate() const;
    chrono::system_clock::time_point getCreatedDate() const;
    int getPriority() const;
    const vector<shared_ptr<Subtask>>& getSubtasks() const;
    const set<int>& getDependencies() const;
    const vector<string>& getTags() const;
    shared_ptr<ActivityLog> getActivityLog() const;
    
    // Setters
    void setTitle(const string& title, const string& modifiedBy);
    void setDescription(const string& description, const string& modifiedBy);
    void setState(const string& state, const string& modifiedBy);
    void setAssignedUserId(int userId, const string& modifiedBy);
    void setDueDate(const chrono::system_clock::time_point& date);
    void setPriority(int priority);
    
    // Gestión de subtareas
    void addSubtask(shared_ptr<Subtask> subtask);
    void removeSubtask(int subtaskId);
    shared_ptr<Subtask> findSubtaskById(int id);
    double getSubtaskCompletionPercentage() const;
    
    // Gestión de dependencias
    void addDependency(int taskId);
    void removeDependency(int taskId);
    bool hasDependency(int taskId) const;
    bool hasDependencies() const;
    
    // Gestión de tags
    void addTag(const string& tag);
    void removeTag(const string& tag);
    bool hasTag(const string& tag) const;
    
    // Patrón Memento - Control de versiones
    shared_ptr<TaskMemento> createMemento(const string& modifiedBy);
    void restoreFromMemento(shared_ptr<TaskMemento> memento);
    const vector<shared_ptr<TaskMemento>>& getHistory() const;
    
    // Validaciones
    bool canStart() const;  // Verifica si puede iniciarse (dependencias completadas)
    bool isOverdue() const;
    int getDaysUntilDue() const;
    
    // Métodos de utilidad
    string toString() const;
    bool operator<(const Task& other) const;  // Para ordenar por prioridad
};

#endif // TASK_H


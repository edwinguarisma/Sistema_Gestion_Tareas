#ifndef TASK_MEMENTO_H
#define TASK_MEMENTO_H

#include <string>
#include <chrono>
#include <memory>

/**
 * @brief Clase que implementa el patrón Memento para control de versiones
 * Guarda el estado de una tarea en un momento específico
 */
class TaskMemento {
private:
    std::string title;
    std::string description;
    std::string state;
    int assignedUserId;
    std::chrono::system_clock::time_point timestamp;
    std::string modifiedBy;  // Usuario que hizo el cambio
    
    // Constructor privado, solo Task puede crear mementos
    friend class Task;
    TaskMemento(const std::string& title, const std::string& description, 
                const std::string& state, int assignedUserId,
                const std::string& modifiedBy);

public:
    // Getters
    std::string getTitle() const;
    std::string getDescription() const;
    std::string getState() const;
    int getAssignedUserId() const;
    std::chrono::system_clock::time_point getTimestamp() const;
    std::string getModifiedBy() const;
    
    std::string toString() const;
};

#endif // TASK_MEMENTO_H


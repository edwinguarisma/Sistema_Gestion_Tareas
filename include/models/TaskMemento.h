#ifndef TASK_MEMENTO_H
#define TASK_MEMENTO_H

#include <string>
#include <chrono>
#include <memory>

using namespace std;

/**
 * @brief Clase que implementa el patrón Memento para control de versiones
 * Guarda el estado de una tarea en un momento específico
 */
class TaskMemento {
private:
    string title;
    string description;
    string state;
    int assignedUserId;
    chrono::system_clock::time_point timestamp;
    string modifiedBy;  // Usuario que hizo el cambio
    
    // Constructor privado, solo Task puede crear mementos
    friend class Task;
    TaskMemento(const string& title, const string& description, 
                const string& state, int assignedUserId,
                const string& modifiedBy);

public:
    // Getters
    string getTitle() const;
    string getDescription() const;
    string getState() const;
    int getAssignedUserId() const;
    chrono::system_clock::time_point getTimestamp() const;
    string getModifiedBy() const;
    
    string toString() const;
};

#endif // TASK_MEMENTO_H


#ifndef BOARD_H
#define BOARD_H

#include <string>
#include <vector>
#include <memory>
#include <map>
#include "Task.h"

using namespace std;

/**
 * @brief Clase que representa un tablero Kanban
 * Organiza las tareas en diferentes estados/columnas
 */
class Board {
private:
    int id;
    string name;
    string description;
    
    // Estados personalizables del tablero
    vector<string> states;
    
    // Tareas organizadas por estado
    map<string, vector<shared_ptr<Task>>> tasksByState;
    
    // Todas las tareas del tablero (para búsqueda rápida por ID)
    map<int, shared_ptr<Task>> tasksById;
    
    // Contador para IDs de tareas
    int nextTaskId;

public:
    // Constructores
    Board();
    Board(int id, const string& name, const string& description = "");
    
    // Destructor
    ~Board();
    
    // Getters
    int getId() const;
    string getName() const;
    string getDescription() const;
    const vector<string>& getStates() const;
    
    // Setters
    void setName(const string& name);
    void setDescription(const string& description);
    
    // Gestión de estados
    void addState(const string& state);
    void removeState(const string& state);
    void reorderStates(const vector<string>& newOrder);
    bool hasState(const string& state) const;
    
    // Gestión de tareas
    shared_ptr<Task> createTask(const string& title, const string& description, 
                                      const string& initialState = "Pendiente");
    void addTask(shared_ptr<Task> task, const string& state);
    void removeTask(int taskId);
    void moveTask(int taskId, const string& newState, const string& movedBy);
    
    // Búsqueda y filtrado
    shared_ptr<Task> findTaskById(int id) const;
    vector<shared_ptr<Task>> getTasksByState(const string& state) const;
    vector<shared_ptr<Task>> getTasksByUser(int userId) const;
    vector<shared_ptr<Task>> getTasksByTag(const string& tag) const;
    vector<shared_ptr<Task>> getOverdueTasks() const;
    vector<shared_ptr<Task>> getAllTasks() const;
    
    // Validaciones de dependencias
    bool canMoveTask(int taskId, const string& newState) const;
    vector<int> getBlockingTasks(int taskId) const;  // Tareas que bloquean esta tarea
    
    // Estadísticas
    int getTotalTaskCount() const;
    int getTaskCountByState(const string& state) const;
    double getCompletionPercentage() const;
    
    // Métodos de utilidad
    string toString() const;
    void clearAllTasks();
};

#endif // BOARD_H


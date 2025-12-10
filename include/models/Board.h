#ifndef BOARD_H
#define BOARD_H

#include <string>
#include <vector>
#include <memory>
#include <map>
#include "Task.h"

/**
 * @brief Clase que representa un tablero Kanban
 * Organiza las tareas en diferentes estados/columnas
 */
class Board {
private:
    int id;
    std::string name;
    std::string description;
    
    // Estados personalizables del tablero
    std::vector<std::string> states;
    
    // Tareas organizadas por estado
    std::map<std::string, std::vector<std::shared_ptr<Task>>> tasksByState;
    
    // Todas las tareas del tablero (para búsqueda rápida por ID)
    std::map<int, std::shared_ptr<Task>> tasksById;
    
    // Contador para IDs de tareas
    int nextTaskId;

public:
    // Constructores
    Board();
    Board(int id, const std::string& name, const std::string& description = "");
    
    // Destructor
    ~Board();
    
    // Getters
    int getId() const;
    std::string getName() const;
    std::string getDescription() const;
    const std::vector<std::string>& getStates() const;
    
    // Setters
    void setName(const std::string& name);
    void setDescription(const std::string& description);
    
    // Gestión de estados
    void addState(const std::string& state);
    void removeState(const std::string& state);
    void reorderStates(const std::vector<std::string>& newOrder);
    bool hasState(const std::string& state) const;
    
    // Gestión de tareas
    std::shared_ptr<Task> createTask(const std::string& title, const std::string& description, 
                                      const std::string& initialState = "Pendiente");
    void addTask(std::shared_ptr<Task> task, const std::string& state);
    void removeTask(int taskId);
    void moveTask(int taskId, const std::string& newState, const std::string& movedBy);
    
    // Búsqueda y filtrado
    std::shared_ptr<Task> findTaskById(int id) const;
    std::vector<std::shared_ptr<Task>> getTasksByState(const std::string& state) const;
    std::vector<std::shared_ptr<Task>> getTasksByUser(int userId) const;
    std::vector<std::shared_ptr<Task>> getTasksByTag(const std::string& tag) const;
    std::vector<std::shared_ptr<Task>> getOverdueTasks() const;
    std::vector<std::shared_ptr<Task>> getAllTasks() const;
    
    // Validaciones de dependencias
    bool canMoveTask(int taskId, const std::string& newState) const;
    std::vector<int> getBlockingTasks(int taskId) const;  // Tareas que bloquean esta tarea
    
    // Estadísticas
    int getTotalTaskCount() const;
    int getTaskCountByState(const std::string& state) const;
    double getCompletionPercentage() const;
    
    // Métodos de utilidad
    std::string toString() const;
    void clearAllTasks();
};

#endif // BOARD_H


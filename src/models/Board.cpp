#include "models/Board.h"
#include <sstream>
#include <algorithm>

// Constructores
Board::Board() 
    : id(-1), name(""), description(""), nextTaskId(1) {
    // Estados predeterminados
    states = {"Pendiente", "En Progreso", "Terminado"};
    
    // Inicializar mapas de tareas
    for (const auto& state : states) {
        tasksByState[state] = std::vector<std::shared_ptr<Task>>();
    }
}

Board::Board(int id, const std::string& name, const std::string& description)
    : id(id), name(name), description(description), nextTaskId(1) {
    // Estados predeterminados
    states = {"Pendiente", "En Progreso", "Terminado"};
    
    // Inicializar mapas de tareas
    for (const auto& state : states) {
        tasksByState[state] = std::vector<std::shared_ptr<Task>>();
    }
}

// Destructor
Board::~Board() {}

// Getters
int Board::getId() const {
    return id;
}

std::string Board::getName() const {
    return name;
}

std::string Board::getDescription() const {
    return description;
}

const std::vector<std::string>& Board::getStates() const {
    return states;
}

// Setters
void Board::setName(const std::string& name) {
    this->name = name;
}

void Board::setDescription(const std::string& description) {
    this->description = description;
}

// Gestión de estados
void Board::addState(const std::string& state) {
    if (!hasState(state)) {
        states.push_back(state);
        tasksByState[state] = std::vector<std::shared_ptr<Task>>();
    }
}

void Board::removeState(const std::string& state) {
    // No permitir eliminar si tiene tareas
    if (tasksByState[state].empty()) {
        states.erase(std::remove(states.begin(), states.end(), state), states.end());
        tasksByState.erase(state);
    }
}

void Board::reorderStates(const std::vector<std::string>& newOrder) {
    // Validar que todos los estados existan
    for (const auto& state : newOrder) {
        if (!hasState(state)) {
            return;  // No reordenar si falta algún estado
        }
    }
    states = newOrder;
}

bool Board::hasState(const std::string& state) const {
    return std::find(states.begin(), states.end(), state) != states.end();
}

// Gestión de tareas
std::shared_ptr<Task> Board::createTask(const std::string& title, 
                                         const std::string& description,
                                         const std::string& initialState) {
    auto task = std::make_shared<Task>(nextTaskId++, title, description);
    
    // Verificar que el estado inicial existe
    std::string state = hasState(initialState) ? initialState : states[0];
    task->setState(state, "Sistema");
    
    addTask(task, state);
    
    return task;
}

void Board::addTask(std::shared_ptr<Task> task, const std::string& state) {
    if (task && hasState(state)) {
        tasksByState[state].push_back(task);
        tasksById[task->getId()] = task;
    }
}

void Board::removeTask(int taskId) {
    auto it = tasksById.find(taskId);
    if (it != tasksById.end()) {
        auto task = it->second;
        std::string state = task->getState();
        
        // Eliminar de la lista por estado
        auto& stateTasks = tasksByState[state];
        stateTasks.erase(
            std::remove_if(stateTasks.begin(), stateTasks.end(),
                [taskId](const std::shared_ptr<Task>& t) {
                    return t->getId() == taskId;
                }),
            stateTasks.end()
        );
        
        // Eliminar del mapa global
        tasksById.erase(it);
    }
}

void Board::moveTask(int taskId, const std::string& newState, const std::string& movedBy) {
    if (!hasState(newState)) {
        return;  // Estado no válido
    }
    
    auto task = findTaskById(taskId);
    if (!task) {
        return;  // Tarea no encontrada
    }
    
    // Validar dependencias si se está moviendo a "En Progreso" o "Terminado"
    if ((newState == "En Progreso" || newState == "Terminado") && 
        !canMoveTask(taskId, newState)) {
        return;  // No se puede mover por dependencias
    }
    
    std::string oldState = task->getState();
    
    // Eliminar de la lista del estado anterior
    auto& oldStateTasks = tasksByState[oldState];
    oldStateTasks.erase(
        std::remove_if(oldStateTasks.begin(), oldStateTasks.end(),
            [taskId](const std::shared_ptr<Task>& t) {
                return t->getId() == taskId;
            }),
        oldStateTasks.end()
    );
    
    // Actualizar estado de la tarea
    task->setState(newState, movedBy);
    
    // Agregar a la lista del nuevo estado
    tasksByState[newState].push_back(task);
}

// Búsqueda y filtrado
std::shared_ptr<Task> Board::findTaskById(int id) const {
    auto it = tasksById.find(id);
    return (it != tasksById.end()) ? it->second : nullptr;
}

std::vector<std::shared_ptr<Task>> Board::getTasksByState(const std::string& state) const {
    auto it = tasksByState.find(state);
    return (it != tasksByState.end()) ? it->second : std::vector<std::shared_ptr<Task>>();
}

std::vector<std::shared_ptr<Task>> Board::getTasksByUser(int userId) const {
    std::vector<std::shared_ptr<Task>> result;
    
    for (const auto& pair : tasksById) {
        if (pair.second->getAssignedUserId() == userId) {
            result.push_back(pair.second);
        }
    }
    
    return result;
}

std::vector<std::shared_ptr<Task>> Board::getTasksByTag(const std::string& tag) const {
    std::vector<std::shared_ptr<Task>> result;
    
    for (const auto& pair : tasksById) {
        if (pair.second->hasTag(tag)) {
            result.push_back(pair.second);
        }
    }
    
    return result;
}

std::vector<std::shared_ptr<Task>> Board::getOverdueTasks() const {
    std::vector<std::shared_ptr<Task>> result;
    
    for (const auto& pair : tasksById) {
        if (pair.second->isOverdue()) {
            result.push_back(pair.second);
        }
    }
    
    return result;
}

std::vector<std::shared_ptr<Task>> Board::getAllTasks() const {
    std::vector<std::shared_ptr<Task>> result;
    
    for (const auto& pair : tasksById) {
        result.push_back(pair.second);
    }
    
    return result;
}

// Validaciones de dependencias
bool Board::canMoveTask(int taskId, const std::string& newState) const {
    auto task = findTaskById(taskId);
    if (!task) {
        return false;
    }
    
    // Si no tiene dependencias, puede moverse
    if (!task->hasDependencies()) {
        return true;
    }
    
    // Si se mueve a Pendiente, siempre puede
    if (newState == "Pendiente") {
        return true;
    }
    
    // Para otros estados, verificar que todas las dependencias estén terminadas
    for (int depId : task->getDependencies()) {
        auto depTask = findTaskById(depId);
        if (!depTask || depTask->getState() != "Terminado") {
            return false;  // Dependencia no terminada
        }
    }
    
    return true;
}

std::vector<int> Board::getBlockingTasks(int taskId) const {
    std::vector<int> blocking;
    
    auto task = findTaskById(taskId);
    if (!task) {
        return blocking;
    }
    
    for (int depId : task->getDependencies()) {
        auto depTask = findTaskById(depId);
        if (depTask && depTask->getState() != "Terminado") {
            blocking.push_back(depId);
        }
    }
    
    return blocking;
}

// Estadísticas
int Board::getTotalTaskCount() const {
    return static_cast<int>(tasksById.size());
}

int Board::getTaskCountByState(const std::string& state) const {
    auto it = tasksByState.find(state);
    return (it != tasksByState.end()) ? static_cast<int>(it->second.size()) : 0;
}

double Board::getCompletionPercentage() const {
    int total = getTotalTaskCount();
    if (total == 0) {
        return 0.0;
    }
    
    int completed = getTaskCountByState("Terminado");
    return (completed * 100.0) / total;
}

// Métodos de utilidad
std::string Board::toString() const {
    std::stringstream ss;
    ss << "Board[ID: " << id << ", Name: " << name 
       << ", Tasks: " << getTotalTaskCount() 
       << ", Completion: " << getCompletionPercentage() << "%]\n";
    
    for (const auto& state : states) {
        ss << "  " << state << ": " << getTaskCountByState(state) << " tareas\n";
    }
    
    return ss.str();
}

void Board::clearAllTasks() {
    tasksById.clear();
    for (auto& pair : tasksByState) {
        pair.second.clear();
    }
    nextTaskId = 1;
}


#include "models/Board.h"
#include <sstream>
#include <algorithm>

using namespace std;

// Constructores
Board::Board() 
    : id(-1), name(""), description(""), nextTaskId(1) {
    // Estados predeterminados
    states = {"Pendiente", "En Progreso", "Terminado"};
    
    // Inicializar mapas de tareas
    for (const auto& state : states) {
        tasksByState[state] = vector<shared_ptr<Task>>();
    }
}

Board::Board(int id, const string& name, const string& description)
    : id(id), name(name), description(description), nextTaskId(1) {
    // Estados predeterminados
    states = {"Pendiente", "En Progreso", "Terminado"};
    
    // Inicializar mapas de tareas
    for (const auto& state : states) {
        tasksByState[state] = vector<shared_ptr<Task>>();
    }
}

// Destructor
Board::~Board() {}

// Getters
int Board::getId() const {
    return id;
}

string Board::getName() const {
    return name;
}

string Board::getDescription() const {
    return description;
}

const vector<string>& Board::getStates() const {
    return states;
}

// Setters
void Board::setName(const string& name) {
    this->name = name;
}

void Board::setDescription(const string& description) {
    this->description = description;
}

// Gestión de estados
void Board::addState(const string& state) {
    if (!hasState(state)) {
        states.push_back(state);
        tasksByState[state] = vector<shared_ptr<Task>>();
    }
}

void Board::removeState(const string& state) {
    // No permitir eliminar si tiene tareas
    if (tasksByState[state].empty()) {
        states.erase(remove(states.begin(), states.end(), state), states.end());
        tasksByState.erase(state);
    }
}

void Board::reorderStates(const vector<string>& newOrder) {
    // Validar que todos los estados existan
    for (const auto& state : newOrder) {
        if (!hasState(state)) {
            return;  // No reordenar si falta algún estado
        }
    }
    states = newOrder;
}

bool Board::hasState(const string& state) const {
    return find(states.begin(), states.end(), state) != states.end();
}

// Gestión de tareas
shared_ptr<Task> Board::createTask(const string& title, 
                                         const string& description,
                                         const string& initialState) {
    auto task = make_shared<Task>(nextTaskId++, title, description);
    
    // Verificar que el estado inicial existe
    string state = hasState(initialState) ? initialState : states[0];
    task->setState(state, "Sistema");
    
    addTask(task, state);
    
    return task;
}

void Board::addTask(shared_ptr<Task> task, const string& state) {
    if (task && hasState(state)) {
        tasksByState[state].push_back(task);
        tasksById[task->getId()] = task;
    }
}

void Board::removeTask(int taskId) {
    auto it = tasksById.find(taskId);
    if (it != tasksById.end()) {
        auto task = it->second;
        string state = task->getState();
        
        // Eliminar de la lista por estado
        auto& stateTasks = tasksByState[state];
        stateTasks.erase(
            remove_if(stateTasks.begin(), stateTasks.end(),
                [taskId](const shared_ptr<Task>& t) {
                    return t->getId() == taskId;
                }),
            stateTasks.end()
        );
        
        // Eliminar del mapa global
        tasksById.erase(it);
    }
}

void Board::moveTask(int taskId, const string& newState, const string& movedBy) {
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
    
    string oldState = task->getState();
    
    // Eliminar de la lista del estado anterior
    auto& oldStateTasks = tasksByState[oldState];
    oldStateTasks.erase(
        remove_if(oldStateTasks.begin(), oldStateTasks.end(),
            [taskId](const shared_ptr<Task>& t) {
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
shared_ptr<Task> Board::findTaskById(int id) const {
    auto it = tasksById.find(id);
    return (it != tasksById.end()) ? it->second : nullptr;
}

vector<shared_ptr<Task>> Board::getTasksByState(const string& state) const {
    auto it = tasksByState.find(state);
    return (it != tasksByState.end()) ? it->second : vector<shared_ptr<Task>>();
}

vector<shared_ptr<Task>> Board::getTasksByUser(int userId) const {
    vector<shared_ptr<Task>> result;
    
    for (const auto& pair : tasksById) {
        if (pair.second->getAssignedUserId() == userId) {
            result.push_back(pair.second);
        }
    }
    
    return result;
}

vector<shared_ptr<Task>> Board::getTasksByTag(const string& tag) const {
    vector<shared_ptr<Task>> result;
    
    for (const auto& pair : tasksById) {
        if (pair.second->hasTag(tag)) {
            result.push_back(pair.second);
        }
    }
    
    return result;
}

vector<shared_ptr<Task>> Board::getOverdueTasks() const {
    vector<shared_ptr<Task>> result;
    
    for (const auto& pair : tasksById) {
        if (pair.second->isOverdue()) {
            result.push_back(pair.second);
        }
    }
    
    return result;
}

vector<shared_ptr<Task>> Board::getAllTasks() const {
    vector<shared_ptr<Task>> result;
    
    for (const auto& pair : tasksById) {
        result.push_back(pair.second);
    }
    
    return result;
}

// Validaciones de dependencias
bool Board::canMoveTask(int taskId, const string& newState) const {
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

vector<int> Board::getBlockingTasks(int taskId) const {
    vector<int> blocking;
    
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

int Board::getTaskCountByState(const string& state) const {
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
string Board::toString() const {
    stringstream ss;
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


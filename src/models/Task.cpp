#include "models/Task.h"
#include <sstream>
#include <algorithm>
#include <ctime>

// Constructores
Task::Task() 
    : id(-1), title(""), description(""), state("Pendiente"),
      assignedUserId(-1), priority(3),
      createdDate(std::chrono::system_clock::now()),
      activityLog(std::make_shared<ActivityLog>()) {}

Task::Task(int id, const std::string& title, const std::string& description)
    : id(id), title(title), description(description), state("Pendiente"),
      assignedUserId(-1), priority(3),
      createdDate(std::chrono::system_clock::now()),
      activityLog(std::make_shared<ActivityLog>()) {}

// Destructor
Task::~Task() {}

// Getters
int Task::getId() const {
    return id;
}

std::string Task::getTitle() const {
    return title;
}

std::string Task::getDescription() const {
    return description;
}

std::string Task::getState() const {
    return state;
}

int Task::getAssignedUserId() const {
    return assignedUserId;
}

std::chrono::system_clock::time_point Task::getDueDate() const {
    return dueDate;
}

std::chrono::system_clock::time_point Task::getCreatedDate() const {
    return createdDate;
}

int Task::getPriority() const {
    return priority;
}

const std::vector<std::shared_ptr<Subtask>>& Task::getSubtasks() const {
    return subtasks;
}

const std::set<int>& Task::getDependencies() const {
    return dependencies;
}

const std::vector<std::string>& Task::getTags() const {
    return tags;
}

std::shared_ptr<ActivityLog> Task::getActivityLog() const {
    return activityLog;
}

// Setters
void Task::setTitle(const std::string& newTitle, const std::string& modifiedBy) {
    if (newTitle != this->title) {
        std::string oldTitle = this->title;
        this->title = newTitle;
        
        // Registrar cambio
        activityLog->logUpdate(modifiedBy, "título", oldTitle, newTitle);
        
        // Crear memento
        createMemento(modifiedBy);
    }
}

void Task::setDescription(const std::string& newDescription, const std::string& modifiedBy) {
    if (newDescription != this->description) {
        std::string oldDescription = this->description;
        this->description = newDescription;
        
        // Registrar cambio
        activityLog->logUpdate(modifiedBy, "descripción", 
                              oldDescription.substr(0, 30), 
                              newDescription.substr(0, 30));
        
        // Crear memento
        createMemento(modifiedBy);
    }
}

void Task::setState(const std::string& newState, const std::string& modifiedBy) {
    if (newState != this->state) {
        std::string oldState = this->state;
        this->state = newState;
        
        // Registrar movimiento
        activityLog->logMove(modifiedBy, oldState, newState);
        
        // Crear memento
        createMemento(modifiedBy);
    }
}

void Task::setAssignedUserId(int userId, const std::string& modifiedBy) {
    if (userId != this->assignedUserId) {
        this->assignedUserId = userId;
        
        // Registrar asignación
        activityLog->logAssignment(modifiedBy, "Usuario ID: " + std::to_string(userId));
    }
}

void Task::setDueDate(const std::chrono::system_clock::time_point& date) {
    this->dueDate = date;
}

void Task::setPriority(int priority) {
    if (priority >= 1 && priority <= 5) {
        this->priority = priority;
    }
}

// Gestión de subtareas
void Task::addSubtask(std::shared_ptr<Subtask> subtask) {
    if (subtask) {
        subtasks.push_back(subtask);
    }
}

void Task::removeSubtask(int subtaskId) {
    subtasks.erase(
        std::remove_if(subtasks.begin(), subtasks.end(),
            [subtaskId](const std::shared_ptr<Subtask>& st) {
                return st->getId() == subtaskId;
            }),
        subtasks.end()
    );
}

std::shared_ptr<Subtask> Task::findSubtaskById(int id) {
    for (auto& subtask : subtasks) {
        auto found = subtask->findSubtaskById(id);
        if (found) {
            return found;
        }
    }
    return nullptr;
}

double Task::getSubtaskCompletionPercentage() const {
    if (subtasks.empty()) {
        return 0.0;
    }
    
    int total = 0;
    int completed = 0;
    
    for (const auto& subtask : subtasks) {
        total += subtask->countTotalSubtasks();
        completed += subtask->countCompletedSubtasks();
    }
    
    return total > 0 ? (completed * 100.0) / total : 0.0;
}

// Gestión de dependencias
void Task::addDependency(int taskId) {
    dependencies.insert(taskId);
}

void Task::removeDependency(int taskId) {
    dependencies.erase(taskId);
}

bool Task::hasDependency(int taskId) const {
    return dependencies.find(taskId) != dependencies.end();
}

bool Task::hasDependencies() const {
    return !dependencies.empty();
}

// Gestión de tags
void Task::addTag(const std::string& tag) {
    if (std::find(tags.begin(), tags.end(), tag) == tags.end()) {
        tags.push_back(tag);
    }
}

void Task::removeTag(const std::string& tag) {
    tags.erase(std::remove(tags.begin(), tags.end(), tag), tags.end());
}

bool Task::hasTag(const std::string& tag) const {
    return std::find(tags.begin(), tags.end(), tag) != tags.end();
}

// Patrón Memento - Control de versiones
std::shared_ptr<TaskMemento> Task::createMemento(const std::string& modifiedBy) {
    // Usar constructor directo en lugar de make_shared porque el constructor es privado
    std::shared_ptr<TaskMemento> memento(new TaskMemento(title, description, state, 
                                                          assignedUserId, modifiedBy));
    history.push_back(memento);
    
    // Limitar historial a 50 versiones
    if (history.size() > 50) {
        history.erase(history.begin());
    }
    
    return memento;
}

void Task::restoreFromMemento(std::shared_ptr<TaskMemento> memento) {
    if (memento) {
        this->title = memento->getTitle();
        this->description = memento->getDescription();
        this->state = memento->getState();
        this->assignedUserId = memento->getAssignedUserId();
        
        activityLog->addEntry(ActivityEntry("Sistema", "restored", "", "", "",
                                           "Restauró versión anterior"));
    }
}

const std::vector<std::shared_ptr<TaskMemento>>& Task::getHistory() const {
    return history;
}

// Validaciones
bool Task::canStart() const {
    // Puede iniciarse si no tiene dependencias o todas están terminadas
    // Esta validación se debe hacer en el Board con acceso a todas las tareas
    return dependencies.empty();
}

bool Task::isOverdue() const {
    auto now = std::chrono::system_clock::now();
    return dueDate < now && state != "Terminado";
}

int Task::getDaysUntilDue() const {
    auto now = std::chrono::system_clock::now();
    auto diff = std::chrono::duration_cast<std::chrono::hours>(dueDate - now);
    return diff.count() / 24;
}

// Métodos de utilidad
std::string Task::toString() const {
    std::stringstream ss;
    ss << "Task[ID: " << id << ", Title: " << title 
       << ", State: " << state << ", Priority: " << priority;
    
    if (assignedUserId >= 0) {
        ss << ", Assigned to: " << assignedUserId;
    }
    
    if (!subtasks.empty()) {
        ss << ", Subtasks: " << subtasks.size();
    }
    
    if (!dependencies.empty()) {
        ss << ", Dependencies: " << dependencies.size();
    }
    
    ss << "]";
    return ss.str();
}

bool Task::operator<(const Task& other) const {
    // Ordenar por prioridad (mayor prioridad primero)
    return priority > other.priority;
}


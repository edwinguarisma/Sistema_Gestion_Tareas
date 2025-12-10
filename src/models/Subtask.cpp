#include "models/Subtask.h"
#include <sstream>
#include <algorithm>

// Constructores
Subtask::Subtask() : id(-1), title(""), description(""), completed(false) {}

Subtask::Subtask(int id, const std::string& title, const std::string& description)
    : id(id), title(title), description(description), completed(false) {}

// Destructor
Subtask::~Subtask() {}

// Getters
int Subtask::getId() const {
    return id;
}

std::string Subtask::getTitle() const {
    return title;
}

std::string Subtask::getDescription() const {
    return description;
}

bool Subtask::isCompleted() const {
    return completed;
}

const std::vector<std::shared_ptr<Subtask>>& Subtask::getChildSubtasks() const {
    return childSubtasks;
}

// Setters
void Subtask::setTitle(const std::string& title) {
    this->title = title;
}

void Subtask::setDescription(const std::string& description) {
    this->description = description;
}

void Subtask::setCompleted(bool completed) {
    this->completed = completed;
}

// Gestión de subtareas anidadas
void Subtask::addChildSubtask(std::shared_ptr<Subtask> subtask) {
    if (subtask) {
        childSubtasks.push_back(subtask);
    }
}

void Subtask::removeChildSubtask(int subtaskId) {
    childSubtasks.erase(
        std::remove_if(childSubtasks.begin(), childSubtasks.end(),
            [subtaskId](const std::shared_ptr<Subtask>& st) {
                return st->getId() == subtaskId;
            }),
        childSubtasks.end()
    );
}

std::shared_ptr<Subtask> Subtask::findSubtaskById(int id) {
    if (this->id == id) {
        // Intentar obtener shared_ptr si existe, sino retornar nullptr
        // porque esta función se llama desde objetos que ya son shared_ptr
        try {
            return shared_from_this();
        } catch (...) {
            return nullptr;
        }
    }
    
    for (auto& child : childSubtasks) {
        auto found = child->findSubtaskById(id);
        if (found) {
            return found;
        }
    }
    
    return nullptr;
}

// Métodos de utilidad
int Subtask::countTotalSubtasks() const {
    int count = 1;  // Contar esta subtarea
    for (const auto& child : childSubtasks) {
        count += child->countTotalSubtasks();
    }
    return count;
}

int Subtask::countCompletedSubtasks() const {
    int count = completed ? 1 : 0;
    for (const auto& child : childSubtasks) {
        count += child->countCompletedSubtasks();
    }
    return count;
}

double Subtask::getCompletionPercentage() const {
    int total = countTotalSubtasks();
    if (total == 0) return 0.0;
    return (countCompletedSubtasks() * 100.0) / total;
}

std::string Subtask::toString(int depth) const {
    std::stringstream ss;
    std::string indent(depth * 2, ' ');
    
    ss << indent << "[" << (completed ? "X" : " ") << "] " 
       << title;
    
    if (!description.empty()) {
        ss << " - " << description;
    }
    
    if (!childSubtasks.empty()) {
        ss << " (" << countCompletedSubtasks() << "/" 
           << countTotalSubtasks() << " completadas)";
    }
    
    for (const auto& child : childSubtasks) {
        ss << "\n" << child->toString(depth + 1);
    }
    
    return ss.str();
}


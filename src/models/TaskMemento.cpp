#include "models/TaskMemento.h"
#include <sstream>
#include <iomanip>
#include <ctime>

// Constructor
TaskMemento::TaskMemento(const std::string& title, const std::string& description,
                         const std::string& state, int assignedUserId,
                         const std::string& modifiedBy)
    : title(title), description(description), state(state), 
      assignedUserId(assignedUserId), modifiedBy(modifiedBy),
      timestamp(std::chrono::system_clock::now()) {}

// Getters
std::string TaskMemento::getTitle() const {
    return title;
}

std::string TaskMemento::getDescription() const {
    return description;
}

std::string TaskMemento::getState() const {
    return state;
}

int TaskMemento::getAssignedUserId() const {
    return assignedUserId;
}

std::chrono::system_clock::time_point TaskMemento::getTimestamp() const {
    return timestamp;
}

std::string TaskMemento::getModifiedBy() const {
    return modifiedBy;
}

std::string TaskMemento::toString() const {
    std::stringstream ss;
    
    // Convertir timestamp a string
    std::time_t time = std::chrono::system_clock::to_time_t(timestamp);
    ss << "Versión del " << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    ss << " por " << modifiedBy << "\n";
    ss << "Estado: " << state << "\n";
    ss << "Título: " << title << "\n";
    ss << "Descripción: " << description;
    
    return ss.str();
}


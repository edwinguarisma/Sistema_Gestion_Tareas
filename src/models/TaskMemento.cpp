#include "models/TaskMemento.h"
#include <sstream>
#include <iomanip>
#include <ctime>

using namespace std;

// Constructor
TaskMemento::TaskMemento(const string& title, const string& description,
                         const string& state, int assignedUserId,
                         const string& modifiedBy)
    : title(title), description(description), state(state), 
      assignedUserId(assignedUserId), modifiedBy(modifiedBy),
      timestamp(chrono::system_clock::now()) {}

// Getters
string TaskMemento::getTitle() const {
    return title;
}

string TaskMemento::getDescription() const {
    return description;
}

string TaskMemento::getState() const {
    return state;
}

int TaskMemento::getAssignedUserId() const {
    return assignedUserId;
}

chrono::system_clock::time_point TaskMemento::getTimestamp() const {
    return timestamp;
}

string TaskMemento::getModifiedBy() const {
    return modifiedBy;
}

string TaskMemento::toString() const {
    stringstream ss;
    
    // Convertir timestamp a string
    time_t time = chrono::system_clock::to_time_t(timestamp);
    ss << "Versión del " << put_time(localtime(&time), "%Y-%m-%d %H:%M:%S");
    ss << " por " << modifiedBy << "\n";
    ss << "Estado: " << state << "\n";
    ss << "Título: " << title << "\n";
    ss << "Descripción: " << description;
    
    return ss.str();
}


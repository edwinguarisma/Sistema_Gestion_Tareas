#include "models/ActivityLog.h"
#include <sstream>
#include <iomanip>
#include <ctime>
#include <algorithm>

using namespace std;

// ActivityEntry
ActivityEntry::ActivityEntry(const string& userName, const string& actionType,
                             const string& fieldModified,
                             const string& oldValue,
                             const string& newValue,
                             const string& description)
    : userName(userName), actionType(actionType), fieldModified(fieldModified),
      oldValue(oldValue), newValue(newValue), description(description),
      timestamp(chrono::system_clock::now()) {}

string ActivityEntry::toString() const {
    stringstream ss;
    
    // Convertir timestamp a string
    time_t time = chrono::system_clock::to_time_t(timestamp);
    ss << "[" << put_time(localtime(&time), "%Y-%m-%d %H:%M:%S") << "] ";
    ss << userName << " - " << actionType;
    
    if (!fieldModified.empty()) {
        ss << " (" << fieldModified;
        if (!oldValue.empty() || !newValue.empty()) {
            ss << ": ";
            if (!oldValue.empty()) {
                ss << "'" << oldValue << "'";
            }
            if (!newValue.empty()) {
                ss << " → '" << newValue << "'";
            }
        }
        ss << ")";
    }
    
    if (!description.empty()) {
        ss << " - " << description;
    }
    
    return ss.str();
}

// ActivityLog

// Constructor
ActivityLog::ActivityLog(int maxEntries) : maxEntries(maxEntries) {}

// Destructor
ActivityLog::~ActivityLog() {}

// Agregar entradas
void ActivityLog::addEntry(const ActivityEntry& entry) {
    entries.push_back(entry);
    
    // Mantener el límite de entradas
    if (entries.size() > static_cast<size_t>(maxEntries)) {
        entries.erase(entries.begin());
    }
}

void ActivityLog::logCreation(const string& userName, const string& objectName) {
    ActivityEntry entry(userName, "created", "", "", "", 
                        "Creó " + objectName);
    addEntry(entry);
}

void ActivityLog::logMove(const string& userName, const string& fromState, 
                         const string& toState) {
    ActivityEntry entry(userName, "moved", "estado", fromState, toState,
                        "Movió de " + fromState + " a " + toState);
    addEntry(entry);
}

void ActivityLog::logUpdate(const string& userName, const string& field,
                           const string& oldValue, const string& newValue) {
    ActivityEntry entry(userName, "updated", field, oldValue, newValue,
                        "Actualizó " + field);
    addEntry(entry);
}

void ActivityLog::logAssignment(const string& userName, const string& assignedTo) {
    ActivityEntry entry(userName, "assigned", "asignado a", "", assignedTo,
                        "Asignó a " + assignedTo);
    addEntry(entry);
}

void ActivityLog::logDeletion(const string& userName) {
    ActivityEntry entry(userName, "deleted", "", "", "",
                        "Eliminó la tarea");
    addEntry(entry);
}

// Consultas
const vector<ActivityEntry>& ActivityLog::getEntries() const {
    return entries;
}

vector<ActivityEntry> ActivityLog::getEntriesByUser(const string& userName) const {
    vector<ActivityEntry> result;
    copy_if(entries.begin(), entries.end(), back_inserter(result),
        [&userName](const ActivityEntry& entry) {
            return entry.userName == userName;
        });
    return result;
}

vector<ActivityEntry> ActivityLog::getEntriesByActionType(const string& actionType) const {
    vector<ActivityEntry> result;
    copy_if(entries.begin(), entries.end(), back_inserter(result),
        [&actionType](const ActivityEntry& entry) {
            return entry.actionType == actionType;
        });
    return result;
}

vector<ActivityEntry> ActivityLog::getEntriesByDateRange(
    const chrono::system_clock::time_point& start,
    const chrono::system_clock::time_point& end) const {
    vector<ActivityEntry> result;
    copy_if(entries.begin(), entries.end(), back_inserter(result),
        [&start, &end](const ActivityEntry& entry) {
            return entry.timestamp >= start && entry.timestamp <= end;
        });
    return result;
}

// Métodos de utilidad
void ActivityLog::clear() {
    entries.clear();
}

size_t ActivityLog::getSize() const {
    return entries.size();
}

string ActivityLog::toString() const {
    stringstream ss;
    ss << "Activity Log (" << entries.size() << " entradas):\n";
    ss << "----------------------------------------\n";
    
    for (const auto& entry : entries) {
        ss << entry.toString() << "\n";
    }
    
    return ss.str();
}


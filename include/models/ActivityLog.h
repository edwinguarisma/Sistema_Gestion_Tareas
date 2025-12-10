#ifndef ACTIVITY_LOG_H
#define ACTIVITY_LOG_H

#include <string>
#include <vector>
#include <chrono>
#include <memory>

using namespace std;

/**
 * @brief Estructura que representa una entrada en el registro de actividad
 */
struct ActivityEntry {
    chrono::system_clock::time_point timestamp;
    string userName;       // Quién hizo el cambio
    string actionType;     // "created", "moved", "updated", "deleted", "assigned"
    string fieldModified;  // Campo que se modificó
    string oldValue;       // Valor anterior
    string newValue;       // Valor nuevo
    string description;    // Descripción detallada del cambio
    
    ActivityEntry(const string& userName, const string& actionType,
                  const string& fieldModified = "", 
                  const string& oldValue = "",
                  const string& newValue = "",
                  const string& description = "");
    
    string toString() const;
};

/**
 * @brief Clase que mantiene un registro detallado de todas las actividades
 * Implementa un historial completo de cambios
 */
class ActivityLog {
private:
    vector<ActivityEntry> entries;
    int maxEntries;  // Límite de entradas para evitar crecimiento indefinido

public:
    // Constructor
    ActivityLog(int maxEntries = 1000);
    
    // Destructor
    ~ActivityLog();
    
    // Agregar entradas
    void addEntry(const ActivityEntry& entry);
    void logCreation(const string& userName, const string& objectName);
    void logMove(const string& userName, const string& fromState, const string& toState);
    void logUpdate(const string& userName, const string& field, 
                   const string& oldValue, const string& newValue);
    void logAssignment(const string& userName, const string& assignedTo);
    void logDeletion(const string& userName);
    
    // Consultas
    const vector<ActivityEntry>& getEntries() const;
    vector<ActivityEntry> getEntriesByUser(const string& userName) const;
    vector<ActivityEntry> getEntriesByActionType(const string& actionType) const;
    vector<ActivityEntry> getEntriesByDateRange(
        const chrono::system_clock::time_point& start,
        const chrono::system_clock::time_point& end) const;
    
    // Métodos de utilidad
    void clear();
    size_t getSize() const;
    string toString() const;
};

#endif // ACTIVITY_LOG_H


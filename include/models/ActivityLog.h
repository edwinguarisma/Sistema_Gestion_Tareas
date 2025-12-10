#ifndef ACTIVITY_LOG_H
#define ACTIVITY_LOG_H

#include <string>
#include <vector>
#include <chrono>
#include <memory>

/**
 * @brief Estructura que representa una entrada en el registro de actividad
 */
struct ActivityEntry {
    std::chrono::system_clock::time_point timestamp;
    std::string userName;       // Quién hizo el cambio
    std::string actionType;     // "created", "moved", "updated", "deleted", "assigned"
    std::string fieldModified;  // Campo que se modificó
    std::string oldValue;       // Valor anterior
    std::string newValue;       // Valor nuevo
    std::string description;    // Descripción detallada del cambio
    
    ActivityEntry(const std::string& userName, const std::string& actionType,
                  const std::string& fieldModified = "", 
                  const std::string& oldValue = "",
                  const std::string& newValue = "",
                  const std::string& description = "");
    
    std::string toString() const;
};

/**
 * @brief Clase que mantiene un registro detallado de todas las actividades
 * Implementa un historial completo de cambios
 */
class ActivityLog {
private:
    std::vector<ActivityEntry> entries;
    int maxEntries;  // Límite de entradas para evitar crecimiento indefinido

public:
    // Constructor
    ActivityLog(int maxEntries = 1000);
    
    // Destructor
    ~ActivityLog();
    
    // Agregar entradas
    void addEntry(const ActivityEntry& entry);
    void logCreation(const std::string& userName, const std::string& objectName);
    void logMove(const std::string& userName, const std::string& fromState, const std::string& toState);
    void logUpdate(const std::string& userName, const std::string& field, 
                   const std::string& oldValue, const std::string& newValue);
    void logAssignment(const std::string& userName, const std::string& assignedTo);
    void logDeletion(const std::string& userName);
    
    // Consultas
    const std::vector<ActivityEntry>& getEntries() const;
    std::vector<ActivityEntry> getEntriesByUser(const std::string& userName) const;
    std::vector<ActivityEntry> getEntriesByActionType(const std::string& actionType) const;
    std::vector<ActivityEntry> getEntriesByDateRange(
        const std::chrono::system_clock::time_point& start,
        const std::chrono::system_clock::time_point& end) const;
    
    // Métodos de utilidad
    void clear();
    size_t getSize() const;
    std::string toString() const;
};

#endif // ACTIVITY_LOG_H


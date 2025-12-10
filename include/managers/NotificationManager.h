#ifndef NOTIFICATION_MANAGER_H
#define NOTIFICATION_MANAGER_H

#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <functional>
#include "models/Task.h"
#include "models/Project.h"

/**
 * @brief Estructura que representa una notificación
 */
struct Notification {
    enum class Type {
        TASK_DUE_SOON,        // Tarea próxima a vencer
        TASK_OVERDUE,         // Tarea vencida
        DEPENDENCY_RESOLVED,  // Dependencia resuelta
        TASK_ASSIGNED,        // Tarea asignada
        TASK_MOVED,          // Tarea movida
        TASK_COMPLETED       // Tarea completada
    };
    
    Type type;
    std::string title;
    std::string message;
    int taskId;
    int userId;  // Usuario al que va dirigida
    std::chrono::system_clock::time_point timestamp;
    bool read;
    
    Notification(Type type, const std::string& title, const std::string& message,
                int taskId = -1, int userId = -1);
    
    std::string getTypeString() const;
    std::string toString() const;
};

/**
 * @brief Gestor de notificaciones del sistema
 * Implementa el patrón Observer para notificar cambios
 */
class NotificationManager {
private:
    std::vector<Notification> notifications;
    std::function<void(const Notification&)> notificationCallback;
    
    // Para verificar tareas próximas a vencer
    std::chrono::system_clock::time_point lastCheck;

public:
    // Constructor
    NotificationManager();
    
    // Destructor
    ~NotificationManager();
    
    // Configurar callback para notificaciones
    void setNotificationCallback(std::function<void(const Notification&)> callback);
    
    // Agregar notificaciones
    void addNotification(const Notification& notification);
    void notifyTaskDueSoon(std::shared_ptr<Task> task, int userId);
    void notifyTaskOverdue(std::shared_ptr<Task> task, int userId);
    void notifyDependencyResolved(std::shared_ptr<Task> task, int userId);
    void notifyTaskAssigned(std::shared_ptr<Task> task, int userId);
    void notifyTaskMoved(std::shared_ptr<Task> task, const std::string& newState, int userId);
    void notifyTaskCompleted(std::shared_ptr<Task> task, int userId);
    
    // Verificar tareas próximas a vencer en un proyecto
    void checkDueDates(std::shared_ptr<Project> project);
    
    // Consultas
    std::vector<Notification> getNotificationsByUser(int userId) const;
    std::vector<Notification> getUnreadNotifications(int userId) const;
    int getUnreadCount(int userId) const;
    
    // Marcar como leída
    void markAsRead(size_t index);
    void markAllAsRead(int userId);
    
    // Limpiar notificaciones
    void clearOldNotifications(int daysOld = 7);
    void clearAll();
    
    // Métodos de utilidad
    size_t getNotificationCount() const;
    const std::vector<Notification>& getAllNotifications() const;
};

#endif // NOTIFICATION_MANAGER_H


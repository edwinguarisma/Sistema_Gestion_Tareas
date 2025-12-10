#ifndef NOTIFICATION_MANAGER_H
#define NOTIFICATION_MANAGER_H

#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <functional>
#include "models/Task.h"
#include "models/Project.h"

using namespace std;

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
    string title;
    string message;
    int taskId;
    int userId;  // Usuario al que va dirigida
    chrono::system_clock::time_point timestamp;
    bool read;
    
    Notification(Type type, const string& title, const string& message,
                int taskId = -1, int userId = -1);
    
    string getTypeString() const;
    string toString() const;
};

/**
 * @brief Gestor de notificaciones del sistema
 * Implementa el patrón Observer para notificar cambios
 */
class NotificationManager {
private:
    vector<Notification> notifications;
    function<void(const Notification&)> notificationCallback;
    
    // Para verificar tareas próximas a vencer
    chrono::system_clock::time_point lastCheck;

public:
    // Constructor
    NotificationManager();
    
    // Destructor
    ~NotificationManager();
    
    // Configurar callback para notificaciones
    void setNotificationCallback(function<void(const Notification&)> callback);
    
    // Agregar notificaciones
    void addNotification(const Notification& notification);
    void notifyTaskDueSoon(shared_ptr<Task> task, int userId);
    void notifyTaskOverdue(shared_ptr<Task> task, int userId);
    void notifyDependencyResolved(shared_ptr<Task> task, int userId);
    void notifyTaskAssigned(shared_ptr<Task> task, int userId);
    void notifyTaskMoved(shared_ptr<Task> task, const string& newState, int userId);
    void notifyTaskCompleted(shared_ptr<Task> task, int userId);
    
    // Verificar tareas próximas a vencer en un proyecto
    void checkDueDates(shared_ptr<Project> project);
    
    // Consultas
    vector<Notification> getNotificationsByUser(int userId) const;
    vector<Notification> getUnreadNotifications(int userId) const;
    int getUnreadCount(int userId) const;
    
    // Marcar como leída
    void markAsRead(size_t index);
    void markAllAsRead(int userId);
    
    // Limpiar notificaciones
    void clearOldNotifications(int daysOld = 7);
    void clearAll();
    
    // Métodos de utilidad
    size_t getNotificationCount() const;
    const vector<Notification>& getAllNotifications() const;
};

#endif // NOTIFICATION_MANAGER_H


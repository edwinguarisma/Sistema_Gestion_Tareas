#include "managers/NotificationManager.h"
#include <sstream>
#include <iomanip>
#include <ctime>
#include <algorithm>

// Notification

Notification::Notification(Type type, const std::string& title, 
                          const std::string& message, int taskId, int userId)
    : type(type), title(title), message(message), taskId(taskId), 
      userId(userId), timestamp(std::chrono::system_clock::now()), read(false) {}

std::string Notification::getTypeString() const {
    switch (type) {
        case Type::TASK_DUE_SOON: return "Tarea Próxima a Vencer";
        case Type::TASK_OVERDUE: return "Tarea Vencida";
        case Type::DEPENDENCY_RESOLVED: return "Dependencia Resuelta";
        case Type::TASK_ASSIGNED: return "Tarea Asignada";
        case Type::TASK_MOVED: return "Tarea Movida";
        case Type::TASK_COMPLETED: return "Tarea Completada";
        default: return "Notificación";
    }
}

std::string Notification::toString() const {
    std::stringstream ss;
    
    std::time_t time = std::chrono::system_clock::to_time_t(timestamp);
    ss << "[" << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M") << "] ";
    ss << (read ? "  " : "• ");
    ss << getTypeString() << ": " << title << "\n";
    ss << "  " << message;
    
    return ss.str();
}

// NotificationManager

// Constructor
NotificationManager::NotificationManager() 
    : lastCheck(std::chrono::system_clock::now()) {}

// Destructor
NotificationManager::~NotificationManager() {}

// Configurar callback
void NotificationManager::setNotificationCallback(
    std::function<void(const Notification&)> callback) {
    notificationCallback = callback;
}

// Agregar notificaciones
void NotificationManager::addNotification(const Notification& notification) {
    notifications.push_back(notification);
    
    // Llamar callback si está configurado
    if (notificationCallback) {
        notificationCallback(notification);
    }
}

void NotificationManager::notifyTaskDueSoon(std::shared_ptr<Task> task, int userId) {
    if (!task) return;
    
    int daysLeft = task->getDaysUntilDue();
    std::string message = "La tarea '" + task->getTitle() + 
                         "' vence en " + std::to_string(daysLeft) + " día(s)";
    
    Notification notification(Notification::Type::TASK_DUE_SOON,
                             "Tarea próxima a vencer",
                             message, task->getId(), userId);
    addNotification(notification);
}

void NotificationManager::notifyTaskOverdue(std::shared_ptr<Task> task, int userId) {
    if (!task) return;
    
    std::string message = "La tarea '" + task->getTitle() + "' está vencida";
    
    Notification notification(Notification::Type::TASK_OVERDUE,
                             "Tarea vencida",
                             message, task->getId(), userId);
    addNotification(notification);
}

void NotificationManager::notifyDependencyResolved(std::shared_ptr<Task> task, int userId) {
    if (!task) return;
    
    std::string message = "Las dependencias de la tarea '" + task->getTitle() + 
                         "' han sido resueltas. Puedes comenzar a trabajar en ella.";
    
    Notification notification(Notification::Type::DEPENDENCY_RESOLVED,
                             "Dependencia resuelta",
                             message, task->getId(), userId);
    addNotification(notification);
}

void NotificationManager::notifyTaskAssigned(std::shared_ptr<Task> task, int userId) {
    if (!task) return;
    
    std::string message = "Se te ha asignado la tarea: '" + task->getTitle() + "'";
    
    Notification notification(Notification::Type::TASK_ASSIGNED,
                             "Nueva tarea asignada",
                             message, task->getId(), userId);
    addNotification(notification);
}

void NotificationManager::notifyTaskMoved(std::shared_ptr<Task> task, 
                                         const std::string& newState, int userId) {
    if (!task) return;
    
    std::string message = "La tarea '" + task->getTitle() + 
                         "' se movió a: " + newState;
    
    Notification notification(Notification::Type::TASK_MOVED,
                             "Tarea movida",
                             message, task->getId(), userId);
    addNotification(notification);
}

void NotificationManager::notifyTaskCompleted(std::shared_ptr<Task> task, int userId) {
    if (!task) return;
    
    std::string message = "La tarea '" + task->getTitle() + "' se ha completado";
    
    Notification notification(Notification::Type::TASK_COMPLETED,
                             "Tarea completada",
                             message, task->getId(), userId);
    addNotification(notification);
}

// Verificar tareas próximas a vencer
void NotificationManager::checkDueDates(std::shared_ptr<Project> project) {
    if (!project) return;
    
    // Solo verificar una vez al día
    auto now = std::chrono::system_clock::now();
    auto hoursSinceLastCheck = std::chrono::duration_cast<std::chrono::hours>(
        now - lastCheck).count();
    
    if (hoursSinceLastCheck < 24) {
        return;  // Ya se verificó recientemente
    }
    
    lastCheck = now;
    
    // Verificar todas las tareas del proyecto
    for (const auto& board : project->getBoards()) {
        auto tasks = board->getAllTasks();
        
        for (const auto& task : tasks) {
            if (task->getState() == "Terminado") {
                continue;  // Ignorar tareas terminadas
            }
            
            int daysUntilDue = task->getDaysUntilDue();
            
            // Notificar si vence en 1 día o menos
            if (daysUntilDue <= 1 && daysUntilDue >= 0) {
                notifyTaskDueSoon(task, task->getAssignedUserId());
            }
            
            // Notificar si está vencida
            if (task->isOverdue()) {
                notifyTaskOverdue(task, task->getAssignedUserId());
            }
        }
    }
}

// Consultas
std::vector<Notification> NotificationManager::getNotificationsByUser(int userId) const {
    std::vector<Notification> result;
    
    std::copy_if(notifications.begin(), notifications.end(), 
                std::back_inserter(result),
        [userId](const Notification& n) {
            return n.userId == userId;
        });
    
    return result;
}

std::vector<Notification> NotificationManager::getUnreadNotifications(int userId) const {
    std::vector<Notification> result;
    
    std::copy_if(notifications.begin(), notifications.end(),
                std::back_inserter(result),
        [userId](const Notification& n) {
            return n.userId == userId && !n.read;
        });
    
    return result;
}

int NotificationManager::getUnreadCount(int userId) const {
    return static_cast<int>(getUnreadNotifications(userId).size());
}

// Marcar como leída
void NotificationManager::markAsRead(size_t index) {
    if (index < notifications.size()) {
        notifications[index].read = true;
    }
}

void NotificationManager::markAllAsRead(int userId) {
    for (auto& notification : notifications) {
        if (notification.userId == userId) {
            notification.read = true;
        }
    }
}

// Limpiar notificaciones
void NotificationManager::clearOldNotifications(int daysOld) {
    auto now = std::chrono::system_clock::now();
    auto cutoff = now - std::chrono::hours(daysOld * 24);
    
    notifications.erase(
        std::remove_if(notifications.begin(), notifications.end(),
            [cutoff](const Notification& n) {
                return n.timestamp < cutoff && n.read;
            }),
        notifications.end()
    );
}

void NotificationManager::clearAll() {
    notifications.clear();
}

// Métodos de utilidad
size_t NotificationManager::getNotificationCount() const {
    return notifications.size();
}

const std::vector<Notification>& NotificationManager::getAllNotifications() const {
    return notifications;
}


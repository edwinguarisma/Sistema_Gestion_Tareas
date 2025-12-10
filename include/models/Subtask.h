#ifndef SUBTASK_H
#define SUBTASK_H

#include <string>
#include <memory>
#include <vector>

/**
 * @brief Clase que representa una subtarea anidada
 * Implementa una estructura de árbol para subtareas anidadas
 */
class Subtask : public std::enable_shared_from_this<Subtask> {
private:
    int id;
    std::string title;
    std::string description;
    bool completed;
    std::vector<std::shared_ptr<Subtask>> childSubtasks;  // Subtareas anidadas

public:
    // Constructores
    Subtask();
    Subtask(int id, const std::string& title, const std::string& description = "");
    
    // Destructor
    ~Subtask();
    
    // Getters
    int getId() const;
    std::string getTitle() const;
    std::string getDescription() const;
    bool isCompleted() const;
    const std::vector<std::shared_ptr<Subtask>>& getChildSubtasks() const;
    
    // Setters
    void setTitle(const std::string& title);
    void setDescription(const std::string& description);
    void setCompleted(bool completed);
    
    // Gestión de subtareas anidadas
    void addChildSubtask(std::shared_ptr<Subtask> subtask);
    void removeChildSubtask(int subtaskId);
    std::shared_ptr<Subtask> findSubtaskById(int id);
    
    // Métodos de utilidad
    int countTotalSubtasks() const;  // Cuenta todas las subtareas recursivamente
    int countCompletedSubtasks() const;
    double getCompletionPercentage() const;
    std::string toString(int depth = 0) const;
};

#endif // SUBTASK_H


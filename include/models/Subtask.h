#ifndef SUBTASK_H
#define SUBTASK_H

#include <string>
#include <memory>
#include <vector>

using namespace std;

/**
 * @brief Clase que representa una subtarea anidada
 * Implementa una estructura de árbol para subtareas anidadas
 */
class Subtask : public enable_shared_from_this<Subtask> {
private:
    int id;
    string title;
    string description;
    bool completed;
    vector<shared_ptr<Subtask>> childSubtasks;  // Subtareas anidadas

public:
    // Constructores
    Subtask();
    Subtask(int id, const string& title, const string& description = "");
    
    // Destructor
    ~Subtask();
    
    // Getters
    int getId() const;
    string getTitle() const;
    string getDescription() const;
    bool isCompleted() const;
    const vector<shared_ptr<Subtask>>& getChildSubtasks() const;
    
    // Setters
    void setTitle(const string& title);
    void setDescription(const string& description);
    void setCompleted(bool completed);
    
    // Gestión de subtareas anidadas
    void addChildSubtask(shared_ptr<Subtask> subtask);
    void removeChildSubtask(int subtaskId);
    shared_ptr<Subtask> findSubtaskById(int id);
    
    // Métodos de utilidad
    int countTotalSubtasks() const;  // Cuenta todas las subtareas recursivamente
    int countCompletedSubtasks() const;
    double getCompletionPercentage() const;
    string toString(int depth = 0) const;
};

#endif // SUBTASK_H


#ifndef PROJECT_H
#define PROJECT_H

#include <string>
#include <vector>
#include <memory>
#include <map>
#include "Board.h"
#include "User.h"

/**
 * @brief Clase que representa un proyecto completo
 * Un proyecto puede contener múltiples tableros y usuarios
 */
class Project {
private:
    int id;
    std::string name;
    std::string description;
    std::chrono::system_clock::time_point createdDate;
    
    // Tableros del proyecto
    std::vector<std::shared_ptr<Board>> boards;
    
    // Usuarios del proyecto
    std::map<int, std::shared_ptr<User>> users;
    
    // Contadores para IDs
    int nextBoardId;
    int nextUserId;

public:
    // Constructores
    Project();
    Project(int id, const std::string& name, const std::string& description = "");
    
    // Destructor
    ~Project();
    
    // Getters
    int getId() const;
    std::string getName() const;
    std::string getDescription() const;
    std::chrono::system_clock::time_point getCreatedDate() const;
    const std::vector<std::shared_ptr<Board>>& getBoards() const;
    
    // Setters
    void setName(const std::string& name);
    void setDescription(const std::string& description);
    
    // Gestión de tableros
    std::shared_ptr<Board> createBoard(const std::string& name, const std::string& description = "");
    void addBoard(std::shared_ptr<Board> board);
    void removeBoard(int boardId);
    std::shared_ptr<Board> findBoardById(int id) const;
    
    // Gestión de usuarios
    std::shared_ptr<User> createUser(const std::string& name, const std::string& email, 
                                      const std::string& role = "developer");
    void addUser(std::shared_ptr<User> user);
    void removeUser(int userId);
    std::shared_ptr<User> findUserById(int id) const;
    std::shared_ptr<User> findUserByEmail(const std::string& email) const;
    std::vector<std::shared_ptr<User>> getAllUsers() const;
    
    // Búsqueda global en el proyecto
    std::shared_ptr<Task> findTaskById(int taskId) const;
    std::vector<std::shared_ptr<Task>> findTasksByUser(int userId) const;
    std::vector<std::shared_ptr<Task>> findTasksByTitle(const std::string& searchText) const;
    
    // Estadísticas del proyecto
    int getTotalTaskCount() const;
    int getTotalBoardCount() const;
    int getTotalUserCount() const;
    double getOverallCompletionPercentage() const;
    
    // Métodos de utilidad
    std::string toString() const;
    void clearAllData();
};

#endif // PROJECT_H


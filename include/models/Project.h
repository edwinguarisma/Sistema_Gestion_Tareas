#ifndef PROJECT_H
#define PROJECT_H

#include <string>
#include <vector>
#include <memory>
#include <map>
#include "Board.h"
#include "User.h"

using namespace std;

/**
 * @brief Clase que representa un proyecto completo
 * Un proyecto puede contener múltiples tableros y usuarios
 */
class Project {
private:
    int id;
    string name;
    string description;
    chrono::system_clock::time_point createdDate;
    
    // Tableros del proyecto
    vector<shared_ptr<Board>> boards;
    
    // Usuarios del proyecto
    map<int, shared_ptr<User>> users;
    
    // Contadores para IDs
    int nextBoardId;
    int nextUserId;

public:
    // Constructores
    Project();
    Project(int id, const string& name, const string& description = "");
    
    // Destructor
    ~Project();
    
    // Getters
    int getId() const;
    string getName() const;
    string getDescription() const;
    chrono::system_clock::time_point getCreatedDate() const;
    const vector<shared_ptr<Board>>& getBoards() const;
    
    // Setters
    void setName(const string& name);
    void setDescription(const string& description);
    
    // Gestión de tableros
    shared_ptr<Board> createBoard(const string& name, const string& description = "");
    void addBoard(shared_ptr<Board> board);
    void removeBoard(int boardId);
    shared_ptr<Board> findBoardById(int id) const;
    
    // Gestión de usuarios
    shared_ptr<User> createUser(const string& name, const string& email, 
                                      const string& role = "developer");
    void addUser(shared_ptr<User> user);
    void removeUser(int userId);
    shared_ptr<User> findUserById(int id) const;
    shared_ptr<User> findUserByEmail(const string& email) const;
    vector<shared_ptr<User>> getAllUsers() const;
    
    // Búsqueda global en el proyecto
    shared_ptr<Task> findTaskById(int taskId) const;
    vector<shared_ptr<Task>> findTasksByUser(int userId) const;
    vector<shared_ptr<Task>> findTasksByTitle(const string& searchText) const;
    
    // Estadísticas del proyecto
    int getTotalTaskCount() const;
    int getTotalBoardCount() const;
    int getTotalUserCount() const;
    double getOverallCompletionPercentage() const;
    
    // Métodos de utilidad
    string toString() const;
    void clearAllData();
};

#endif // PROJECT_H


#ifndef USER_H
#define USER_H

#include <string>
#include <memory>

using namespace std;

/**
 * @brief Clase que representa un usuario del sistema
 */
class User {
private:
    int id;
    string name;
    string email;
    string role;  // "admin", "developer", "manager", etc.

public:
    // Constructores
    User();
    User(int id, const string& name, const string& email, const string& role = "developer");
    
    // Destructor
    ~User();
    
    // Getters
    int getId() const;
    string getName() const;
    string getEmail() const;
    string getRole() const;
    
    // Setters
    void setName(const string& name);
    void setEmail(const string& email);
    void setRole(const string& role);
    
    // Métodos de utilidad
    string toString() const;
    bool operator==(const User& other) const;
};

#endif // USER_H


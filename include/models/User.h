#ifndef USER_H
#define USER_H

#include <string>
#include <memory>

/**
 * @brief Clase que representa un usuario del sistema
 */
class User {
private:
    int id;
    std::string name;
    std::string email;
    std::string role;  // "admin", "developer", "manager", etc.

public:
    // Constructores
    User();
    User(int id, const std::string& name, const std::string& email, const std::string& role = "developer");
    
    // Destructor
    ~User();
    
    // Getters
    int getId() const;
    std::string getName() const;
    std::string getEmail() const;
    std::string getRole() const;
    
    // Setters
    void setName(const std::string& name);
    void setEmail(const std::string& email);
    void setRole(const std::string& role);
    
    // Métodos de utilidad
    std::string toString() const;
    bool operator==(const User& other) const;
};

#endif // USER_H


#include "models/User.h"
#include <sstream>

// Constructores
User::User() : id(-1), name(""), email(""), role("developer") {}

User::User(int id, const std::string& name, const std::string& email, const std::string& role)
    : id(id), name(name), email(email), role(role) {}

// Destructor
User::~User() {}

// Getters
int User::getId() const {
    return id;
}

std::string User::getName() const {
    return name;
}

std::string User::getEmail() const {
    return email;
}

std::string User::getRole() const {
    return role;
}

// Setters
void User::setName(const std::string& name) {
    this->name = name;
}

void User::setEmail(const std::string& email) {
    this->email = email;
}

void User::setRole(const std::string& role) {
    this->role = role;
}

// Métodos de utilidad
std::string User::toString() const {
    std::stringstream ss;
    ss << "User[ID: " << id << ", Name: " << name 
       << ", Email: " << email << ", Role: " << role << "]";
    return ss.str();
}

bool User::operator==(const User& other) const {
    return id == other.id;
}


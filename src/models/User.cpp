#include "models/User.h"
#include <sstream>

using namespace std;

// Constructores
User::User() : id(-1), name(""), email(""), role("developer") {}

User::User(int id, const string& name, const string& email, const string& role)
    : id(id), name(name), email(email), role(role) {}

// Destructor
User::~User() {}

// Getters
int User::getId() const {
    return id;
}

string User::getName() const {
    return name;
}

string User::getEmail() const {
    return email;
}

string User::getRole() const {
    return role;
}

// Setters
void User::setName(const string& name) {
    this->name = name;
}

void User::setEmail(const string& email) {
    this->email = email;
}

void User::setRole(const string& role) {
    this->role = role;
}

// Métodos de utilidad
string User::toString() const {
    stringstream ss;
    ss << "User[ID: " << id << ", Name: " << name 
       << ", Email: " << email << ", Role: " << role << "]";
    return ss.str();
}

bool User::operator==(const User& other) const {
    return id == other.id;
}


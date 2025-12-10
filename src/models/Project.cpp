#include "models/Project.h"
#include <sstream>
#include <algorithm>

using namespace std;

// Constructores
Project::Project()
    : id(-1), name(""), description(""), 
      createdDate(chrono::system_clock::now()),
      nextBoardId(1), nextUserId(1) {}

Project::Project(int id, const string& name, const string& description)
    : id(id), name(name), description(description),
      createdDate(chrono::system_clock::now()),
      nextBoardId(1), nextUserId(1) {}

// Destructor
Project::~Project() {}

// Getters
int Project::getId() const {
    return id;
}

string Project::getName() const {
    return name;
}

string Project::getDescription() const {
    return description;
}

chrono::system_clock::time_point Project::getCreatedDate() const {
    return createdDate;
}

const vector<shared_ptr<Board>>& Project::getBoards() const {
    return boards;
}

// Setters
void Project::setName(const string& name) {
    this->name = name;
}

void Project::setDescription(const string& description) {
    this->description = description;
}

// Gestión de tableros
shared_ptr<Board> Project::createBoard(const string& name, 
                                             const string& description) {
    auto board = make_shared<Board>(nextBoardId++, name, description);
    boards.push_back(board);
    return board;
}

void Project::addBoard(shared_ptr<Board> board) {
    if (board) {
        boards.push_back(board);
    }
}

void Project::removeBoard(int boardId) {
    boards.erase(
        remove_if(boards.begin(), boards.end(),
            [boardId](const shared_ptr<Board>& b) {
                return b->getId() == boardId;
            }),
        boards.end()
    );
}

shared_ptr<Board> Project::findBoardById(int id) const {
    for (const auto& board : boards) {
        if (board->getId() == id) {
            return board;
        }
    }
    return nullptr;
}

// Gestión de usuarios
shared_ptr<User> Project::createUser(const string& name, 
                                           const string& email,
                                           const string& role) {
    auto user = make_shared<User>(nextUserId++, name, email, role);
    users[user->getId()] = user;
    return user;
}

void Project::addUser(shared_ptr<User> user) {
    if (user) {
        users[user->getId()] = user;
    }
}

void Project::removeUser(int userId) {
    users.erase(userId);
}

shared_ptr<User> Project::findUserById(int id) const {
    auto it = users.find(id);
    return (it != users.end()) ? it->second : nullptr;
}

shared_ptr<User> Project::findUserByEmail(const string& email) const {
    for (const auto& pair : users) {
        if (pair.second->getEmail() == email) {
            return pair.second;
        }
    }
    return nullptr;
}

vector<shared_ptr<User>> Project::getAllUsers() const {
    vector<shared_ptr<User>> result;
    
    for (const auto& pair : users) {
        result.push_back(pair.second);
    }
    
    return result;
}

// Búsqueda global en el proyecto
shared_ptr<Task> Project::findTaskById(int taskId) const {
    for (const auto& board : boards) {
        auto task = board->findTaskById(taskId);
        if (task) {
            return task;
        }
    }
    return nullptr;
}

vector<shared_ptr<Task>> Project::findTasksByUser(int userId) const {
    vector<shared_ptr<Task>> result;
    
    for (const auto& board : boards) {
        auto tasks = board->getTasksByUser(userId);
        result.insert(result.end(), tasks.begin(), tasks.end());
    }
    
    return result;
}

vector<shared_ptr<Task>> Project::findTasksByTitle(const string& searchText) const {
    vector<shared_ptr<Task>> result;
    
    for (const auto& board : boards) {
        auto tasks = board->getAllTasks();
        for (const auto& task : tasks) {
            if (task->getTitle().find(searchText) != string::npos) {
                result.push_back(task);
            }
        }
    }
    
    return result;
}

// Estadísticas del proyecto
int Project::getTotalTaskCount() const {
    int count = 0;
    for (const auto& board : boards) {
        count += board->getTotalTaskCount();
    }
    return count;
}

int Project::getTotalBoardCount() const {
    return static_cast<int>(boards.size());
}

int Project::getTotalUserCount() const {
    return static_cast<int>(users.size());
}

double Project::getOverallCompletionPercentage() const {
    if (boards.empty()) {
        return 0.0;
    }
    
    double totalCompletion = 0.0;
    for (const auto& board : boards) {
        totalCompletion += board->getCompletionPercentage();
    }
    
    return totalCompletion / boards.size();
}

// Métodos de utilidad
string Project::toString() const {
    stringstream ss;
    ss << "Project[ID: " << id << ", Name: " << name << "]\n";
    ss << "  Boards: " << getTotalBoardCount() << "\n";
    ss << "  Users: " << getTotalUserCount() << "\n";
    ss << "  Total Tasks: " << getTotalTaskCount() << "\n";
    ss << "  Completion: " << getOverallCompletionPercentage() << "%\n";
    
    return ss.str();
}

void Project::clearAllData() {
    boards.clear();
    users.clear();
    nextBoardId = 1;
    nextUserId = 1;
}


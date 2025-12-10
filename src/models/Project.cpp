#include "models/Project.h"
#include <sstream>
#include <algorithm>

// Constructores
Project::Project()
    : id(-1), name(""), description(""), 
      createdDate(std::chrono::system_clock::now()),
      nextBoardId(1), nextUserId(1) {}

Project::Project(int id, const std::string& name, const std::string& description)
    : id(id), name(name), description(description),
      createdDate(std::chrono::system_clock::now()),
      nextBoardId(1), nextUserId(1) {}

// Destructor
Project::~Project() {}

// Getters
int Project::getId() const {
    return id;
}

std::string Project::getName() const {
    return name;
}

std::string Project::getDescription() const {
    return description;
}

std::chrono::system_clock::time_point Project::getCreatedDate() const {
    return createdDate;
}

const std::vector<std::shared_ptr<Board>>& Project::getBoards() const {
    return boards;
}

// Setters
void Project::setName(const std::string& name) {
    this->name = name;
}

void Project::setDescription(const std::string& description) {
    this->description = description;
}

// Gestión de tableros
std::shared_ptr<Board> Project::createBoard(const std::string& name, 
                                             const std::string& description) {
    auto board = std::make_shared<Board>(nextBoardId++, name, description);
    boards.push_back(board);
    return board;
}

void Project::addBoard(std::shared_ptr<Board> board) {
    if (board) {
        boards.push_back(board);
    }
}

void Project::removeBoard(int boardId) {
    boards.erase(
        std::remove_if(boards.begin(), boards.end(),
            [boardId](const std::shared_ptr<Board>& b) {
                return b->getId() == boardId;
            }),
        boards.end()
    );
}

std::shared_ptr<Board> Project::findBoardById(int id) const {
    for (const auto& board : boards) {
        if (board->getId() == id) {
            return board;
        }
    }
    return nullptr;
}

// Gestión de usuarios
std::shared_ptr<User> Project::createUser(const std::string& name, 
                                           const std::string& email,
                                           const std::string& role) {
    auto user = std::make_shared<User>(nextUserId++, name, email, role);
    users[user->getId()] = user;
    return user;
}

void Project::addUser(std::shared_ptr<User> user) {
    if (user) {
        users[user->getId()] = user;
    }
}

void Project::removeUser(int userId) {
    users.erase(userId);
}

std::shared_ptr<User> Project::findUserById(int id) const {
    auto it = users.find(id);
    return (it != users.end()) ? it->second : nullptr;
}

std::shared_ptr<User> Project::findUserByEmail(const std::string& email) const {
    for (const auto& pair : users) {
        if (pair.second->getEmail() == email) {
            return pair.second;
        }
    }
    return nullptr;
}

std::vector<std::shared_ptr<User>> Project::getAllUsers() const {
    std::vector<std::shared_ptr<User>> result;
    
    for (const auto& pair : users) {
        result.push_back(pair.second);
    }
    
    return result;
}

// Búsqueda global en el proyecto
std::shared_ptr<Task> Project::findTaskById(int taskId) const {
    for (const auto& board : boards) {
        auto task = board->findTaskById(taskId);
        if (task) {
            return task;
        }
    }
    return nullptr;
}

std::vector<std::shared_ptr<Task>> Project::findTasksByUser(int userId) const {
    std::vector<std::shared_ptr<Task>> result;
    
    for (const auto& board : boards) {
        auto tasks = board->getTasksByUser(userId);
        result.insert(result.end(), tasks.begin(), tasks.end());
    }
    
    return result;
}

std::vector<std::shared_ptr<Task>> Project::findTasksByTitle(const std::string& searchText) const {
    std::vector<std::shared_ptr<Task>> result;
    
    for (const auto& board : boards) {
        auto tasks = board->getAllTasks();
        for (const auto& task : tasks) {
            if (task->getTitle().find(searchText) != std::string::npos) {
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
std::string Project::toString() const {
    std::stringstream ss;
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


#include "utils/DataPersistence.h"
#include "utils/DateUtils.h"
#include <fstream>
#include <sstream>
#include <filesystem>
#include <algorithm>

// Constructor
DataPersistence::DataPersistence(const std::string& dataDirectory)
    : dataDirectory(dataDirectory) {
    if (!directoryExists(dataDirectory)) {
        createDirectory(dataDirectory);
    }
}

// Destructor
DataPersistence::~DataPersistence() {}

// Utilidades de strings JSON
std::string DataPersistence::escapeJson(const std::string& str) const {
    std::string result;
    for (char c : str) {
        switch (c) {
            case '"': result += "\\\""; break;
            case '\\': result += "\\\\"; break;
            case '\n': result += "\\n"; break;
            case '\r': result += "\\r"; break;
            case '\t': result += "\\t"; break;
            default: result += c; break;
        }
    }
    return result;
}

std::string DataPersistence::unescapeJson(const std::string& str) const {
    std::string result;
    bool escape = false;
    
    for (char c : str) {
        if (escape) {
            switch (c) {
                case 'n': result += '\n'; break;
                case 'r': result += '\r'; break;
                case 't': result += '\t'; break;
                case '\\': result += '\\'; break;
                case '"': result += '"'; break;
                default: result += c; break;
            }
            escape = false;
        } else if (c == '\\') {
            escape = true;
        } else {
            result += c;
        }
    }
    
    return result;
}

// Serialización en formato TXT simple
std::string DataPersistence::serializeUser(std::shared_ptr<User> user) const {
    if (!user) return "";
    
    std::stringstream ss;
    ss << "USER|" << user->getId() << "|" 
       << user->getName() << "|" 
       << user->getEmail() << "|" 
       << user->getRole() << "\n";
    
    return ss.str();
}

std::string DataPersistence::serializeTask(std::shared_ptr<Task> task) const {
    if (!task) return "";
    
    std::stringstream ss;
    ss << "TASK|" << task->getId() << "|" 
       << task->getTitle() << "|" 
       << task->getDescription() << "|" 
       << task->getState() << "|" 
       << task->getAssignedUserId() << "|" 
       << task->getPriority() << "|" 
       << DateUtils::toDateTimeString(task->getDueDate()) << "|" 
       << DateUtils::toDateTimeString(task->getCreatedDate());
    
    // Tags (separados por coma)
    const auto& tags = task->getTags();
    ss << "|";
    for (size_t i = 0; i < tags.size(); ++i) {
        ss << tags[i];
        if (i < tags.size() - 1) ss << ",";
    }
    
    // Dependencias (separadas por coma)
    const auto& deps = task->getDependencies();
    ss << "|";
    bool first = true;
    for (int depId : deps) {
        if (!first) ss << ",";
        ss << depId;
        first = false;
    }
    
    ss << "\n";
    return ss.str();
}

std::string DataPersistence::serializeBoard(std::shared_ptr<Board> board) const {
    if (!board) return "";
    
    std::stringstream ss;
    ss << "BOARD|" << board->getId() << "|" 
       << board->getName() << "|" 
       << board->getDescription();
    
    // Estados (separados por coma)
    const auto& states = board->getStates();
    ss << "|";
    for (size_t i = 0; i < states.size(); ++i) {
        ss << states[i];
        if (i < states.size() - 1) ss << ",";
    }
    ss << "\n";
    
    // Tareas del board
    auto tasks = board->getAllTasks();
    for (const auto& task : tasks) {
        ss << serializeTask(task);
    }
    
    return ss.str();
}

std::string DataPersistence::serializeProject(std::shared_ptr<Project> project) const {
    if (!project) return "";
    
    std::stringstream ss;
    ss << "PROJECT|" << project->getId() << "|" 
       << project->getName() << "|" 
       << project->getDescription() << "|" 
       << DateUtils::toDateTimeString(project->getCreatedDate()) << "\n";
    
    // Usuarios
    auto users = project->getAllUsers();
    for (const auto& user : users) {
        ss << serializeUser(user);
    }
    
    // Boards
    const auto& boards = project->getBoards();
    for (const auto& board : boards) {
        ss << serializeBoard(board);
    }
    
    return ss.str();
}

// Guardar
bool DataPersistence::saveProject(std::shared_ptr<Project> project) {
    if (!project) return false;
    
    std::string filePath = getProjectFilePath(project->getId());
    std::string json = serializeProject(project);
    
    std::ofstream file(filePath);
    if (!file.is_open()) {
        return false;
    }
    
    file << json;
    file.close();
    
    return true;
}

bool DataPersistence::saveAllProjects(std::shared_ptr<ProjectManager> manager) {
    if (!manager) return false;
    
    const auto& projects = manager->getAllProjects();
    
    for (const auto& project : projects) {
        if (!saveProject(project)) {
            return false;
        }
    }
    
    return true;
}

bool DataPersistence::createBackup(int projectId) {
    std::string originalPath = getProjectFilePath(projectId);
    std::string backupPath = originalPath + ".backup";
    
    try {
        std::filesystem::copy_file(originalPath, backupPath, 
                                  std::filesystem::copy_options::overwrite_existing);
        return true;
    } catch (...) {
        return false;
    }
}

// Cargar desde archivo TXT
std::shared_ptr<Project> DataPersistence::loadProject(int projectId) {
    std::string filePath = getProjectFilePath(projectId);
    
    std::ifstream file(filePath);
    if (!file.is_open()) {
        return nullptr;
    }
    
    std::shared_ptr<Project> project = nullptr;
    std::shared_ptr<Board> currentBoard = nullptr;
    std::string line;
    
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        
        std::stringstream ss(line);
        std::string type;
        std::getline(ss, type, '|');
        
        if (type == "PROJECT") {
            std::string idStr, name, desc, date;
            std::getline(ss, idStr, '|');
            std::getline(ss, name, '|');
            std::getline(ss, desc, '|');
            std::getline(ss, date, '|');
            
            project = std::make_shared<Project>(std::stoi(idStr), name, desc);
        }
        else if (type == "USER" && project) {
            std::string idStr, name, email, role;
            std::getline(ss, idStr, '|');
            std::getline(ss, name, '|');
            std::getline(ss, email, '|');
            std::getline(ss, role, '|');
            
            project->createUser(name, email, role);
        }
        else if (type == "BOARD" && project) {
            std::string idStr, name, desc, statesStr;
            std::getline(ss, idStr, '|');
            std::getline(ss, name, '|');
            std::getline(ss, desc, '|');
            std::getline(ss, statesStr, '|');
            
            currentBoard = project->createBoard(name);
            currentBoard->setDescription(desc);
            
            // Parsear estados si es necesario
        }
        else if (type == "TASK" && project && currentBoard) {
            std::string idStr, title, desc, state, userIdStr, priorityStr, dueDate, createdDate, tagsStr, depsStr;
            std::getline(ss, idStr, '|');
            std::getline(ss, title, '|');
            std::getline(ss, desc, '|');
            std::getline(ss, state, '|');
            std::getline(ss, userIdStr, '|');
            std::getline(ss, priorityStr, '|');
            std::getline(ss, dueDate, '|');
            std::getline(ss, createdDate, '|');
            std::getline(ss, tagsStr, '|');
            std::getline(ss, depsStr, '|');
            
            int taskId = std::stoi(idStr);
            int userId = std::stoi(userIdStr);
            int priority = std::stoi(priorityStr);
            
            auto task = std::make_shared<Task>(taskId, title, desc);
            task->setPriority(priority);
            if (userId >= 0) {
                task->setAssignedUserId(userId, "System");
            }
            task->setState(state, "System");
            
            // Parsear tags
            if (!tagsStr.empty()) {
                std::stringstream tagStream(tagsStr);
                std::string tag;
                while (std::getline(tagStream, tag, ',')) {
                    if (!tag.empty()) {
                        task->addTag(tag);
                    }
                }
            }
            
            currentBoard->addTask(task, state);
        }
    }
    
    file.close();
    return project;
}

bool DataPersistence::loadAllProjects(std::shared_ptr<ProjectManager> manager) {
    if (!manager) return false;
    
    auto projectIds = getAvailableProjectIds();
    
    for (int id : projectIds) {
        auto project = loadProject(id);
        if (project) {
            manager->addProject(project);
        }
    }
    
    return true;
}

// Eliminar
bool DataPersistence::deleteProject(int projectId) {
    std::string filePath = getProjectFilePath(projectId);
    
    try {
        return std::filesystem::remove(filePath);
    } catch (...) {
        return false;
    }
}

// Exportar/Importar
bool DataPersistence::exportProjectToFile(std::shared_ptr<Project> project, 
                                         const std::string& filePath) {
    if (!project) return false;
    
    std::string json = serializeProject(project);
    
    std::ofstream file(filePath);
    if (!file.is_open()) {
        return false;
    }
    
    file << json;
    file.close();
    
    return true;
}

std::shared_ptr<Project> DataPersistence::importProjectFromFile(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        return nullptr;
    }
    
    // Parsear y crear proyecto
    // Implementación simplificada
    file.close();
    return nullptr;
}

// Métodos de utilidad
bool DataPersistence::directoryExists(const std::string& path) const {
    return std::filesystem::exists(path) && std::filesystem::is_directory(path);
}

bool DataPersistence::createDirectory(const std::string& path) const {
    try {
        return std::filesystem::create_directories(path);
    } catch (...) {
        return false;
    }
}

std::vector<int> DataPersistence::getAvailableProjectIds() const {
    std::vector<int> ids;
    
    try {
        for (const auto& entry : std::filesystem::directory_iterator(dataDirectory)) {
            if (entry.is_regular_file()) {
                std::string filename = entry.path().filename().string();
                if (filename.find("project_") == 0 && filename.find(".txt") != std::string::npos) {
                    std::string idStr = filename.substr(8, filename.length() - 12);
                    ids.push_back(std::stoi(idStr));
                }
            }
        }
    } catch (...) {
        // Error al leer directorio
    }
    
    return ids;
}

std::string DataPersistence::getProjectFilePath(int projectId) const {
    return dataDirectory + "/project_" + std::to_string(projectId) + ".txt";
}

std::string DataPersistence::getProjectsIndexPath() const {
    return dataDirectory + "/projects_index.txt";
}


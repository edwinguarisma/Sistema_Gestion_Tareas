#include "utils/DataPersistence.h"
#include "utils/DateUtils.h"
#include <fstream>
#include <sstream>
#include <filesystem>
#include <algorithm>

using namespace std;

// Constructor
DataPersistence::DataPersistence(const string& dataDirectory)
    : dataDirectory(dataDirectory) {
    if (!directoryExists(dataDirectory)) {
        createDirectory(dataDirectory);
    }
}

// Destructor
DataPersistence::~DataPersistence() {}

// Utilidades de strings JSON
string DataPersistence::escapeJson(const string& str) const {
    string result;
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

string DataPersistence::unescapeJson(const string& str) const {
    string result;
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
string DataPersistence::serializeUser(shared_ptr<User> user) const {
    if (!user) return "";
    
    stringstream ss;
    ss << "USER|" << user->getId() << "|" 
       << user->getName() << "|" 
       << user->getEmail() << "|" 
       << user->getRole() << "\n";
    
    return ss.str();
}

string DataPersistence::serializeTask(shared_ptr<Task> task) const {
    if (!task) return "";
    
    stringstream ss;
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

string DataPersistence::serializeBoard(shared_ptr<Board> board) const {
    if (!board) return "";
    
    stringstream ss;
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

string DataPersistence::serializeProject(shared_ptr<Project> project) const {
    if (!project) return "";
    
    stringstream ss;
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
bool DataPersistence::saveProject(shared_ptr<Project> project) {
    if (!project) return false;
    
    string filePath = getProjectFilePath(project->getId());
    string json = serializeProject(project);
    
    ofstream file(filePath);
    if (!file.is_open()) {
        return false;
    }
    
    file << json;
    file.close();
    
    return true;
}

bool DataPersistence::saveAllProjects(shared_ptr<ProjectManager> manager) {
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
    string originalPath = getProjectFilePath(projectId);
    string backupPath = originalPath + ".backup";
    
    try {
        filesystem::copy_file(originalPath, backupPath, 
                                  filesystem::copy_options::overwrite_existing);
        return true;
    } catch (...) {
        return false;
    }
}

// Cargar desde archivo TXT
shared_ptr<Project> DataPersistence::loadProject(int projectId) {
    string filePath = getProjectFilePath(projectId);
    
    ifstream file(filePath);
    if (!file.is_open()) {
        return nullptr;
    }
    
    shared_ptr<Project> project = nullptr;
    shared_ptr<Board> currentBoard = nullptr;
    string line;
    
    while (getline(file, line)) {
        if (line.empty()) continue;
        
        stringstream ss(line);
        string type;
        getline(ss, type, '|');
        
        if (type == "PROJECT") {
            string idStr, name, desc, date;
            getline(ss, idStr, '|');
            getline(ss, name, '|');
            getline(ss, desc, '|');
            getline(ss, date, '|');
            
            project = make_shared<Project>(stoi(idStr), name, desc);
        }
        else if (type == "USER" && project) {
            string idStr, name, email, role;
            getline(ss, idStr, '|');
            getline(ss, name, '|');
            getline(ss, email, '|');
            getline(ss, role, '|');
            
            project->createUser(name, email, role);
        }
        else if (type == "BOARD" && project) {
            string idStr, name, desc, statesStr;
            getline(ss, idStr, '|');
            getline(ss, name, '|');
            getline(ss, desc, '|');
            getline(ss, statesStr, '|');
            
            currentBoard = project->createBoard(name);
            currentBoard->setDescription(desc);
            
            // Parsear estados si es necesario
        }
        else if (type == "TASK" && project && currentBoard) {
            string idStr, title, desc, state, userIdStr, priorityStr, dueDate, createdDate, tagsStr, depsStr;
            getline(ss, idStr, '|');
            getline(ss, title, '|');
            getline(ss, desc, '|');
            getline(ss, state, '|');
            getline(ss, userIdStr, '|');
            getline(ss, priorityStr, '|');
            getline(ss, dueDate, '|');
            getline(ss, createdDate, '|');
            getline(ss, tagsStr, '|');
            getline(ss, depsStr, '|');
            
            int taskId = stoi(idStr);
            int userId = stoi(userIdStr);
            int priority = stoi(priorityStr);
            
            auto task = make_shared<Task>(taskId, title, desc);
            task->setPriority(priority);
            if (userId >= 0) {
                task->setAssignedUserId(userId, "System");
            }
            task->setState(state, "System");
            
            // Parsear tags
            if (!tagsStr.empty()) {
                stringstream tagStream(tagsStr);
                string tag;
                while (getline(tagStream, tag, ',')) {
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

bool DataPersistence::loadAllProjects(shared_ptr<ProjectManager> manager) {
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
    string filePath = getProjectFilePath(projectId);
    
    try {
        return filesystem::remove(filePath);
    } catch (...) {
        return false;
    }
}

// Exportar/Importar
bool DataPersistence::exportProjectToFile(shared_ptr<Project> project, 
                                         const string& filePath) {
    if (!project) return false;
    
    string json = serializeProject(project);
    
    ofstream file(filePath);
    if (!file.is_open()) {
        return false;
    }
    
    file << json;
    file.close();
    
    return true;
}

shared_ptr<Project> DataPersistence::importProjectFromFile(const string& filePath) {
    ifstream file(filePath);
    if (!file.is_open()) {
        return nullptr;
    }
    
    // Parsear y crear proyecto
    // Implementación simplificada
    file.close();
    return nullptr;
}

// Métodos de utilidad
bool DataPersistence::directoryExists(const string& path) const {
    return filesystem::exists(path) && filesystem::is_directory(path);
}

bool DataPersistence::createDirectory(const string& path) const {
    try {
        return filesystem::create_directories(path);
    } catch (...) {
        return false;
    }
}

vector<int> DataPersistence::getAvailableProjectIds() const {
    vector<int> ids;
    
    try {
        for (const auto& entry : filesystem::directory_iterator(dataDirectory)) {
            if (entry.is_regular_file()) {
                string filename = entry.path().filename().string();
                if (filename.find("project_") == 0 && filename.find(".txt") != string::npos) {
                    string idStr = filename.substr(8, filename.length() - 12);
                    ids.push_back(stoi(idStr));
                }
            }
        }
    } catch (...) {
        // Error al leer directorio
    }
    
    return ids;
}

string DataPersistence::getProjectFilePath(int projectId) const {
    return dataDirectory + "/project_" + to_string(projectId) + ".txt";
}

string DataPersistence::getProjectsIndexPath() const {
    return dataDirectory + "/projects_index.txt";
}


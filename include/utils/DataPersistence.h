#ifndef DATA_PERSISTENCE_H
#define DATA_PERSISTENCE_H

#include <string>
#include <memory>
#include "models/Project.h"
#include "managers/ProjectManager.h"

using namespace std;

/**
 * @brief Clase para persistencia de datos
 * Guarda y carga proyectos en formato JSON
 */
class DataPersistence
{
private:
    string dataDirectory;

    // Métodos auxiliares de serialización
    string serializeTask(shared_ptr<Task> task) const;
    string serializeBoard(shared_ptr<Board> board) const;
    string serializeProject(shared_ptr<Project> project) const;
    string serializeUser(shared_ptr<User> user) const;

    // Métodos auxiliares de deserialización
    shared_ptr<Task> deserializeTask(const string &json) const;
    shared_ptr<Board> deserializeBoard(const string &json) const;
    shared_ptr<Project> deserializeProject(const string &json) const;
    shared_ptr<User> deserializeUser(const string &json) const;

    // Utilidades
    string escapeJson(const string &str) const;
    string unescapeJson(const string &str) const;
    string getProjectFilePath(int projectId) const;
    string getProjectsIndexPath() const;

public:
    // Constructor
    DataPersistence(const string &dataDirectory = "data");

    // Destructor
    ~DataPersistence();

    // Guardar
    bool saveProject(shared_ptr<Project> project);
    bool saveAllProjects(shared_ptr<ProjectManager> manager);
    bool createBackup(int projectId);

    // Cargar
    shared_ptr<Project> loadProject(int projectId);
    bool loadAllProjects(shared_ptr<ProjectManager> manager);

    // Eliminar
    bool deleteProject(int projectId);

    // Exportar/Importar
    bool exportProjectToFile(shared_ptr<Project> project, const string &filePath);
    shared_ptr<Project> importProjectFromFile(const string &filePath);

    // Métodos de utilidad
    bool directoryExists(const string &path) const;
    bool createDirectory(const string &path) const;
    vector<int> getAvailableProjectIds() const;
};

#endif // DATA_PERSISTENCE_H

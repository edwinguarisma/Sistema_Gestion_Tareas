#ifndef DATA_PERSISTENCE_H
#define DATA_PERSISTENCE_H

#include <string>
#include <memory>
#include "models/Project.h"
#include "managers/ProjectManager.h"

/**
 * @brief Clase para persistencia de datos
 * Guarda y carga proyectos en formato JSON
 */
class DataPersistence
{
private:
    std::string dataDirectory;

    // Métodos auxiliares de serialización
    std::string serializeTask(std::shared_ptr<Task> task) const;
    std::string serializeBoard(std::shared_ptr<Board> board) const;
    std::string serializeProject(std::shared_ptr<Project> project) const;
    std::string serializeUser(std::shared_ptr<User> user) const;

    // Métodos auxiliares de deserialización
    std::shared_ptr<Task> deserializeTask(const std::string &json) const;
    std::shared_ptr<Board> deserializeBoard(const std::string &json) const;
    std::shared_ptr<Project> deserializeProject(const std::string &json) const;
    std::shared_ptr<User> deserializeUser(const std::string &json) const;

    // Utilidades
    std::string escapeJson(const std::string &str) const;
    std::string unescapeJson(const std::string &str) const;
    std::string getProjectFilePath(int projectId) const;
    std::string getProjectsIndexPath() const;

public:
    // Constructor
    DataPersistence(const std::string &dataDirectory = "data");

    // Destructor
    ~DataPersistence();

    // Guardar
    bool saveProject(std::shared_ptr<Project> project);
    bool saveAllProjects(std::shared_ptr<ProjectManager> manager);
    bool createBackup(int projectId);

    // Cargar
    std::shared_ptr<Project> loadProject(int projectId);
    bool loadAllProjects(std::shared_ptr<ProjectManager> manager);

    // Eliminar
    bool deleteProject(int projectId);

    // Exportar/Importar
    bool exportProjectToFile(std::shared_ptr<Project> project, const std::string &filePath);
    std::shared_ptr<Project> importProjectFromFile(const std::string &filePath);

    // Métodos de utilidad
    bool directoryExists(const std::string &path) const;
    bool createDirectory(const std::string &path) const;
    std::vector<int> getAvailableProjectIds() const;
};

#endif // DATA_PERSISTENCE_H

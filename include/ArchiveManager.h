#ifndef ARCHIVEMANAGER_H
#define ARCHIVEMANAGER_H

#include "Directory.h"
#include "File.h"
#include "AccessLevel.h"
#include <memory>
#include <string>
#include <fstream>
#include <cstdint>

class ArchiveManager {
private:
    std::unique_ptr<Directory> root;
    AccessLevel currentUserLevel;
    const uint32_t MAGIC_NUMBER = 0xFEEDBEEF;

    void logOperation(const std::string& operation, bool success, const std::string& details) const;
    void serializeResource(const Resource* res, std::ofstream& out) const;
    std::unique_ptr<Resource> deserializeResource(std::ifstream& in) const;
    void writeString(std::ofstream& out, const std::string& str) const;
    std::string readString(std::ifstream& in) const;

public:
    ArchiveManager();

    void setCurrentUserLevel(AccessLevel level);
    AccessLevel getCurrentUserLevel() const;

    void addDirectory(const std::string& name, AccessLevel level);
    void addFile(const std::string& name, const std::string& ext, size_t size);

    void printTree() const;
    void globalAudit() const;

    void saveToFile(const std::string& filename) const;
    void loadFromFile(const std::string& filename);

    void searchByMask(const std::string& maskStr) const;
    void sortResources(int criteria);
};

#endif // ARCHIVEMANAGER_H
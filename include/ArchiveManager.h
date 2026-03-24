#ifndef ARCHIVEMANAGER_H
#define ARCHIVEMANAGER_H

#include "Directory.h"
#include "File.h"
#include "AccessLevel.h"
#include <memory>
#include <string>

class ArchiveManager {
private:
    std::unique_ptr<Directory> root;
    AccessLevel currentUserLevel;

    void logOperation(const std::string& operation, bool success, const std::string& details) const;

public:
    ArchiveManager();

    void setCurrentUserLevel(AccessLevel level);
    AccessLevel getCurrentUserLevel() const;

    void addDirectory(const std::string& name, AccessLevel level);
    void addFile(const std::string& name, const std::string& ext, size_t size);

    void printTree() const;
    void globalAudit() const;
};

#endif // ARCHIVEMANAGER_H
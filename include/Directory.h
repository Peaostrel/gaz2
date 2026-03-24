#ifndef DIRECTORY_H
#define DIRECTORY_H

#include "Resource.h"
#include "AccessLevel.h"
#include <vector>
#include <memory>
#include <functional>

class Directory : public Resource {
private:
    AccessLevel level;
    std::vector<std::unique_ptr<Resource>> children;

public:
    Directory(const std::string& name, AccessLevel accLevel = AccessLevel::USER);

    AccessLevel getAccessLevel() const;
    void setAccessLevel(AccessLevel newLevel);

    void addResource(std::unique_ptr<Resource> resource);
    const std::vector<std::unique_ptr<Resource>>& getChildren() const;

    void collectAll(std::vector<const Resource*>& list) const;
    void sortChildren(const std::function<bool(const std::unique_ptr<Resource>&, const std::unique_ptr<Resource>&)>& comp);
    
    // Метод для рекурсивного удаления
    bool removeResource(const std::string& targetName, AccessLevel userLevel);

    size_t calculateSize() const override;
    void print(int depth = 0) const override;
    bool isDirectory() const override;
};

#endif // DIRECTORY_H
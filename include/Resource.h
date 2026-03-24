#ifndef RESOURCE_H
#define RESOURCE_H

#include <string>
#include <ctime>

class Resource {
private:
    std::string name;
    std::time_t creationDate;

public:
    Resource(const std::string& name);
    virtual ~Resource() = default;

    Resource(const Resource&) = delete;
    Resource& operator=(const Resource&) = delete;

    const std::string& getName() const;
    void setName(const std::string& newName);
    std::time_t getCreationDate() const;
    void updateCreationDate();

    virtual size_t calculateSize() const = 0;
    virtual void print(int depth = 0) const = 0;
    virtual bool isDirectory() const = 0;
};

#endif // RESOURCE_H
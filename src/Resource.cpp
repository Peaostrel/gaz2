#include "Resource.h"
#include "FileSystemException.h"
#include <regex>

Resource::Resource(const std::string& name) {
    setName(name);
    updateCreationDate();
}

const std::string& Resource::getName() const {
    return name;
}

void Resource::setName(const std::string& newName) {
    std::regex invalidChars("[\\\\/:*?\"<>|]");
    if (std::regex_search(newName, invalidChars)) {
        throw FileSystemException("Недопустимые символы в имени ресурса: " + newName);
    }
    if (newName.empty()) {
        throw FileSystemException("Имя ресурса не может быть пустым");
    }
    name = newName;
}

std::time_t Resource::getCreationDate() const {
    return creationDate;
}

void Resource::updateCreationDate() {
    creationDate = std::time(nullptr);
}
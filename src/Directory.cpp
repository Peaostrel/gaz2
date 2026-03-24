#include "Directory.h"
#include <iostream>

Directory::Directory(const std::string& name, AccessLevel accLevel)
    : Resource(name), level(accLevel) {}

AccessLevel Directory::getAccessLevel() const {
    return level;
}

void Directory::setAccessLevel(AccessLevel newLevel) {
    level = newLevel;
}

void Directory::addResource(std::unique_ptr<Resource> resource) {
    if (resource) {
        children.push_back(std::move(resource));
    }
}

size_t Directory::calculateSize() const {
    size_t totalSize = 0;
    for (const auto& child : children) {
        totalSize += child->calculateSize(); // Рекурсивный обход
    }
    return totalSize;
}

void Directory::print(int depth) const {
    std::string indent(depth * 2, ' ');
    // Вывод структуры в консоль с отступами
    std::cout << indent << "+ [Dir] " << getName() 
              << " (Access: " << static_cast<int>(level) << ")\n";
    
    for (const auto& child : children) {
        child->print(depth + 1);
    }
}

bool Directory::isDirectory() const {
    return true;
}
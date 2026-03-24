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

const std::vector<std::unique_ptr<Resource>>& Directory::getChildren() const {
    return children;
}

void Directory::collectAll(std::vector<const Resource*>& list) const {
    list.push_back(this);
    for (const auto& child : children) {
        if (child->isDirectory()) {
            dynamic_cast<const Directory*>(child.get())->collectAll(list);
        } else {
            list.push_back(child.get());
        }
    }
}

size_t Directory::calculateSize() const {
    size_t totalSize = 0;
    for (const auto& child : children) {
        totalSize += child->calculateSize();
    }
    return totalSize;
}

void Directory::print(int depth) const {
    std::string indent(depth * 2, ' ');
    std::cout << indent << "+ [Dir] " << getName() 
              << " (Access: " << static_cast<int>(level) << ")\n";
    
    for (const auto& child : children) {
        child->print(depth + 1);
    }
}

bool Directory::isDirectory() const {
    return true;
}
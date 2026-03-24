#include "Directory.h"
#include "FileSystemException.h"
#include <iostream>
#include <algorithm>

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

void Directory::sortChildren(const std::function<bool(const std::unique_ptr<Resource>&, const std::unique_ptr<Resource>&)>& comp) {
    std::sort(children.begin(), children.end(), comp);
    for (auto& child : children) {
        if (child->isDirectory()) {
            dynamic_cast<Directory*>(child.get())->sortChildren(comp);
        }
    }
}

bool Directory::removeResource(const std::string& targetName, AccessLevel userLevel) {
    // Ищем элемент среди прямых потомков
    auto it = std::find_if(children.begin(), children.end(),
                           [&targetName](const std::unique_ptr<Resource>& res) {
                               return res->getName() == targetName;
                           });

    if (it != children.end()) {
        // Проверка ACL: уровень пользователя должен быть >= уровня текущей (родительской) папки
        if (userLevel < this->level) {
            throw FileSystemException("Отказано в доступе: недостаточно прав для удаления из папки '" + getName() + "'");
        }
        // Удаляем из вектора. unique_ptr автоматически вызовет деструкторы всей вложенной ветки
        children.erase(it); 
        return true;
    }

    // Если не нашли, ищем рекурсивно в подпапках
    for (auto& child : children) {
        if (child->isDirectory()) {
            auto dir = dynamic_cast<Directory*>(child.get());
            if (dir->removeResource(targetName, userLevel)) {
                return true;
            }
        }
    }
    return false;
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
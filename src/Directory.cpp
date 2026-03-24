#include "Directory.h" // Заголовок
#include "FileSystemException.h" // Ошибки
#include <iostream> // Вывод
#include <algorithm> // Алгоритмы

Directory::Directory(const std::string& name, AccessLevel accLevel) // Конструктор
    : Resource(name), level(accLevel) {} // Инициализация

AccessLevel Directory::getAccessLevel() const { // Получить права
    return level; // Возврат
} // Конец метода

void Directory::setAccessLevel(AccessLevel newLevel) { // Установить права
    level = newLevel; // Сохранить
} // Конец метода

void Directory::addResource(std::unique_ptr<Resource> resource) { // Добавить ребенка
    if (resource) { // Если не нулл
        children.push_back(std::move(resource)); // Перемещаем в вектор
    } // Конец if
} // Конец метода

const std::vector<std::unique_ptr<Resource>>& Directory::getChildren() const { // Получить детей
    return children; // Возврат
} // Конец метода

void Directory::collectAll(std::vector<const Resource*>& list) const { // Сбор всех
    list.push_back(this); // Добавляем себя
    for (const auto& child : children) { // Цикл по детям
        if (child->isDirectory()) { // Если папка
            dynamic_cast<const Directory*>(child.get())->collectAll(list); // Рекурсия
        } else { // Если файл
            list.push_back(child.get()); // Добавляем
        } // Конец if
    } // Конец цикла
} // Конец метода

void Directory::sortChildren(const std::function<bool(const std::unique_ptr<Resource>&, const std::unique_ptr<Resource>&)>& comp) { // Сортировка
    std::sort(children.begin(), children.end(), comp); // Сортируем текущий уровень
    for (auto& child : children) { // Цикл
        if (child->isDirectory()) { // Если папка
            dynamic_cast<Directory*>(child.get())->sortChildren(comp); // Сортируем внутри рекурсивно
        } // Конец if
    } // Конец цикла
} // Конец метода

bool Directory::removeResource(const std::string& targetName, AccessLevel userLevel) { // Удаление
    auto it = std::find_if(children.begin(), children.end(), [&targetName](const std::unique_ptr<Resource>& res) { // Поиск
        return res->getName() == targetName; // Сравнение
    }); // Конец поиска
    if (it != children.end()) { // Если нашли
        if (userLevel < this->level) throw FileSystemException("Недостаточно прав для удаления"); // ACL
        children.erase(it); // Удаление
        return true; // Успех
    } // Конец if
    for (auto& child : children) { // Рекурсия
        if (child->isDirectory()) { // Если папка
            if (dynamic_cast<Directory*>(child.get())->removeResource(targetName, userLevel)) return true; // Ищем глубже
        } // Конец if
    } // Конец цикла
    return false; // Не нашли
} // Конец метода

std::unique_ptr<Resource> Directory::detachResource(const std::string& targetName, AccessLevel userLevel) { // Отрыв
    auto it = std::find_if(children.begin(), children.end(), [&targetName](const std::unique_ptr<Resource>& res) { // Поиск
        return res->getName() == targetName; // Сравнение
    }); // Конец поиска
    if (it != children.end()) { // Если нашли
        if (userLevel < this->level) throw FileSystemException("Отказано в доступе"); // ACL
        auto detached = std::move(*it); // Забираем права
        children.erase(it); // Удаляем из вектора
        return detached; // Возвращаем
    } // Конец if
    for (auto& child : children) { // Рекурсия
        if (child->isDirectory()) { // Если папка
            auto detached = dynamic_cast<Directory*>(child.get())->detachResource(targetName, userLevel); // Ищем глубже
            if (detached) return detached; // Возвращаем
        } // Конец if
    } // Конец цикла
    return nullptr; // Не нашли
} // Конец метода

Directory* Directory::findDirectory(const std::string& targetName) { // Поиск папки
    if (this->getName() == targetName) return this; // Нашли себя
    for (const auto& child : children) { // Цикл
        if (child->isDirectory()) { // Если папка
            auto found = dynamic_cast<Directory*>(child.get())->findDirectory(targetName); // Ищем внутри
            if (found) return found; // Возвращаем
        } // Конец if
    } // Конец цикла
    return nullptr; // Не нашли
} // Конец метода

const Resource* Directory::findResource(const std::string& targetName) const { // НОВОЕ: Поиск ресурса для клонирования
    auto it = std::find_if(children.begin(), children.end(), [&targetName](const std::unique_ptr<Resource>& res) { // Поиск
        return res->getName() == targetName; // Сравнение
    }); // Конец поиска
    if (it != children.end()) return it->get(); // Возвращаем указатель
    for (const auto& child : children) { // Цикл
        if (child->isDirectory()) { // Если папка
            auto found = dynamic_cast<const Directory*>(child.get())->findResource(targetName); // Ищем внутри
            if (found) return found; // Возвращаем
        } // Конец if
    } // Конец цикла
    return nullptr; // Не нашли
} // Конец метода

size_t Directory::calculateSize() const { // Подсчет размера
    size_t total = 0; // Сумма
    for (const auto& child : children) total += child->calculateSize(); // Рекурсивное сложение
    return total; // Возврат
} // Конец метода

void Directory::print(int depth) const { // Печать
    std::string indent(depth * 2, ' '); // Отступ
    std::cout << indent << "+ [Dir] " << getName() << " (Access: " << static_cast<int>(level) << ")\n"; // Имя
    for (const auto& child : children) child->print(depth + 1); // Дети
} // Конец метода

bool Directory::isDirectory() const { // Флаг
    return true; // Папка
} // Конец метода

std::unique_ptr<Resource> Directory::clone() const { // НОВОЕ: Рекурсивное клонирование ветки
    auto copy = std::make_unique<Directory>(getName(), level); // Копия папки
    for (const auto& child : children) { // Цикл по детям
        copy->addResource(child->clone()); // Рекурсивный клон каждого ребенка
    } // Конец цикла
    return copy; // Возврат клона
} // Конец метода
#include "Directory.h" // Заголовок директории
#include "FileSystemException.h" // Исключения системы
#include <iostream> // Поток вывода
#include <algorithm> // Алгоритмы (find_if, sort)

Directory::Directory(const std::string& name, AccessLevel accLevel) // Реализация конструктора
    : Resource(name), level(accLevel) {} // Вызов конструктора предка и инициализация прав

AccessLevel Directory::getAccessLevel() const { // Геттер уровня доступа
    return level; // Возврат уровня
} // Конец метода

void Directory::setAccessLevel(AccessLevel newLevel) { // Сеттер уровня
    level = newLevel; // Запись уровня
} // Конец метода

void Directory::addResource(std::unique_ptr<Resource> resource) { // Добавление элемента в вектор
    if (resource) { // Если передан не нулевой указатель
        children.push_back(std::move(resource)); // Перемещаем владение указателем в конец вектора
    } // Конец if
} // Конец метода

const std::vector<std::unique_ptr<Resource>>& Directory::getChildren() const { // Геттер детей (для записи в файл)
    return children; // Возвращаем константную ссылку на вектор
} // Конец метода

void Directory::collectAll(std::vector<const Resource*>& list) const { // Сбор всех вложенных элементов в один плоский список
    list.push_back(this); // Добавляем саму себя в список
    for (const auto& child : children) { // Проходим по всем прямым детям
        if (child->isDirectory()) { // Если ребенок это папка
            dynamic_cast<const Directory*>(child.get())->collectAll(list); // Кастуем к папке и вызываем сбор рекурсивно
        } else { // Если это файл
            list.push_back(child.get()); // Просто добавляем в список
        } // Конец if
    } // Конец цикла
} // Конец метода

void Directory::sortChildren(const std::function<bool(const std::unique_ptr<Resource>&, const std::unique_ptr<Resource>&)>& comp) { // Сортировка
    std::sort(children.begin(), children.end(), comp); // Сортируем прямых детей с помощью переданного правила (функтора)
    for (auto& child : children) { // Пробегаемся по детям
        if (child->isDirectory()) { // Если встретили папку
            dynamic_cast<Directory*>(child.get())->sortChildren(comp); // Запускаем сортировку внутри нее рекурсивно
        } // Конец if
    } // Конец цикла
} // Конец метода

bool Directory::removeResource(const std::string& targetName, AccessLevel userLevel) { // Удаление элемента с проверкой ACL
    auto it = std::find_if(children.begin(), children.end(), // Ищем элемент в векторе с помощью алгоритма
                           [&targetName](const std::unique_ptr<Resource>& res) { // Лямбда-функция проверки
                               return res->getName() == targetName; // Вернет true, если имена совпали
                           }); // Конец поиска

    if (it != children.end()) { // Если элемент найден в текущей папке
        if (userLevel < this->level) { // Проверяем, хватает ли прав на удаление
            throw FileSystemException("Отказано в доступе: недостаточно прав для удаления"); // Бросаем исключение
        } // Конец if
        children.erase(it);  // Стираем элемент вектора (unique_ptr автоматически очистит память каскадно)
        return true; // Сигнализируем об успехе
    } // Конец if

    for (auto& child : children) { // Если не нашли, ищем глубже по дереву
        if (child->isDirectory()) { // Если ребенок - папка
            auto dir = dynamic_cast<Directory*>(child.get()); // Приводим к типу Directory
            if (dir->removeResource(targetName, userLevel)) { // Запускаем рекурсивное удаление
                return true; // Если где-то в глубине удалилось - выходим с успехом
            } // Конец if
        } // Конец if
    } // Конец цикла
    return false; // Элемент вообще не найден
} // Конец метода

std::unique_ptr<Resource> Directory::detachResource(const std::string& targetName, AccessLevel userLevel) { // Отсоединение ресурса для перемещения
    auto it = std::find_if(children.begin(), children.end(), [&targetName](const std::unique_ptr<Resource>& res) { // Ищем элемент
        return res->getName() == targetName; // Сравниваем имена
    }); // Конец поиска
    if (it != children.end()) { // Если элемент найден
        if (userLevel < this->level) { // Защита от перемещения файла без прав
            throw FileSystemException("Отказано в доступе при извлечении из папки '" + getName() + "'"); // Ошибка
        } // Конец if
        auto detached = std::move(*it); // Забираем права на управление памятью (std::move)
        children.erase(it); // Удаляем пустую ячейку из вектора родителя
        return detached; // Возвращаем умный указатель
    } // Конец if
    for (auto& child : children) { // Погружаемся в дочерние папки
        if (child->isDirectory()) { // Если папка
            auto dir = dynamic_cast<Directory*>(child.get()); // Безопасное приведение
            auto detached = dir->detachResource(targetName, userLevel); // Рекурсивный поиск на отрыв
            if (detached) return detached; // Если нашли и оторвали - пробрасываем наверх
        } // Конец if
    } // Конец цикла
    return nullptr; // Если обошли всё и не нашли - возвращаем пустой указатель
} // Конец метода

Directory* Directory::findDirectory(const std::string& targetName) { // Поиск папки (для получения целевой папки при перемещении)
    if (this->getName() == targetName) return this; // Если текущая папка - искомая, возвращаем себя
    for (const auto& child : children) { // Идем по детям
        if (child->isDirectory()) { // Если папка
            auto dir = dynamic_cast<Directory*>(child.get()); // Кастуем
            auto found = dir->findDirectory(targetName); // Ищем рекурсивно
            if (found) return found; // Нашли внутри - возвращаем
        } // Конец if
    } // Конец цикла
    return nullptr; // Не нашли такую папку
} // Конец метода

size_t Directory::calculateSize() const { // Подсчет размера всей ветки
    size_t totalSize = 0; // Накопитель
    for (const auto& child : children) { // Обходим прямых детей
        totalSize += child->calculateSize(); // Суммируем их размеры (у папок рекурсивно)
    } // Конец цикла
    return totalSize; // Возврат суммы
} // Конец метода

void Directory::print(int depth) const { // Вывод папки в консоль
    std::string indent(depth * 2, ' '); // Отступ
    std::cout << indent << "+ [Dir] " << getName() // Маркер папки и имя
              << " (Access: " << static_cast<int>(level) << ")\n"; // Уровень доступа
    for (const auto& child : children) { // Обход детей
        child->print(depth + 1); // Рекурсивный вывод с увеличенным отступом
    } // Конец цикла
} // Конец метода

bool Directory::isDirectory() const { // Проверка на папку
    return true; // Папка является папкой, возвращаем true
} // Конец метода
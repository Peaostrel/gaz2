#ifndef DIRECTORY_H // Защита от двойного включения
#define DIRECTORY_H // Макрос

#include "Resource.h" // Заголовок базового класса
#include "AccessLevel.h" // Заголовок уровней прав
#include <vector> // Подключаем динамические массивы
#include <memory> // Подключаем умные указатели std::unique_ptr
#include <functional> // Подключаем библиотеку для функторов сортировки

class Directory : public Resource { // Класс директории, наследуется от ресурса
private: // Скрытые поля
    AccessLevel level; // Уровень доступа к папке (ACL)
    std::vector<std::unique_ptr<Resource>> children; // Вектор умных указателей на дочерние элементы (решает проблему утечек памяти)

public: // Публичный интерфейс
    Directory(const std::string& name, AccessLevel accLevel = AccessLevel::USER); // Конструктор с именем и правами по умолчанию

    AccessLevel getAccessLevel() const; // Геттер уровня доступа
    void setAccessLevel(AccessLevel newLevel); // Сеттер уровня доступа

    void addResource(std::unique_ptr<Resource> resource); // Добавление нового ресурса (через std::move)
    const std::vector<std::unique_ptr<Resource>>& getChildren() const; // Получение списка детей для сериализации

    void collectAll(std::vector<const Resource*>& list) const; // Рекурсивный сборщик всех элементов для поиска и CSV
    void sortChildren(const std::function<bool(const std::unique_ptr<Resource>&, const std::unique_ptr<Resource>&)>& comp); // Рекурсивная сортировка функтором
    
    bool removeResource(const std::string& targetName, AccessLevel userLevel); // Рекурсивное удаление элемента
    std::unique_ptr<Resource> detachResource(const std::string& targetName, AccessLevel userLevel); // Новое: отрыв указателя для перемещения (std::move)
    Directory* findDirectory(const std::string& targetName); // Новое: поиск папки по имени для перемещения

    size_t calculateSize() const override; // Рекурсивный расчет размера папки
    void print(int depth = 0) const override; // Рекурсивный вывод дерева папки
    bool isDirectory() const override; // Флаг папки
}; // Конец класса

#endif // DIRECTORY_H // Конец защиты
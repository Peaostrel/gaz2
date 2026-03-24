#ifndef DIRECTORY_H // Защита
#define DIRECTORY_H // Макрос

#include "Resource.h" // База
#include "AccessLevel.h" // Права
#include <vector> // Векторы
#include <memory> // Умные указатели
#include <functional> // Функторы

class Directory : public Resource { // Класс папки
private: // Скрытое
    AccessLevel level; // Права
    std::vector<std::unique_ptr<Resource>> children; // Вектор детей

public: // Интерфейс
    Directory(const std::string& name, AccessLevel accLevel = AccessLevel::USER); // Конструктор

    AccessLevel getAccessLevel() const; // Геттер прав
    void setAccessLevel(AccessLevel newLevel); // Сеттер прав

    void addResource(std::unique_ptr<Resource> resource); // Добавление
    const std::vector<std::unique_ptr<Resource>>& getChildren() const; // Получение массива детей

    void collectAll(std::vector<const Resource*>& list) const; // Сбор в плоский массив
    void sortChildren(const std::function<bool(const std::unique_ptr<Resource>&, const std::unique_ptr<Resource>&)>& comp); // Сортировка
    
    bool removeResource(const std::string& targetName, AccessLevel userLevel); // Удаление
    std::unique_ptr<Resource> detachResource(const std::string& targetName, AccessLevel userLevel); // Отрыв для перемещения
    Directory* findDirectory(const std::string& targetName); // Поиск папки
    const Resource* findResource(const std::string& targetName) const; // НОВОЕ: Поиск любого ресурса (указатель)

    size_t calculateSize() const override; // Размер ветки
    void print(int depth = 0) const override; // Печать дерева
    bool isDirectory() const override; // Флаг папки
    
    std::unique_ptr<Resource> clone() const override; // НОВОЕ: Рекурсивное клонирование
}; // Конец класса

#endif // DIRECTORY_H // Защита
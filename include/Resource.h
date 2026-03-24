#ifndef RESOURCE_H // Защита от двойного включения
#define RESOURCE_H // Макрос

#include <string> // Библиотека строк
#include <ctime> // Библиотека времени
#include <memory> // Библиотека умных указателей

class Resource { // Базовый абстрактный класс
private: // Приватные поля
    std::string name; // Имя ресурса
    std::time_t creationDate; // Дата создания

public: // Публичные методы
    Resource(const std::string& name); // Конструктор
    virtual ~Resource() = default; // Виртуальный деструктор

    Resource(const Resource&) = delete; // Запрет копирования по умолчанию
    Resource& operator=(const Resource&) = delete; // Запрет присваивания

    const std::string& getName() const; // Геттер имени
    void setName(const std::string& newName); // Сеттер имени
    std::time_t getCreationDate() const; // Геттер даты
    void updateCreationDate(); // Обновление даты на текущую

    virtual size_t calculateSize() const = 0; // Подсчет размера (чисто виртуальный)
    virtual void print(int depth = 0) const = 0; // Печать в консоль (чисто виртуальный)
    virtual bool isDirectory() const = 0; // Проверка на папку (чисто виртуальный)
    
    virtual std::unique_ptr<Resource> clone() const = 0; // НОВОЕ: Метод для глубокого клонирования (Паттерн Прототип)
}; // Конец класса

#endif // RESOURCE_H // Конец защиты
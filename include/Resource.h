#ifndef RESOURCE_H // Защита от двойного включения файла
#define RESOURCE_H // Установка макроса

#include <string> // Подключение библиотеки строк
#include <ctime> // Подключение библиотеки для работы со временем (дата создания)

class Resource { // Объявление базового абстрактного класса
private: // Приватная секция (инкапсуляция)
    std::string name; // Поле для хранения имени ресурса
    std::time_t creationDate; // Поле для хранения времени создания в формате Unix timestamp

public: // Публичный интерфейс
    Resource(const std::string& name); // Конструктор, принимающий имя ресурса
    virtual ~Resource() = default; // Виртуальный деструктор по умолчанию для корректного удаления наследников

    Resource(const Resource&) = delete; // Явный запрет конструктора копирования (ресурс уникален)
    Resource& operator=(const Resource&) = delete; // Явный запрет оператора присваивания копированием

    const std::string& getName() const; // Геттер для получения имени (возвращает константную ссылку)
    void setName(const std::string& newName); // Сеттер для установки нового имени (с проверкой)
    std::time_t getCreationDate() const; // Геттер для получения даты создания
    void updateCreationDate(); // Метод для обновления даты создания на текущую

    virtual size_t calculateSize() const = 0; // Чисто виртуальный метод подсчета размера (делает класс абстрактным)
    virtual void print(int depth = 0) const = 0; // Чисто виртуальный метод вывода в консоль (с отступами)
    virtual bool isDirectory() const = 0; // Чисто виртуальный метод проверки, является ли объект папкой
}; // Конец класса

#endif // RESOURCE_H // Конец защиты
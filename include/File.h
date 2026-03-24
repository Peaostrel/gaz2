#ifndef FILE_H // Защита от повторного включения
#define FILE_H // Макрос защиты

#include "Resource.h" // Подключаем базовый класс ресурса
#include <string> // Подключаем библиотеку строк

class File : public Resource { // Класс File наследуется от базового Resource
private: // Скрытые поля файла
    std::string extension; // Метаданные: расширение файла
    size_t size; // Метаданные: размер файла в байтах

public: // Публичные методы файла
    File(const std::string& name, const std::string& ext, size_t fileSize = 0); // Конструктор с именем, расширением и размером

    const std::string& getExtension() const; // Геттер для получения расширения
    void setExtension(const std::string& ext); // Сеттер для установки расширения (с проверкой)
    
    size_t getSize() const; // Геттер для получения размера файла
    void setSize(size_t newSize); // Сеттер для изменения размера

    size_t calculateSize() const override; // Переопределение виртуального метода подсчета размера
    void print(int depth = 0) const override; // Переопределение метода вывода в консоль
    bool isDirectory() const override; // Переопределение флага проверки на папку
}; // Конец класса

#endif // FILE_H // Конец защиты
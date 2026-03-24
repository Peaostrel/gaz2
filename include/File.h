#ifndef FILE_H // Защита
#define FILE_H // Макрос

#include "Resource.h" // Базовый класс
#include <string> // Строки

class File : public Resource { // Класс файла
private: // Скрытые поля
    std::string extension; // Расширение
    size_t size; // Заявленный размер
    std::string content; // Текст внутри файла

public: // Методы
    File(const std::string& name, const std::string& ext, size_t fileSize = 0); // Конструктор

    const std::string& getExtension() const; // Геттер расширения
    void setExtension(const std::string& ext); // Сеттер расширения
    
    size_t getSize() const; // Геттер размера
    void setSize(size_t newSize); // Сеттер размера

    const std::string& getContent() const; // Геттер контента
    void setContent(const std::string& text); // Сеттер контента

    size_t calculateSize() const override; // Расчет размера
    void print(int depth = 0) const override; // Печать
    bool isDirectory() const override; // Флаг папки
    
    std::unique_ptr<Resource> clone() const override; // НОВОЕ: Переопределение клонирования
}; // Конец класса

#endif // FILE_H // Защита
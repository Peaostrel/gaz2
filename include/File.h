#ifndef FILE_H // Защита от двойного включения
#define FILE_H // Макрос защиты

#include "Resource.h" // Подключаем базовый ресурс
#include <string> // Библиотека строк

class File : public Resource { // Класс файла, наследник ресурса
private: // Приватные поля
    std::string extension; // Расширение файла
    size_t size; // Заявленный размер файла
    std::string content; // НОВОЕ: Поле для хранения реального текста внутри файла

public: // Публичные методы
    File(const std::string& name, const std::string& ext, size_t fileSize = 0); // Конструктор

    const std::string& getExtension() const; // Геттер расширения
    void setExtension(const std::string& ext); // Сеттер расширения
    
    size_t getSize() const; // Геттер размера
    void setSize(size_t newSize); // Сеттер размера

    const std::string& getContent() const; // НОВОЕ: Геттер текстового содержимого
    void setContent(const std::string& text); // НОВОЕ: Сеттер текстового содержимого

    size_t calculateSize() const override; // Подсчет размера (с учетом текста)
    void print(int depth = 0) const override; // Печать в консоль
    bool isDirectory() const override; // Флаг папки
}; // Конец класса

#endif // FILE_H // Конец защиты
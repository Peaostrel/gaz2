#ifndef FILESYSTEMEXCEPTION_H // Защита от двойного включения
#define FILESYSTEMEXCEPTION_H // Установка макроса

#include <exception> // Подключение стандартных исключений
#include <string> // Подключение библиотеки строк

class FileSystemException : public std::exception { // Класс кастомных ошибок ФС
private: // Скрытая секция
    std::string message; // Сообщение об ошибке

public: // Публичный интерфейс
    explicit FileSystemException(const std::string& msg) : message(msg) {} // Конструктор с текстом ошибки
    
    const char* what() const noexcept override { // Переопределение стандартного метода
        return message.c_str(); // Возврат C-строки
    } // Конец метода
}; // Конец класса

#endif // FILESYSTEMEXCEPTION_H // Конец защиты
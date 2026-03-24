#ifndef FILESYSTEMEXCEPTION_H // Защита от множетельного включения заголовка
#define FILESYSTEMEXCEPTION_H // Объявление макроса защиты

#include <exception> // Подключаем библиотеку стандартных исключений C++
#include <string> // Подключаем библиотеку для работы со строками std::string

class FileSystemException : public std::exception { // Создаем кастомный класс ошибки, наследуя от std::exception
private: // Скрытая секция класса
    std::string message; // Поле для хранения текста конкретной ошибки

public: // Публичная секция класса
    explicit FileSystemException(const std::string& msg) : message(msg) {} // Конструктор, принимающий текст ошибки и сохраняющий его в message
    
    const char* what() const noexcept override { // Переопределяем стандартный метод возврата текста ошибки (не бросает исключений)
        return message.c_str(); // Возвращаем строку в формате C-string (const char*)
    } // Конец метода what
}; // Конец объявления класса

#endif // FILESYSTEMEXCEPTION_H (Конец защиты)
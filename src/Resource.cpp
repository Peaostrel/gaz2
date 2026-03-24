#include "Resource.h" // Подключаем заголовок базового класса
#include "FileSystemException.h" // Подключаем класс наших кастомных исключений
#include <regex> // Подключаем библиотеку регулярных выражений для проверки имени

Resource::Resource(const std::string& name) { // Реализация конструктора
    setName(name); // Вызываем сеттер для установки имени (там вшита проверка)
    updateCreationDate(); // Фиксируем системное время в момент создания объекта
} // Конец конструктора

const std::string& Resource::getName() const { // Реализация геттера имени
    return name; // Возвращаем текущее имя ресурса
} // Конец метода

void Resource::setName(const std::string& newName) { // Реализация сеттера имени
    std::regex invalidChars("[\\\\/:*?\"<>|]"); // Создаем паттерн регулярного выражения с запрещенными символами Windows
    if (std::regex_search(newName, invalidChars)) { // Если в новом имени найден хотя бы один запрещенный символ
        throw FileSystemException("Недопустимые символы в имени ресурса: " + newName); // Выбрасываем ошибку валидации
    } // Конец проверки символов
    if (newName.empty()) { // Если передана пустая строка
        throw FileSystemException("Имя ресурса не может быть пустым"); // Выбрасываем ошибку пустого имени
    } // Конец проверки пустоты
    name = newName; // Если всё отлично, сохраняем новое имя в приватное поле
} // Конец метода

std::time_t Resource::getCreationDate() const { // Реализация геттера даты
    return creationDate; // Возвращаем сохраненный timestamp
} // Конец метода

void Resource::updateCreationDate() { // Реализация обновления времени
    creationDate = std::time(nullptr); // Запрашиваем текущее системное время и сохраняем его
} // Конец метода
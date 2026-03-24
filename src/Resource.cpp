#include "Resource.h" // Подключение заголовка
#include "FileSystemException.h" // Подключение ошибок
#include <regex> // Подключение регулярных выражений

Resource::Resource(const std::string& name) { // Конструктор
    setName(name); // Установка имени с валидацией
    updateCreationDate(); // Установка текущего времени
} // Конец конструктора

const std::string& Resource::getName() const { // Получение имени
    return name; // Возврат
} // Конец метода

void Resource::setName(const std::string& newName) { // Установка имени
    std::regex invalidChars("[\\\\/:*?\"<>|]"); // Регулярка для запрещенных символов
    if (std::regex_search(newName, invalidChars)) { // Проверка
        throw FileSystemException("Недопустимые символы в имени: " + newName); // Ошибка
    } // Конец if
    if (newName.empty()) { // Проверка на пустоту
        throw FileSystemException("Имя не может быть пустым"); // Ошибка
    } // Конец if
    name = newName; // Присваивание
} // Конец метода

std::time_t Resource::getCreationDate() const { // Получение даты
    return creationDate; // Возврат
} // Конец метода

void Resource::updateCreationDate() { // Обновление времени
    creationDate = std::time(nullptr); // Взятие системного времени
} // Конец метода
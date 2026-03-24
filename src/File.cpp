#include "File.h" // Заголовок
#include "FileSystemException.h" // Ошибки
#include <iostream> // Вывод
#include <regex> // Регулярки

File::File(const std::string& name, const std::string& ext, size_t fileSize) // Конструктор
    : Resource(name), size(fileSize), content("") { // Инициализация
    setExtension(ext); // Проверка расширения
} // Конец конструктора

const std::string& File::getExtension() const { // Получить расширение
    return extension; // Возврат
} // Конец метода

void File::setExtension(const std::string& ext) { // Установить расширение
    std::regex validExt("^[a-zA-Z0-9]*$"); // Только буквы и цифры
    if (!std::regex_match(ext, validExt)) { // Если не совпало
        throw FileSystemException("Некорректный формат расширения: " + ext); // Ошибка
    } // Конец if
    extension = ext; // Сохраняем
} // Конец метода

size_t File::getSize() const { // Получить размер
    return size; // Возврат
} // Конец метода

void File::setSize(size_t newSize) { // Установить размер
    size = newSize; // Сохраняем
} // Конец метода

const std::string& File::getContent() const { // Получить текст
    return content; // Возврат
} // Конец метода

void File::setContent(const std::string& text) { // Установить текст
    content = text; // Сохраняем
} // Конец метода

size_t File::calculateSize() const { // Подсчет размера
    return size + content.length(); // Базовый размер + длина текста
} // Конец метода

void File::print(int depth) const { // Печать
    std::string indent(depth * 2, ' '); // Отступы
    std::cout << indent << "- [File] " << getName(); // Имя
    if (!extension.empty()) { // Если есть расширение
        std::cout << "." << extension; // Печатаем
    } // Конец if
    std::cout << " (" << calculateSize() << " bytes)\n"; // Вес
} // Конец метода

bool File::isDirectory() const { // Проверка
    return false; // Это не папка
} // Конец метода

std::unique_ptr<Resource> File::clone() const { // НОВОЕ: Метод клонирования
    auto copy = std::make_unique<File>(getName(), extension, size); // Создаем копию
    copy->setContent(content); // Копируем текст
    return copy; // Возвращаем умный указатель
} // Конец метода
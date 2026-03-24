#include "File.h" // Заголовок
#include "FileSystemException.h" // Ошибки
#include <iostream> // Вывод
#include <regex> // Регулярки

File::File(const std::string& name, const std::string& ext, size_t fileSize) // Конструктор
    : Resource(name), size(fileSize), content("") { // Вызов предка и инициализация полей (контент пуст)
    setExtension(ext); // Проверка и установка расширения
} // Конец конструктора

const std::string& File::getExtension() const { // Получить расширение
    return extension; // Возврат
} // Конец метода

void File::setExtension(const std::string& ext) { // Установить расширение
    std::regex validExt("^[a-zA-Z0-9]*$"); // Только буквы и цифры
    if (!std::regex_match(ext, validExt)) { // Если не совпало
        throw FileSystemException("Некорректный формат расширения файла: " + ext); // Ошибка
    } // Конец if
    extension = ext; // Сохраняем
} // Конец метода

size_t File::getSize() const { // Получить заявленный размер
    return size; // Возврат
} // Конец метода

void File::setSize(size_t newSize) { // Установить размер
    size = newSize; // Сохраняем
} // Конец метода

const std::string& File::getContent() const { // Получить текст файла
    return content; // Возвращаем строку
} // Конец метода

void File::setContent(const std::string& text) { // Записать текст в файл
    content = text; // Сохраняем
} // Конец метода

size_t File::calculateSize() const { // Подсчет веса
    return size + content.length(); // Реальный размер = заявленный байтаж + длина строки контента
} // Конец метода

void File::print(int depth) const { // Печать
    std::string indent(depth * 2, ' '); // Отступы
    std::cout << indent << "- [File] " << getName(); // Имя
    if (!extension.empty()) { // Если есть формат
        std::cout << "." << extension; // Выводим
    } // Конец if
    std::cout << " (" << calculateSize() << " bytes)\n"; // Выводим актуальный размер
} // Конец метода

bool File::isDirectory() const { // Проверка на тип
    return false; // Это не папка
} // Конец метода
#include "File.h" // Подключаем заголовок файла
#include "FileSystemException.h" // Подключаем исключения
#include <iostream> // Подключаем поток ввода-вывода
#include <regex> // Подключаем регулярные выражения для проверки

File::File(const std::string& name, const std::string& ext, size_t fileSize) // Инициализация конструктора
    : Resource(name), size(fileSize) { // Вызываем конструктор базового класса и инициализируем размер
    setExtension(ext); // Проверяем и устанавливаем расширение через сеттер
} // Конец конструктора

const std::string& File::getExtension() const { // Реализация геттера расширения
    return extension; // Возвращаем расширение
} // Конец метода

void File::setExtension(const std::string& ext) { // Реализация сеттера расширения
    std::regex validExt("^[a-zA-Z0-9]*$"); // Регулярка: только латинские буквы и цифры, без точек
    if (!std::regex_match(ext, validExt)) { // Проверяем строку на соответствие правилу
        throw FileSystemException("Некорректный формат расширения файла: " + ext); // Если не совпало - ошибка
    } // Конец проверки
    extension = ext; // Сохраняем проверенное расширение
} // Конец метода

size_t File::getSize() const { // Реализация геттера размера
    return size; // Возвращаем размер
} // Конец метода

void File::setSize(size_t newSize) { // Реализация сеттера размера
    size = newSize; // Обновляем размер
} // Конец метода

size_t File::calculateSize() const { // Подсчет размера (для файла он равен его собственному размеру)
    return size; // Возвращаем размер файла
} // Конец метода

void File::print(int depth) const { // Метод вывода дерева в консоль
    std::string indent(depth * 2, ' '); // Генерируем отступы пробелами в зависимости от глубины
    std::cout << indent << "- [File] " << getName(); // Печатаем маркер, имя
    if (!extension.empty()) { // Если есть расширение
        std::cout << "." << extension; // Печатаем точку и расширение
    } // Конец if
    std::cout << " (" << size << " bytes)\n"; // Печатаем размер и перенос строки
} // Конец метода

bool File::isDirectory() const { // Проверка на папку
    return false; // Файл не является папкой, возвращаем false
} // Конец метода
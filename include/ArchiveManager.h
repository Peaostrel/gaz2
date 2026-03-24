#ifndef ARCHIVEMANAGER_H // Защита
#define ARCHIVEMANAGER_H // Макрос

#include "Directory.h" // Каталоги
#include "File.h" // Файлы
#include "AccessLevel.h" // Уровни доступа
#include <memory> // Умные указатели
#include <string> // Строки
#include <fstream> // Файловые потоки
#include <cstdint> // Фиксированные типы целых чисел

class ArchiveManager { // Менеджер управления всем деревом
private: // Скрытая механика
    std::unique_ptr<Directory> root; // Указатель на корень виртуальной ФС
    AccessLevel currentUserLevel; // Текущий уровень прав пользователя
    const uint32_t MAGIC_NUMBER = 0xFEEDBEEF; // Уникальное число для валидации бинарника архива

    void logOperation(const std::string& operation, bool success, const std::string& details) const; // Функция логирования в текстовый файл
    void serializeResource(const Resource* res, std::ofstream& out) const; // Рекурсивная побайтовая запись
    std::unique_ptr<Resource> deserializeResource(std::ifstream& in) const; // Рекурсивное чтение байтов в дерево
    void writeString(std::ofstream& out, const std::string& str) const; // Запись строки (длина + символы)
    std::string readString(std::ifstream& in) const; // Чтение строки

public: // Интерфейс программы
    ArchiveManager(); // Конструктор

    void setCurrentUserLevel(AccessLevel level); // Установка прав
    AccessLevel getCurrentUserLevel() const; // Получение прав

    void addDirectory(const std::string& name, AccessLevel level); // Добавить папку в корень
    void addFile(const std::string& name, const std::string& ext, size_t size); // Добавить файл в корень

    void printTree() const; // Вывод всего дерева
    void globalAudit() const; // Вывод статистики

    void saveToFile(const std::string& filename) const; // Инициировать сериализацию
    void loadFromFile(const std::string& filename); // Инициировать десериализацию

    void searchByMask(const std::string& maskStr) const; // Поиск по Regex
    void sortResources(int criteria); // Сортировка функторами
    void exportToCSV(const std::string& filename) const; // Выгрузка в таблицу
    
    void deleteResource(const std::string& name); // Удаление узла
    void moveResource(const std::string& resName, const std::string& destDirName); // Новое: перемещение узла
}; // Конец класса

#endif // ARCHIVEMANAGER_H // Защита
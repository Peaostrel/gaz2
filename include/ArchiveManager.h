#ifndef ARCHIVEMANAGER_H // Защита
#define ARCHIVEMANAGER_H // Макрос

#include "Directory.h" // Папки
#include "File.h" // Файлы
#include "AccessLevel.h" // Права
#include <memory> // Указатели
#include <string> // Строки
#include <fstream> // Потоки файлов
#include <cstdint> // Типы интов

class ArchiveManager { // Менеджер архива
private: // Скрытое
    std::unique_ptr<Directory> root; // Корень
    AccessLevel currentUserLevel; // Текущие права
    const uint32_t MAGIC_NUMBER = 0xFEEDBEEF; // Заголовок бинарника

    void logOperation(const std::string& operation, bool success, const std::string& details) const; // Логгер
    void serializeResource(const Resource* res, std::ofstream& out) const; // Запись узла
    std::unique_ptr<Resource> deserializeResource(std::ifstream& in) const; // Чтение узла
    void writeString(std::ofstream& out, const std::string& str) const; // Запись строки
    std::string readString(std::ifstream& in) const; // Чтение строки

public: // Доступное меню
    ArchiveManager(); // Конструктор

    void setCurrentUserLevel(AccessLevel level); // Сеттер прав
    AccessLevel getCurrentUserLevel() const; // Геттер прав

    void addDirectory(const std::string& name, AccessLevel level); // Создать папку
    void addFile(const std::string& name, const std::string& ext, size_t size, const std::string& content); // Изменено: добавлен контент при создании

    void printTree() const; // Дерево
    void globalAudit() const; // Аудит

    void saveToFile(const std::string& filename) const; // В файл
    void loadFromFile(const std::string& filename); // Из файла

    void searchByMask(const std::string& maskStr) const; // Поиск regex
    void sortResources(int criteria); // Сортировка
    void exportToCSV(const std::string& filename) const; // Выгрузка
    
    void deleteResource(const std::string& name); // Удаление
    void moveResource(const std::string& resName, const std::string& destDirName); // Перемещение
    
    // НОВОЕ: Поиск текста внутри файлов указанной папки
    void searchContentInDir(const std::string& dirName, const std::string& query) const; 
}; // Конец класса

#endif // ARCHIVEMANAGER_H // Конец защиты
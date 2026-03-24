#ifndef ARCHIVEMANAGER_H // Защита
#define ARCHIVEMANAGER_H // Макрос

#include "Directory.h" // Папки
#include "File.h" // Файлы
#include "AccessLevel.h" // Права
#include <memory> // Указатели
#include <string> // Строки
#include <fstream> // Потоки файлов
#include <cstdint> // Типы

class ArchiveManager { // Менеджер
private: // Скрытое
    std::unique_ptr<Directory> root; // Корень
    AccessLevel currentUserLevel; // Текущие права
    const uint32_t MAGIC_NUMBER = 0xFEEDBEEF; // Магическое число

    void logOperation(const std::string& operation, bool success, const std::string& details) const; // Логгер
    void serializeResource(const Resource* res, std::ofstream& out) const; // Запись узла
    std::unique_ptr<Resource> deserializeResource(std::ifstream& in) const; // Чтение узла
    void writeString(std::ofstream& out, const std::string& str) const; // Запись строки
    std::string readString(std::ifstream& in) const; // Чтение строки

public: // Интерфейс
    ArchiveManager(); // Конструктор

    void setCurrentUserLevel(AccessLevel level); // Сеттер прав
    AccessLevel getCurrentUserLevel() const; // Геттер прав

    void addDirectory(const std::string& name, AccessLevel level); // Добавить папку
    void addFile(const std::string& name, const std::string& ext, size_t size, const std::string& content); // Добавить файл

    void printTree() const; // Вывод дерева
    void globalAudit() const; // Глобальный аудит (ОБНОВЛЕНО ПО ТЗ)

    void saveToFile(const std::string& filename) const; // Сохранение
    void loadFromFile(const std::string& filename); // Загрузка

    void searchByMask(const std::string& maskStr) const; // Поиск маской
    void sortResources(int criteria); // Сортировка
    void exportToCSV(const std::string& filename) const; // Экспорт
    
    void deleteResource(const std::string& name); // Удаление
    void moveResource(const std::string& resName, const std::string& destDirName); // Перемещение
    void searchContentInDir(const std::string& dirName, const std::string& query) const; // Поиск текста
    
    void copyResource(const std::string& resName, const std::string& destDirName); // НОВОЕ: Глубокое копирование
    void filterByDate(const std::string& startDateStr, const std::string& endDateStr) const; // НОВОЕ: Фильтрация по дате
}; // Конец класса

#endif // ARCHIVEMANAGER_H // Защита
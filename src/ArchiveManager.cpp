#include "ArchiveManager.h"
#include "FileSystemException.h"
#include <iostream>
#include <fstream>
#include <chrono>
#include <ctime>

ArchiveManager::ArchiveManager() : currentUserLevel(AccessLevel::ADMIN) {
    // Инициализация корня
    root = std::make_unique<Directory>("root", AccessLevel::GUEST);
    logOperation("INIT", true, "Инициализация корневого каталога");
}

void ArchiveManager::setCurrentUserLevel(AccessLevel level) {
    currentUserLevel = level;
    logOperation("AUTH", true, "Изменен уровень доступа текущего пользователя");
}

AccessLevel ArchiveManager::getCurrentUserLevel() const {
    return currentUserLevel;
}

void ArchiveManager::logOperation(const std::string& operation, bool success, const std::string& details) const {
    std::ofstream logFile("history.log", std::ios::app);
    if (logFile.is_open()) {
        auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        std::string timeStr = std::ctime(&now);
        timeStr.pop_back(); // Убираем символ новой строки из ctime
        
        logFile << "[" << timeStr << "] " << operation 
                << " | Статус: " << (success ? "УСПЕХ" : "ОШИБКА") 
                << " | " << details << "\n";
    }
}

void ArchiveManager::addDirectory(const std::string& name, AccessLevel level) {
    try {
        auto newDir = std::make_unique<Directory>(name, level);
        root->addResource(std::move(newDir));
        logOperation("CREATE_DIR", true, "Создана папка: " + name);
    } catch (const FileSystemException& e) {
        logOperation("CREATE_DIR", false, e.what());
        throw; // Пробрасываем выше для обработки в main
    }
}

void ArchiveManager::addFile(const std::string& name, const std::string& ext, size_t size) {
    try {
        auto newFile = std::make_unique<File>(name, ext, size);
        root->addResource(std::move(newFile));
        logOperation("CREATE_FILE", true, "Создан файл: " + name + "." + ext);
    } catch (const FileSystemException& e) {
        logOperation("CREATE_FILE", false, e.what());
        throw;
    }
}

void ArchiveManager::printTree() const {
    std::cout << "--- Структура виртуального архива ---\n";
    root->print();
    std::cout << "-------------------------------------\n";
}

void ArchiveManager::globalAudit() const {
    size_t totalSize = root->calculateSize();
    std::cout << "\n=== Глобальный аудит ===\n";
    std::cout << "Общий размер хранилища: " << totalSize << " байт\n";
    std::cout << "Текущий уровень прав: " << static_cast<int>(currentUserLevel) << "\n";
    std::cout << "========================\n\n";
    logOperation("AUDIT", true, "Запрошен глобальный аудит. Размер: " + std::to_string(totalSize));
}
#include "ArchiveManager.h"
#include "FileSystemException.h"
#include <iostream>
#include <chrono>
#include <ctime>
#include <regex>

// Функторы для сортировки
struct SortByName {
    bool operator()(const std::unique_ptr<Resource>& a, const std::unique_ptr<Resource>& b) const {
        return a->getName() < b->getName();
    }
};

struct SortBySize {
    bool operator()(const std::unique_ptr<Resource>& a, const std::unique_ptr<Resource>& b) const {
        return a->calculateSize() < b->calculateSize();
    }
};

struct SortByDate {
    bool operator()(const std::unique_ptr<Resource>& a, const std::unique_ptr<Resource>& b) const {
        return a->getCreationDate() < b->getCreationDate();
    }
};

ArchiveManager::ArchiveManager() : currentUserLevel(AccessLevel::ADMIN) {
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
        timeStr.pop_back();
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
        throw;
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

void ArchiveManager::searchByMask(const std::string& maskStr) const {
    std::cout << "\n=== Результаты поиска по маске: " << maskStr << " ===\n";
    try {
        std::regex mask(maskStr);
        std::vector<const Resource*> allRes;
        root->collectAll(allRes);
        
        bool found = false;
        for (const auto& res : allRes) {
            if (std::regex_search(res->getName(), mask)) {
                std::cout << (res->isDirectory() ? "[Dir] " : "[File] ") 
                          << res->getName() << " (" << res->calculateSize() << " bytes)\n";
                found = true;
            }
        }
        if (!found) std::cout << "Ничего не найдено.\n";
        logOperation("SEARCH", true, "Поиск по маске: " + maskStr);
    } catch (const std::regex_error&) {
        throw FileSystemException("Некорректное регулярное выражение для поиска.");
    }
}

void ArchiveManager::sortResources(int criteria) {
    if (criteria == 1) {
        root->sortChildren(SortByName());
        std::cout << "[+] Сортировка по имени применена.\n";
    } else if (criteria == 2) {
        root->sortChildren(SortBySize());
        std::cout << "[+] Сортировка по размеру применена.\n";
    } else if (criteria == 3) {
        root->sortChildren(SortByDate());
        std::cout << "[+] Сортировка по дате применена.\n";
    } else {
        throw FileSystemException("Неизвестный критерий сортировки.");
    }
    logOperation("SORT", true, "Изменен режим сортировки: " + std::to_string(criteria));
}

void ArchiveManager::writeString(std::ofstream& out, const std::string& str) const {
    size_t len = str.length();
    out.write(reinterpret_cast<const char*>(&len), sizeof(len));
    out.write(str.c_str(), len);
}

std::string ArchiveManager::readString(std::ifstream& in) const {
    size_t len;
    in.read(reinterpret_cast<char*>(&len), sizeof(len));
    std::string str(len, '\0');
    in.read(&str[0], len);
    return str;
}

void ArchiveManager::serializeResource(const Resource* res, std::ofstream& out) const {
    bool isDir = res->isDirectory();
    out.write(reinterpret_cast<const char*>(&isDir), sizeof(isDir));
    writeString(out, res->getName());
    std::time_t cDate = res->getCreationDate();
    out.write(reinterpret_cast<const char*>(&cDate), sizeof(cDate));

    if (isDir) {
        const Directory* dir = dynamic_cast<const Directory*>(res);
        AccessLevel lvl = dir->getAccessLevel();
        out.write(reinterpret_cast<const char*>(&lvl), sizeof(lvl));
        const auto& children = dir->getChildren();
        size_t childCount = children.size();
        out.write(reinterpret_cast<const char*>(&childCount), sizeof(childCount));
        for (const auto& child : children) {
            serializeResource(child.get(), out);
        }
    } else {
        const File* file = dynamic_cast<const File*>(res);
        writeString(out, file->getExtension());
        size_t size = file->getSize();
        out.write(reinterpret_cast<const char*>(&size), sizeof(size));
    }
}

std::unique_ptr<Resource> ArchiveManager::deserializeResource(std::ifstream& in) const {
    bool isDir;
    in.read(reinterpret_cast<char*>(&isDir), sizeof(isDir));
    std::string name = readString(in);
    std::time_t cDate;
    in.read(reinterpret_cast<char*>(&cDate), sizeof(cDate));

    if (isDir) {
        AccessLevel lvl;
        in.read(reinterpret_cast<char*>(&lvl), sizeof(lvl));
        auto dir = std::make_unique<Directory>(name, lvl);
        size_t childCount;
        in.read(reinterpret_cast<char*>(&childCount), sizeof(childCount));
        for (size_t i = 0; i < childCount; ++i) {
            dir->addResource(deserializeResource(in));
        }
        return dir;
    } else {
        std::string ext = readString(in);
        size_t size;
        in.read(reinterpret_cast<char*>(&size), sizeof(size));
        return std::make_unique<File>(name, ext, size);
    }
}

void ArchiveManager::saveToFile(const std::string& filename) const {
    std::ofstream out(filename, std::ios::binary);
    if (!out) throw FileSystemException("Не удалось открыть файл для записи: " + filename);
    out.write(reinterpret_cast<const char*>(&MAGIC_NUMBER), sizeof(MAGIC_NUMBER));
    serializeResource(root.get(), out);
    logOperation("SAVE", true, "Архив сохранен в " + filename);
}

void ArchiveManager::loadFromFile(const std::string& filename) {
    std::ifstream in(filename, std::ios::binary);
    if (!in) throw FileSystemException("Не удалось открыть файл для чтения: " + filename);
    uint32_t magic;
    in.read(reinterpret_cast<char*>(&magic), sizeof(magic));
    if (magic != MAGIC_NUMBER) {
        throw FileSystemException("Нарушена целостность данных! Неверное магическое число.");
    }
    auto newRoot = deserializeResource(in);
    if (newRoot && newRoot->isDirectory()) {
        root = std::unique_ptr<Directory>(static_cast<Directory*>(newRoot.release()));
        logOperation("LOAD", true, "Архив загружен из " + filename);
    } else {
        throw FileSystemException("Ошибка структуры при загрузке архива.");
    }
}
#define _CRT_SECURE_NO_WARNINGS // Фикс предупреждений MSVC
#include "ArchiveManager.h" // Заголовок
#include "FileSystemException.h" // Ошибки
#include <iostream> // Вывод
#include <chrono> // Время
#include <ctime> // Системное время
#include <regex> // Регулярки
#include <iomanip> // Манипуляторы ввода/вывода для парсинга дат
#include <sstream> // Потоки строк для дат

struct SortByName { // Функтор
    bool operator()(const std::unique_ptr<Resource>& a, const std::unique_ptr<Resource>& b) const { return a->getName() < b->getName(); } // Сравнение
}; // Конец
struct SortBySize { // Функтор
    bool operator()(const std::unique_ptr<Resource>& a, const std::unique_ptr<Resource>& b) const { return a->calculateSize() < b->calculateSize(); } // Сравнение
}; // Конец
struct SortByDate { // Функтор
    bool operator()(const std::unique_ptr<Resource>& a, const std::unique_ptr<Resource>& b) const { return a->getCreationDate() < b->getCreationDate(); } // Сравнение
}; // Конец

ArchiveManager::ArchiveManager() : currentUserLevel(AccessLevel::ADMIN) { // Конструктор
    root = std::make_unique<Directory>("root", AccessLevel::GUEST); // Корень
    logOperation("INIT", true, "Инициализация"); // Лог
} // Конец метода

void ArchiveManager::setCurrentUserLevel(AccessLevel level) { // Права
    currentUserLevel = level; // Установка
    logOperation("AUTH", true, "Смена прав"); // Лог
} // Конец метода

AccessLevel ArchiveManager::getCurrentUserLevel() const { // Геттер
    return currentUserLevel; // Возврат
} // Конец метода

void ArchiveManager::logOperation(const std::string& operation, bool success, const std::string& details) const { // Логгер
    std::ofstream logFile("history.log", std::ios::app); // Файл
    if (logFile.is_open()) { // Проверка
        auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()); // Время
        std::string timeStr = std::ctime(&now); // Конвертация
        timeStr.pop_back(); // Убрать перенос
        logFile << "[" << timeStr << "] " << operation << " | " << (success ? "УСПЕХ" : "ОШИБКА") << " | " << details << "\n"; // Запись
    } // Конец if
} // Конец метода

void ArchiveManager::addDirectory(const std::string& name, AccessLevel level) { // Папка
    try { // Перехват
        auto newDir = std::make_unique<Directory>(name, level); // Указатель
        root->addResource(std::move(newDir)); // Вставка
        logOperation("CREATE_DIR", true, name); // Лог
    } catch (const FileSystemException& e) { // Ловушка
        logOperation("CREATE_DIR", false, e.what()); // Лог
        throw; // Бросок
    } // Конец catch
} // Конец метода

void ArchiveManager::addFile(const std::string& name, const std::string& ext, size_t size, const std::string& content) { // Файл
    try { // Перехват
        auto newFile = std::make_unique<File>(name, ext, size); // Память
        if (content != "-") newFile->setContent(content); // Текст
        root->addResource(std::move(newFile)); // Вставка
        logOperation("CREATE_FILE", true, name); // Лог
    } catch (const FileSystemException& e) { // Ловушка
        logOperation("CREATE_FILE", false, e.what()); // Лог
        throw; // Бросок
    } // Конец catch
} // Конец метода

void ArchiveManager::printTree() const { // Дерево
    std::cout << "--- Структура ---\n"; // Шапка
    root->print(); // Вызов
    std::cout << "-----------------\n"; // Подвал
} // Конец метода

void ArchiveManager::globalAudit() const { // Глобальный аудит
    std::vector<const Resource*> allRes; // Вектор
    root->collectAll(allRes); // Сбор всех узлов
    
    size_t fileCount = 0; // Счетчик файлов
    size_t dirCount = 0; // Счетчик папок
    size_t totalFileSize = 0; // Сумма веса
    
    for (const auto& res : allRes) { // Цикл
        if (res->isDirectory()) { // Если папка
            dirCount++; // Плюс папка
        } else { // Если файл
            fileCount++; // Плюс файл
            totalFileSize += res->calculateSize(); // Плюс размер
        } // Конец if
    } // Конец цикла
    
    size_t avgSize = (fileCount > 0) ? (totalFileSize / fileCount) : 0; // Расчет среднего размера
    
    std::cout << "\n=== Глобальный аудит ===\n"; // Шапка
    std::cout << "Общее количество папок: " << dirCount << "\n"; // Вывод папок
    std::cout << "Общее количество файлов: " << fileCount << "\n"; // Вывод файлов
    std::cout << "Общий объем файлов: " << totalFileSize << " байт\n"; // Вывод объема
    std::cout << "Средний размер файла: " << avgSize << " байт\n"; // Вывод среднего
    std::cout << "========================\n\n"; // Подвал
    logOperation("AUDIT", true, "Собран аудит"); // Лог
} // Конец метода

void ArchiveManager::searchByMask(const std::string& maskStr) const { // Regex
    try { // Блок
        std::regex mask(maskStr); // Компиляция
        std::vector<const Resource*> allRes; // Вектор
        root->collectAll(allRes); // Сбор
        bool found = false; // Флаг
        for (const auto& res : allRes) { // Обход
            if (std::regex_search(res->getName(), mask)) { // Если совпало
                std::cout << (res->isDirectory() ? "[Dir] " : "[File] ") << res->getName() << " (" << res->calculateSize() << " bytes)\n"; // Вывод
                found = true; // Нашли
            } // Конец if
        } // Конец цикла
        if (!found) std::cout << "Ничего не найдено.\n"; // Пусто
        logOperation("SEARCH", true, maskStr); // Лог
    } catch (const std::regex_error&) { throw FileSystemException("Некорректное регулярное выражение."); } // Ошибка
} // Конец метода

void ArchiveManager::searchContentInDir(const std::string& dirName, const std::string& query) const { // Поиск текста
    Directory* targetDir = (dirName == root->getName()) ? root.get() : root->findDirectory(dirName); // Поиск папки
    if (!targetDir) throw FileSystemException("Директория не найдена."); // Ошибка
    std::vector<const Resource*> allRes; // Вектор
    targetDir->collectAll(allRes); // Сбор
    bool found = false; // Флаг
    for (const auto& res : allRes) { // Цикл
        if (!res->isDirectory()) { // Только файлы
            const File* f = dynamic_cast<const File*>(res); // Каст
            if (f->getContent().find(query) != std::string::npos) { // Ищем
                std::cout << "[Найден текст] в файле: " << f->getName() << "." << f->getExtension() << "\n"; // Вывод
                found = true; // Нашли
            } // Конец if
        } // Конец if
    } // Конец цикла
    if (!found) std::cout << "Текст не найден.\n"; // Глухо
    logOperation("CONTENT_SEARCH", true, query); // Лог
} // Конец метода

void ArchiveManager::sortResources(int criteria) { // Сортировка
    if (criteria == 1) root->sortChildren(SortByName()); // Имя
    else if (criteria == 2) root->sortChildren(SortBySize()); // Размер
    else if (criteria == 3) root->sortChildren(SortByDate()); // Дата
    else throw FileSystemException("Неизвестный критерий."); // Ошибка
    std::cout << "[+] Сортировка применена.\n"; // Успех
} // Конец метода

void ArchiveManager::exportToCSV(const std::string& filename) const { // CSV
    std::ofstream out(filename); // Файл
    if (!out) throw FileSystemException("Ошибка открытия CSV."); // Ошибка
    out << "Name,Type,Size,CreationDate\n"; // Шапка
    std::vector<const Resource*> allRes; // Вектор
    root->collectAll(allRes); // Сбор
    for (const auto& res : allRes) { // Цикл
        std::time_t cDate = res->getCreationDate(); // Дата
        char timeBuf[80]; // Буфер
        std::strftime(timeBuf, sizeof(timeBuf), "%Y-%m-%d %H:%M:%S", std::localtime(&cDate)); // Формат
        out << res->getName() << "," << (res->isDirectory() ? "Directory" : "File") << "," << res->calculateSize() << "," << timeBuf << "\n"; // Вывод
    } // Конец цикла
    std::cout << "[+] Выгружено в " << filename << "\n"; // Успех
} // Конец метода

void ArchiveManager::writeString(std::ofstream& out, const std::string& str) const { // Запись строки
    size_t len = str.length(); // Длина
    out.write(reinterpret_cast<const char*>(&len), sizeof(len)); // Длина байт
    out.write(str.c_str(), len); // Строка
} // Конец метода

std::string ArchiveManager::readString(std::ifstream& in) const { // Чтение строки
    size_t len; // Переменная
    in.read(reinterpret_cast<char*>(&len), sizeof(len)); // Читаем длину
    std::string str(len, '\0'); // Выделяем
    in.read(&str[0], len); // Читаем байты
    return str; // Возврат
} // Конец метода

void ArchiveManager::serializeResource(const Resource* res, std::ofstream& out) const { // Сериализация
    bool isDir = res->isDirectory(); // Тип
    out.write(reinterpret_cast<const char*>(&isDir), sizeof(isDir)); // Тип в байты
    writeString(out, res->getName()); // Имя
    std::time_t cDate = res->getCreationDate(); // Дата
    out.write(reinterpret_cast<const char*>(&cDate), sizeof(cDate)); // Дата в байты
    if (isDir) { // Папка
        const Directory* dir = dynamic_cast<const Directory*>(res); // Каст
        AccessLevel lvl = dir->getAccessLevel(); // Права
        out.write(reinterpret_cast<const char*>(&lvl), sizeof(lvl)); // Запись
        const auto& children = dir->getChildren(); // Дети
        size_t childCount = children.size(); // Кол-во
        out.write(reinterpret_cast<const char*>(&childCount), sizeof(childCount)); // Запись
        for (const auto& child : children) serializeResource(child.get(), out); // Рекурсия
    } else { // Файл
        const File* file = dynamic_cast<const File*>(res); // Каст
        writeString(out, file->getExtension()); // Формат
        size_t size = file->getSize(); // Вес
        out.write(reinterpret_cast<const char*>(&size), sizeof(size)); // Запись
        writeString(out, file->getContent()); // Текст
    } // Конец if
} // Конец метода

std::unique_ptr<Resource> ArchiveManager::deserializeResource(std::ifstream& in) const { // Десериализация
    bool isDir; // Флаг
    in.read(reinterpret_cast<char*>(&isDir), sizeof(isDir)); // Читаем тип
    std::string name = readString(in); // Имя
    std::time_t cDate; // Дата
    in.read(reinterpret_cast<char*>(&cDate), sizeof(cDate)); // Дата
    if (isDir) { // Папка
        AccessLevel lvl; // Права
        in.read(reinterpret_cast<char*>(&lvl), sizeof(lvl)); // Читаем
        auto dir = std::make_unique<Directory>(name, lvl); // Создаем
        size_t childCount; // Количество
        in.read(reinterpret_cast<char*>(&childCount), sizeof(childCount)); // Читаем
        for (size_t i = 0; i < childCount; ++i) dir->addResource(deserializeResource(in)); // Рекурсия
        return dir; // Возврат
    } else { // Файл
        std::string ext = readString(in); // Формат
        size_t size; // Вес
        in.read(reinterpret_cast<char*>(&size), sizeof(size)); // Читаем
        auto file = std::make_unique<File>(name, ext, size); // Создаем
        file->setContent(readString(in)); // Текст
        return file; // Возврат
    } // Конец if
} // Конец метода

void ArchiveManager::saveToFile(const std::string& filename) const { // Сохранение
    std::ofstream out(filename, std::ios::binary); // Поток
    if (!out) throw FileSystemException("Ошибка записи."); // Ошибка
    out.write(reinterpret_cast<const char*>(&MAGIC_NUMBER), sizeof(MAGIC_NUMBER)); // Магическое число
    serializeResource(root.get(), out); // Старт
} // Конец метода

void ArchiveManager::loadFromFile(const std::string& filename) { // Загрузка
    std::ifstream in(filename, std::ios::binary); // Поток
    if (!in) throw FileSystemException("Ошибка чтения."); // Ошибка
    uint32_t magic; // Хэдер
    in.read(reinterpret_cast<char*>(&magic), sizeof(magic)); // Читаем
    if (magic != MAGIC_NUMBER) throw FileSystemException("Неверное магическое число."); // Сверяем
    auto newRoot = deserializeResource(in); // Читаем дерево
    if (newRoot && newRoot->isDirectory()) root = std::unique_ptr<Directory>(static_cast<Directory*>(newRoot.release())); // Замещаем
    else throw FileSystemException("Ошибка структуры."); // Ошибка
} // Конец метода

void ArchiveManager::deleteResource(const std::string& name) { // Удаление
    if (name == root->getName()) throw FileSystemException("Корень удалить нельзя!"); // Защита
    if (root->removeResource(name, currentUserLevel)) std::cout << "[+] Удалено.\n"; // Успех
    else throw FileSystemException("Не найдено."); // Ошибка
} // Конец метода

void ArchiveManager::moveResource(const std::string& resName, const std::string& destDirName) { // Перемещение
    if (resName == root->getName() || resName == destDirName) throw FileSystemException("Ошибка путей."); // Защита
    Directory* destDir = root->findDirectory(destDirName); // Цель
    if (!destDir) throw FileSystemException("Папка назначения не найдена."); // Ошибка
    if (currentUserLevel < destDir->getAccessLevel()) throw FileSystemException("Нет прав записи."); // Права
    auto detached = root->detachResource(resName, currentUserLevel); // Отрыв
    if (!detached) throw FileSystemException("Ресурс не найден."); // Ошибка
    destDir->addResource(std::move(detached)); // Перенос
    std::cout << "[+] Перемещено.\n"; // Успех
} // Конец метода

void ArchiveManager::copyResource(const std::string& resName, const std::string& destDirName) { // Глубокое копирование
    if (resName == root->getName()) throw FileSystemException("Нельзя скопировать системный корень."); // Защита
    Directory* destDir = (destDirName == root->getName()) ? root.get() : root->findDirectory(destDirName); // Цель
    if (!destDir) throw FileSystemException("Папка назначения не найдена."); // Ошибка
    if (currentUserLevel < destDir->getAccessLevel()) throw FileSystemException("Нет прав записи."); // ACL
    
    const Resource* sourceRes = nullptr; // Указатель
    auto it = std::find_if(root->getChildren().begin(), root->getChildren().end(), [&resName](const std::unique_ptr<Resource>& r) { return r->getName() == resName; }); // Ищем в корне
    if (it != root->getChildren().end()) sourceRes = it->get(); // Нашли в корне
    else sourceRes = root->findResource(resName); // Ищем глубже
    
    if (!sourceRes) throw FileSystemException("Исходный ресурс не найден."); // Не нашли
    
    destDir->addResource(sourceRes->clone()); // Паттерн Прототип в действии
    std::cout << "[+] Ветка архива успешно скопирована физически.\n"; // Вывод
} // Конец метода

void ArchiveManager::filterByDate(const std::string& startDateStr, const std::string& endDateStr) const { // Фильтрация по дате
    // ЖЕЛЕЗОБЕТОННАЯ ЗАЩИТА: Строгий Regex (4 цифры - 2 цифры - 2 цифры)
    std::regex datePattern("^\\d{4}-\\d{2}-\\d{2}$"); 
    if (!std::regex_match(startDateStr, datePattern) || !std::regex_match(endDateStr, datePattern)) {
        throw FileSystemException("Некорректный формат! Введите строго YYYY-MM-DD (например, 2026-03-24).");
    }

    std::tm startTm = {}; // Структура времени начала
    std::tm endTm = {}; // Структура времени конца
    std::istringstream ssStart(startDateStr); // Поток для парсинга
    std::istringstream ssEnd(endDateStr); // Поток для парсинга
    
    ssStart >> std::get_time(&startTm, "%Y-%m-%d"); // Читаем формат
    ssEnd >> std::get_time(&endTm, "%Y-%m-%d"); // Читаем формат
    
    // Проверка логических ошибок (например, 2026-13-45)
    if (ssStart.fail() || ssEnd.fail()) throw FileSystemException("Логическая ошибка в дате."); 
    
    endTm.tm_hour = 23; endTm.tm_min = 59; endTm.tm_sec = 59; // Конец дня включительно
    
    std::time_t start = std::mktime(&startTm); // В Unix timestamp
    std::time_t end = std::mktime(&endTm); // В Unix timestamp
    
    std::vector<const Resource*> allRes; // Вектор
    root->collectAll(allRes); // Собираем всех
    
    std::cout << "\n=== Ресурсы, созданные с " << startDateStr << " по " << endDateStr << " ===\n"; // Шапка
    bool found = false; // Флаг
    for (const auto& res : allRes) { // Пробег
        std::time_t cDate = res->getCreationDate(); // Берем дату
        if (cDate >= start && cDate <= end) { // Сравниваем
            char timeBuf[80]; // Буфер
            std::strftime(timeBuf, sizeof(timeBuf), "%Y-%m-%d %H:%M:%S", std::localtime(&cDate)); // Формат
            std::cout << (res->isDirectory() ? "[Dir] " : "[File] ") << res->getName() << " (Создан: " << timeBuf << ")\n"; // Вывод
            found = true; // Нашли
        } // Конец if
    } // Конец цикла
    if (!found) std::cout << "Ничего не найдено в этом диапазоне.\n"; // Пусто
} // Конец метода
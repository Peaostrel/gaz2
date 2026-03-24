#include "ArchiveManager.h" // Заголовок менеджера
#include "FileSystemException.h" // Ошибки
#include <iostream> // Вывод
#include <chrono> // Время
#include <ctime> // Системные часы
#include <regex> // Регулярки

struct SortByName { // Функтор имени
    bool operator()(const std::unique_ptr<Resource>& a, const std::unique_ptr<Resource>& b) const { // Сравнение
        return a->getName() < b->getName(); // Строки
    } // Конец функции
}; // Конец структуры

struct SortBySize { // Функтор размера
    bool operator()(const std::unique_ptr<Resource>& a, const std::unique_ptr<Resource>& b) const { // Сравнение
        return a->calculateSize() < b->calculateSize(); // Числа
    } // Конец функции
}; // Конец структуры

struct SortByDate { // Функтор даты
    bool operator()(const std::unique_ptr<Resource>& a, const std::unique_ptr<Resource>& b) const { // Сравнение
        return a->getCreationDate() < b->getCreationDate(); // Даты
    } // Конец функции
}; // Конец структуры

ArchiveManager::ArchiveManager() : currentUserLevel(AccessLevel::ADMIN) { // Конструктор с правами Админа
    root = std::make_unique<Directory>("root", AccessLevel::GUEST); // Корень системы
    logOperation("INIT", true, "Инициализация корневого каталога"); // Лог
} // Конец конструктора

void ArchiveManager::setCurrentUserLevel(AccessLevel level) { // Смена пользователя
    currentUserLevel = level; // Применяем
    logOperation("AUTH", true, "Изменен уровень доступа"); // Логируем
} // Конец метода

AccessLevel ArchiveManager::getCurrentUserLevel() const { // Получить права
    return currentUserLevel; // Возврат
} // Конец метода

void ArchiveManager::logOperation(const std::string& operation, bool success, const std::string& details) const { // Запись лога
    std::ofstream logFile("history.log", std::ios::app); // Открываем файл
    if (logFile.is_open()) { // Проверка
        auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()); // Текущее время
        std::string timeStr = std::ctime(&now); // В строку
        timeStr.pop_back(); // Убрать перенос
        logFile << "[" << timeStr << "] " << operation // Формат записи
                << " | Статус: " << (success ? "УСПЕХ" : "ОШИБКА") // Успех/Фейл
                << " | " << details << "\n"; // Детали
    } // Конец if
} // Конец метода

void ArchiveManager::addDirectory(const std::string& name, AccessLevel level) { // Добавление папки
    try { // Старт
        auto newDir = std::make_unique<Directory>(name, level); // Указатель
        root->addResource(std::move(newDir)); // Вставка
        logOperation("CREATE_DIR", true, "Создана папка: " + name); // Лог
    } catch (const FileSystemException& e) { // Ловушка
        logOperation("CREATE_DIR", false, e.what()); // Лог
        throw; // Бросок
    } // Конец catch
} // Конец метода

void ArchiveManager::addFile(const std::string& name, const std::string& ext, size_t size, const std::string& content) { // Добавление файла (ОБНОВЛЕНО)
    try { // Старт
        auto newFile = std::make_unique<File>(name, ext, size); // Выделяем память
        if (content != "-") newFile->setContent(content); // Если ввели текст (не минус), сохраняем его
        root->addResource(std::move(newFile)); // Вставляем в корень
        logOperation("CREATE_FILE", true, "Создан файл: " + name + "." + ext); // Лог
    } catch (const FileSystemException& e) { // Ловушка
        logOperation("CREATE_FILE", false, e.what()); // Лог ошибки
        throw; // Бросок
    } // Конец catch
} // Конец метода

void ArchiveManager::printTree() const { // Печать дерева
    std::cout << "--- Структура виртуального архива ---\n"; // Шапка
    root->print(); // Вызов
    std::cout << "-------------------------------------\n"; // Подвал
} // Конец метода

void ArchiveManager::globalAudit() const { // Аудит
    size_t totalSize = root->calculateSize(); // Подсчет
    std::cout << "\n=== Глобальный аудит ===\n"; // Шапка
    std::cout << "Общий размер хранилища: " << totalSize << " байт\n"; // Вывод
    std::cout << "Текущий уровень прав: " << static_cast<int>(currentUserLevel) << "\n"; // Права
    std::cout << "========================\n\n"; // Конец
    logOperation("AUDIT", true, "Запрошен глобальный аудит"); // Лог
} // Конец метода

void ArchiveManager::searchByMask(const std::string& maskStr) const { // Regex поиск
    std::cout << "\n=== Результаты поиска по маске: " << maskStr << " ===\n"; // Вывод
    try { // Блок
        std::regex mask(maskStr); // Компиляция
        std::vector<const Resource*> allRes; // Вектор
        root->collectAll(allRes); // Сбор
        
        bool found = false; // Флаг
        for (const auto& res : allRes) { // Обход
            if (std::regex_search(res->getName(), mask)) { // Если совпало
                std::cout << (res->isDirectory() ? "[Dir] " : "[File] ") // Пишем тип
                          << res->getName() << " (" << res->calculateSize() << " bytes)\n"; // Имя
                found = true; // Нашли
            } // Конец if
        } // Конец цикла
        if (!found) std::cout << "Ничего не найдено.\n"; // Пусто
        logOperation("SEARCH", true, "Поиск по маске: " + maskStr); // Лог
    } catch (const std::regex_error&) { // Ловим кривой синтаксис
        throw FileSystemException("Некорректное регулярное выражение."); // Ошибка
    } // Конец catch
} // Конец метода

void ArchiveManager::searchContentInDir(const std::string& dirName, const std::string& query) const { // НОВОЕ: Поиск по содержимому
    std::cout << "\n=== Поиск текста '" << query << "' в папке '" << dirName << "' ===\n"; // Вывод шапки
    
    // Ищем указанную директорию. Если ввели root - берем сам корень
    Directory* targetDir = (dirName == root->getName()) ? root.get() : root->findDirectory(dirName); // Поиск
    
    if (!targetDir) { // Если не нашли
        throw FileSystemException("Указанная директория не найдена."); // Ошибка
    } // Конец if

    std::vector<const Resource*> allRes; // Подготавливаем плоский вектор
    targetDir->collectAll(allRes); // Собираем в него все вложенные элементы целевой папки
    
    bool found = false; // Флаг успешного поиска
    for (const auto& res : allRes) { // Проходимся по всем элементам
        if (!res->isDirectory()) { // Нас интересуют только файлы
            const File* f = dynamic_cast<const File*>(res); // Приводим к типу File
            if (f->getContent().find(query) != std::string::npos) { // Ищем подстроку в контенте (если != npos, значит нашли)
                std::cout << "[Найден текст] в файле: " << f->getName() << "." << f->getExtension() << "\n"; // Выводим имя файла
                found = true; // Ставим флаг
            } // Конец if
        } // Конец if
    } // Конец цикла
    
    if (!found) std::cout << "Текст не найден ни в одном файле.\n"; // Если глухо
    logOperation("CONTENT_SEARCH", true, "Поиск текста в " + dirName); // Логируем
} // Конец метода

void ArchiveManager::sortResources(int criteria) { // Сортировка
    if (criteria == 1) { // 1
        root->sortChildren(SortByName()); // Имя
    } else if (criteria == 2) { // 2
        root->sortChildren(SortBySize()); // Размер
    } else if (criteria == 3) { // 3
        root->sortChildren(SortByDate()); // Дата
    } else { // Иначе
        throw FileSystemException("Неизвестный критерий."); // Ошибка
    } // Конец if
    std::cout << "[+] Сортировка применена.\n"; // Успех
    logOperation("SORT", true, "Сортировка"); // Лог
} // Конец метода

void ArchiveManager::exportToCSV(const std::string& filename) const { // В CSV
    std::ofstream out(filename); // Открываем
    if (!out) throw FileSystemException("Ошибка открытия CSV."); // Ошибка
    
    out << "Name,Type,Size,CreationDate\n"; // Шапка таблицы
    std::vector<const Resource*> allRes; // Вектор
    root->collectAll(allRes); // Собираем
    
    for (const auto& res : allRes) { // Цикл
        std::time_t cDate = res->getCreationDate(); // Берем дату
        char timeBuf[80]; // Буфер
        std::strftime(timeBuf, sizeof(timeBuf), "%Y-%m-%d %H:%M:%S", std::localtime(&cDate)); // Формат
        out << res->getName() << "," // Имя
            << (res->isDirectory() ? "Directory" : "File") << "," // Тип
            << res->calculateSize() << "," // Размер
            << timeBuf << "\n"; // Дата
    } // Конец цикла
    std::cout << "[+] Выгружено в " << filename << "\n"; // Успех
    logOperation("EXPORT", true, "Экспорт"); // Лог
} // Конец метода

void ArchiveManager::writeString(std::ofstream& out, const std::string& str) const { // Пишем строку в бинарник
    size_t len = str.length(); // Длина
    out.write(reinterpret_cast<const char*>(&len), sizeof(len)); // Запись длины
    out.write(str.c_str(), len); // Запись символов
} // Конец метода

std::string ArchiveManager::readString(std::ifstream& in) const { // Читаем строку
    size_t len; // Длина
    in.read(reinterpret_cast<char*>(&len), sizeof(len)); // Читаем длину
    std::string str(len, '\0'); // Выделяем место
    in.read(&str[0], len); // Читаем байты
    return str; // Возвращаем
} // Конец метода

void ArchiveManager::serializeResource(const Resource* res, std::ofstream& out) const { // Сериализация
    bool isDir = res->isDirectory(); // Проверяем папку
    out.write(reinterpret_cast<const char*>(&isDir), sizeof(isDir)); // Пишем байт типа
    writeString(out, res->getName()); // Имя
    std::time_t cDate = res->getCreationDate(); // Дата
    out.write(reinterpret_cast<const char*>(&cDate), sizeof(cDate)); // Запись даты

    if (isDir) { // Если папка
        const Directory* dir = dynamic_cast<const Directory*>(res); // Каст
        AccessLevel lvl = dir->getAccessLevel(); // Права
        out.write(reinterpret_cast<const char*>(&lvl), sizeof(lvl)); // Пишем права
        const auto& children = dir->getChildren(); // Вектор детей
        size_t childCount = children.size(); // Размер вектора
        out.write(reinterpret_cast<const char*>(&childCount), sizeof(childCount)); // Пишем кол-во детей
        for (const auto& child : children) { // Цикл
            serializeResource(child.get(), out); // Рекурсия
        } // Конец цикла
    } else { // Если файл
        const File* file = dynamic_cast<const File*>(res); // Каст
        writeString(out, file->getExtension()); // Пишем расширение
        size_t size = file->getSize(); // Берем размер
        out.write(reinterpret_cast<const char*>(&size), sizeof(size)); // Пишем размер
        writeString(out, file->getContent()); // ОБНОВЛЕНО: Дописываем в бинарник текст внутри файла
    } // Конец if
} // Конец метода

std::unique_ptr<Resource> ArchiveManager::deserializeResource(std::ifstream& in) const { // Десериализация
    bool isDir; // Булево
    in.read(reinterpret_cast<char*>(&isDir), sizeof(isDir)); // Читаем тип
    std::string name = readString(in); // Имя
    std::time_t cDate; // Дата
    in.read(reinterpret_cast<char*>(&cDate), sizeof(cDate)); // Читаем дату

    if (isDir) { // Если папка
        AccessLevel lvl; // Права
        in.read(reinterpret_cast<char*>(&lvl), sizeof(lvl)); // Читаем
        auto dir = std::make_unique<Directory>(name, lvl); // Создаем объект
        size_t childCount; // Количество
        in.read(reinterpret_cast<char*>(&childCount), sizeof(childCount)); // Читаем
        for (size_t i = 0; i < childCount; ++i) { // Цикл
            dir->addResource(deserializeResource(in)); // Рекурсия
        } // Конец цикла
        return dir; // Возвращаем собранную ветку
    } else { // Если файл
        std::string ext = readString(in); // Расширение
        size_t size; // Размер
        in.read(reinterpret_cast<char*>(&size), sizeof(size)); // Читаем
        auto file = std::make_unique<File>(name, ext, size); // Создаем объект
        file->setContent(readString(in)); // ОБНОВЛЕНО: Считываем текст файла из бинарника
        return file; // Возвращаем готовый файл
    } // Конец if
} // Конец метода

void ArchiveManager::saveToFile(const std::string& filename) const { // Сохранение
    std::ofstream out(filename, std::ios::binary); // Открываем поток
    if (!out) throw FileSystemException("Ошибка записи."); // Проверка
    out.write(reinterpret_cast<const char*>(&MAGIC_NUMBER), sizeof(MAGIC_NUMBER)); // Магическое число
    serializeResource(root.get(), out); // Старт записи
    logOperation("SAVE", true, "Сохранено"); // Лог
} // Конец метода

void ArchiveManager::loadFromFile(const std::string& filename) { // Загрузка
    std::ifstream in(filename, std::ios::binary); // Поток
    if (!in) throw FileSystemException("Ошибка чтения."); // Проверка
    uint32_t magic; // Хэдер
    in.read(reinterpret_cast<char*>(&magic), sizeof(magic)); // Читаем
    if (magic != MAGIC_NUMBER) throw FileSystemException("Неверное магическое число."); // Сверяем
    auto newRoot = deserializeResource(in); // Парсим
    if (newRoot && newRoot->isDirectory()) { // Если ок
        root = std::unique_ptr<Directory>(static_cast<Directory*>(newRoot.release())); // Замещаем
    } else { // Иначе
        throw FileSystemException("Ошибка структуры."); // Фейл
    } // Конец if
} // Конец метода

void ArchiveManager::deleteResource(const std::string& name) { // Удаление
    // Реализация не менялась, скрыто комментариями
    if (name == root->getName()) throw FileSystemException("Корень удалить нельзя!"); // Защита
    if (root->removeResource(name, currentUserLevel)) std::cout << "[+] Удалено.\n"; // Рекурсивное удаление
    else throw FileSystemException("Не найдено."); // Фейл
} // Конец метода

void ArchiveManager::moveResource(const std::string& resName, const std::string& destDirName) { // Перемещение
    // Реализация не менялась, скрыто комментариями
    if (resName == root->getName() || resName == destDirName) throw FileSystemException("Ошибка путей."); // Защита
    Directory* destDir = root->findDirectory(destDirName); // Ищем цель
    if (!destDir) throw FileSystemException("Папка назначения не найдена."); // Фейл
    if (currentUserLevel < destDir->getAccessLevel()) throw FileSystemException("Нет прав записи."); // ACL
    auto detached = root->detachResource(resName, currentUserLevel); // Отрываем
    if (!detached) throw FileSystemException("Ресурс не найден."); // Фейл
    destDir->addResource(std::move(detached)); // Перекладываем
    std::cout << "[+] Перемещено.\n"; // Успех
} // Конец метода
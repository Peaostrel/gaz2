#include "ArchiveManager.h" // Заголовок менеджера
#include "FileSystemException.h" // Исключения
#include <iostream> // Вывод
#include <chrono> // Время (миллисекунды)
#include <ctime> // Системное время
#include <regex> // Регулярки

struct SortByName { // Функтор для сортировки по алфавиту
    bool operator()(const std::unique_ptr<Resource>& a, const std::unique_ptr<Resource>& b) const { // Оператор сравнения
        return a->getName() < b->getName(); // Сравнение строк
    } // Конец оператора
}; // Конец структуры

struct SortBySize { // Функтор для сортировки по размеру
    bool operator()(const std::unique_ptr<Resource>& a, const std::unique_ptr<Resource>& b) const { // Оператор сравнения
        return a->calculateSize() < b->calculateSize(); // Сравнение чисел (sizes)
    } // Конец оператора
}; // Конец структуры

struct SortByDate { // Функтор для сортировки по дате создания
    bool operator()(const std::unique_ptr<Resource>& a, const std::unique_ptr<Resource>& b) const { // Оператор сравнения
        return a->getCreationDate() < b->getCreationDate(); // Сравнение времени (timestamp)
    } // Конец оператора
}; // Конец структуры

ArchiveManager::ArchiveManager() : currentUserLevel(AccessLevel::ADMIN) { // Конструктор, по умолчанию даем права Админа для удобства тестов
    root = std::make_unique<Directory>("root", AccessLevel::GUEST); // Создаем системный корень (права GUEST чтобы в него всегда можно было писать)
    logOperation("INIT", true, "Инициализация корневого каталога"); // Пишем в лог старт
} // Конец конструктора

void ArchiveManager::setCurrentUserLevel(AccessLevel level) { // Установить права пользователя
    currentUserLevel = level; // Меняем поле
    logOperation("AUTH", true, "Изменен уровень доступа текущего пользователя"); // Логируем смену
} // Конец метода

AccessLevel ArchiveManager::getCurrentUserLevel() const { // Получить права
    return currentUserLevel; // Возврат
} // Конец метода

void ArchiveManager::logOperation(const std::string& operation, bool success, const std::string& details) const { // Логирование
    std::ofstream logFile("history.log", std::ios::app); // Открываем файл на дозапись (app)
    if (logFile.is_open()) { // Если открылся
        auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()); // Берем время
        std::string timeStr = std::ctime(&now); // Конвертируем в строку
        timeStr.pop_back(); // Удаляем символ переноса строки из ctime
        logFile << "[" << timeStr << "] " << operation // Пишем время и тип операции
                << " | Статус: " << (success ? "УСПЕХ" : "ОШИБКА") // Пишем статус
                << " | " << details << "\n"; // Пишем детали
    } // Конец if
} // Конец метода

void ArchiveManager::addDirectory(const std::string& name, AccessLevel level) { // Добавление папки в корень
    try { // Блок перехвата
        auto newDir = std::make_unique<Directory>(name, level); // Создаем умный указатель
        root->addResource(std::move(newDir)); // Передаем его в корень (move)
        logOperation("CREATE_DIR", true, "Создана папка: " + name); // Лог успеха
    } catch (const FileSystemException& e) { // Ловим ошибки валидации
        logOperation("CREATE_DIR", false, e.what()); // Пишем в лог ошибку
        throw; // Пробрасываем ошибку дальше в main
    } // Конец catch
} // Конец метода

void ArchiveManager::addFile(const std::string& name, const std::string& ext, size_t size) { // Создание файла в корне
    try { // Попытка создания
        auto newFile = std::make_unique<File>(name, ext, size); // Выделяем память
        root->addResource(std::move(newFile)); // Вставляем в корень
        logOperation("CREATE_FILE", true, "Создан файл: " + name + "." + ext); // Логируем
    } catch (const FileSystemException& e) { // Ловим
        logOperation("CREATE_FILE", false, e.what()); // Логируем ошибку
        throw; // Перебрасываем
    } // Конец catch
} // Конец метода

void ArchiveManager::printTree() const { // Вывод интерфейса
    std::cout << "--- Структура виртуального архива ---\n"; // Заголовок
    root->print(); // Запуск рекурсивной печати от корня
    std::cout << "-------------------------------------\n"; // Подвал
} // Конец метода

void ArchiveManager::globalAudit() const { // Вывод статистики
    size_t totalSize = root->calculateSize(); // Подсчет объема
    std::cout << "\n=== Глобальный аудит ===\n"; // Шапка
    std::cout << "Общий размер хранилища: " << totalSize << " байт\n"; // Вывод веса
    std::cout << "Текущий уровень прав: " << static_cast<int>(currentUserLevel) << "\n"; // Вывод прав
    std::cout << "========================\n\n"; // Конец вывода
    logOperation("AUDIT", true, "Запрошен глобальный аудит. Размер: " + std::to_string(totalSize)); // Логируем
} // Конец метода

void ArchiveManager::searchByMask(const std::string& maskStr) const { // Поиск regex
    std::cout << "\n=== Результаты поиска по маске: " << maskStr << " ===\n"; // Шапка
    try { // Старт блока
        std::regex mask(maskStr); // Компилируем регулярку
        std::vector<const Resource*> allRes; // Создаем плоский массив
        root->collectAll(allRes); // Собираем туда все элементы дерева
        
        bool found = false; // Флаг нахождения
        for (const auto& res : allRes) { // Обходим все элементы
            if (std::regex_search(res->getName(), mask)) { // Если имя подходит под маску
                std::cout << (res->isDirectory() ? "[Dir] " : "[File] ") // Выводим тип
                          << res->getName() << " (" << res->calculateSize() << " bytes)\n"; // Имя и размер
                found = true; // Меняем флаг
            } // Конец if
        } // Конец цикла
        if (!found) std::cout << "Ничего не найдено.\n"; // Если не нашли
        logOperation("SEARCH", true, "Поиск по маске: " + maskStr); // Логируем успех
    } catch (const std::regex_error&) { // Ловим кривой синтаксис regex
        throw FileSystemException("Некорректное регулярное выражение для поиска."); // Кидаем свою ошибку
    } // Конец catch
} // Конец метода

void ArchiveManager::sortResources(int criteria) { // Сортировка
    if (criteria == 1) { // По имени
        root->sortChildren(SortByName()); // Вызываем с функтором имени
        std::cout << "[+] Сортировка по имени применена.\n"; // Уведомление
    } else if (criteria == 2) { // По размеру
        root->sortChildren(SortBySize()); // Вызов с функтором размера
        std::cout << "[+] Сортировка по размеру применена.\n"; // Уведомление
    } else if (criteria == 3) { // По дате
        root->sortChildren(SortByDate()); // Вызов с функтором даты
        std::cout << "[+] Сортировка по дате применена.\n"; // Уведомление
    } else { // Если кривой критерий
        throw FileSystemException("Неизвестный критерий сортировки."); // Ошибка
    } // Конец if-else
    logOperation("SORT", true, "Изменен режим сортировки: " + std::to_string(criteria)); // Логируем
} // Конец метода

void ArchiveManager::exportToCSV(const std::string& filename) const { // Выгрузка в CSV
    std::ofstream out(filename); // Открываем текстовый файл
    if (!out) throw FileSystemException("Не удалось открыть файл для CSV экспорта."); // Защита
    
    out << "Name,Type,Size,CreationDate\n"; // Заголовок таблицы
    std::vector<const Resource*> allRes; // Вектор
    root->collectAll(allRes); // Собираем всех
    
    for (const auto& res : allRes) { // Проходимся по элементам
        std::time_t cDate = res->getCreationDate(); // Берем дату
        char timeBuf[80]; // Буфер для текста
        std::strftime(timeBuf, sizeof(timeBuf), "%Y-%m-%d %H:%M:%S", std::localtime(&cDate)); // Форматируем время
        
        out << res->getName() << "," // Пишем ячейку имени
            << (res->isDirectory() ? "Directory" : "File") << "," // Тип
            << res->calculateSize() << "," // Размер
            << timeBuf << "\n"; // Дата
    } // Конец цикла
    std::cout << "[+] Структура успешно выгружена в " << filename << "\n"; // Уведомляем
    logOperation("EXPORT", true, "Экспорт в CSV: " + filename); // В лог
} // Конец метода

void ArchiveManager::writeString(std::ofstream& out, const std::string& str) const { // Побайтовая запись строки
    size_t len = str.length(); // Узнаем длину
    out.write(reinterpret_cast<const char*>(&len), sizeof(len)); // Сначала пишем байты длины
    out.write(str.c_str(), len); // Затем пишем байты самих символов
} // Конец метода

std::string ArchiveManager::readString(std::ifstream& in) const { // Побайтовое чтение строки
    size_t len; // Переменная под длину
    in.read(reinterpret_cast<char*>(&len), sizeof(len)); // Читаем длину в байтах
    std::string str(len, '\0'); // Выделяем место в строке
    in.read(&str[0], len); // Читаем сами символы
    return str; // Возвращаем строку
} // Конец метода

void ArchiveManager::serializeResource(const Resource* res, std::ofstream& out) const { // Сериализация ветки
    bool isDir = res->isDirectory(); // Запоминаем тип
    out.write(reinterpret_cast<const char*>(&isDir), sizeof(isDir)); // Пишем тип в байтах
    writeString(out, res->getName()); // Пишем имя
    std::time_t cDate = res->getCreationDate(); // Берем дату
    out.write(reinterpret_cast<const char*>(&cDate), sizeof(cDate)); // Пишем дату

    if (isDir) { // Если это папка
        const Directory* dir = dynamic_cast<const Directory*>(res); // Кастуем
        AccessLevel lvl = dir->getAccessLevel(); // Получаем права
        out.write(reinterpret_cast<const char*>(&lvl), sizeof(lvl)); // Пишем права
        const auto& children = dir->getChildren(); // Берем детей
        size_t childCount = children.size(); // Считаем количество
        out.write(reinterpret_cast<const char*>(&childCount), sizeof(childCount)); // Пишем количество детей
        for (const auto& child : children) { // Проходим по детям
            serializeResource(child.get(), out); // Рекурсивно сериализуем каждого
        } // Конец цикла
    } else { // Если файл
        const File* file = dynamic_cast<const File*>(res); // Кастуем
        writeString(out, file->getExtension()); // Пишем расширение
        size_t size = file->getSize(); // Берем размер
        out.write(reinterpret_cast<const char*>(&size), sizeof(size)); // Пишем размер
    } // Конец if-else
} // Конец метода

std::unique_ptr<Resource> ArchiveManager::deserializeResource(std::ifstream& in) const { // Десериализация
    bool isDir; // Переменная типа
    in.read(reinterpret_cast<char*>(&isDir), sizeof(isDir)); // Читаем байт типа
    std::string name = readString(in); // Читаем имя
    std::time_t cDate; // Переменная даты
    in.read(reinterpret_cast<char*>(&cDate), sizeof(cDate)); // Читаем дату

    if (isDir) { // Если прочитали что это папка
        AccessLevel lvl; // Права
        in.read(reinterpret_cast<char*>(&lvl), sizeof(lvl)); // Читаем права
        auto dir = std::make_unique<Directory>(name, lvl); // Создаем объект папки
        size_t childCount; // Количество детей
        in.read(reinterpret_cast<char*>(&childCount), sizeof(childCount)); // Читаем количество
        for (size_t i = 0; i < childCount; ++i) { // Цикл по детям
            dir->addResource(deserializeResource(in)); // Рекурсивно читаем и добавляем детей
        } // Конец цикла
        return dir; // Возвращаем собранную ветку
    } else { // Если файл
        std::string ext = readString(in); // Читаем расширение
        size_t size; // Читаем размер
        in.read(reinterpret_cast<char*>(&size), sizeof(size)); // Считываем
        return std::make_unique<File>(name, ext, size); // Возвращаем файл
    } // Конец if-else
} // Конец метода

void ArchiveManager::saveToFile(const std::string& filename) const { // Сохранение файла
    std::ofstream out(filename, std::ios::binary); // Открываем файл как бинарный поток
    if (!out) throw FileSystemException("Не удалось открыть файл для записи."); // Ошибка открытия
    out.write(reinterpret_cast<const char*>(&MAGIC_NUMBER), sizeof(MAGIC_NUMBER)); // Пишем заголовок защиты (magic number)
    serializeResource(root.get(), out); // Запускаем сериализацию с корня
    logOperation("SAVE", true, "Архив сохранен в " + filename); // Пишем в лог
} // Конец метода

void ArchiveManager::loadFromFile(const std::string& filename) { // Чтение файла
    std::ifstream in(filename, std::ios::binary); // Открываем бинарный поток
    if (!in) throw FileSystemException("Не удалось открыть файл для чтения."); // Ошибка
    uint32_t magic; // Переменная для магического числа
    in.read(reinterpret_cast<char*>(&magic), sizeof(magic)); // Читаем заголовок файла
    if (magic != MAGIC_NUMBER) { // Сверяем
        throw FileSystemException("Нарушена целостность данных! Неверное магическое число."); // Ошибка (ловим из-за Hex-редактора)
    } // Конец if
    auto newRoot = deserializeResource(in); // Запускаем загрузку
    if (newRoot && newRoot->isDirectory()) { // Если успешно прочитан корень
        root = std::unique_ptr<Directory>(static_cast<Directory*>(newRoot.release())); // Заменяем старое дерево новым
        logOperation("LOAD", true, "Архив загружен из " + filename); // Логируем успех
    } else { // Ошибка данных
        throw FileSystemException("Ошибка структуры при загрузке архива."); // Ошибка
    } // Конец if-else
} // Конец метода

void ArchiveManager::deleteResource(const std::string& name) { // Метод удаления
    try { // Старт проверки
        if (name == root->getName()) { // Не даем удалить системную папку root
            throw FileSystemException("Невозможно удалить корневой каталог!"); // Ошибка
        } // Конец if
        bool success = root->removeResource(name, currentUserLevel); // Запускаем рекурсивное удаление
        if (success) { // Если найдено и удалено
            logOperation("DELETE", true, "Удален ресурс: " + name); // Лог
            std::cout << "[+] Ресурс '" << name << "' успешно удален.\n"; // Вывод
        } else { // Не нашли
            throw FileSystemException("Ресурс '" + name + "' не найден."); // Ошибка
        } // Конец if-else
    } catch (const FileSystemException& e) { // Ловим
        logOperation("DELETE", false, e.what()); // Пишем
        throw; // Бросаем
    } // Конец catch
} // Конец метода

void ArchiveManager::moveResource(const std::string& resName, const std::string& destDirName) { // Реализация перемещения
    try { // Блок try
        if (resName == root->getName()) throw FileSystemException("Нельзя переместить корень."); // Блок защиты корня
        if (resName == destDirName) throw FileSystemException("Логическая ошибка: рекурсия в себя."); // Защита от бреда
        
        Directory* destDir = root->findDirectory(destDirName); // Ищем объект папки назначения
        if (!destDir) throw FileSystemException("Папка назначения не найдена."); // Ошибка поиска папки
        if (currentUserLevel < destDir->getAccessLevel()) throw FileSystemException("Нет прав записи в папку-получатель."); // Проверка прав папки (ACL)
        
        auto detached = root->detachResource(resName, currentUserLevel); // Отрываем узел от старого места (сдвиг unique_ptr)
        if (!detached) throw FileSystemException("Перемещаемый ресурс не найден."); // Если не нашли что отрывать
        
        destDir->addResource(std::move(detached)); // Передаем владение указателем в новую папку (std::move без копирования)
        logOperation("MOVE", true, resName + " -> " + destDirName); // Логируем перемещение
        std::cout << "[+] Ресурс успешно перемещен.\n"; // Радуем пользователя
    } catch (const FileSystemException& e) { // Ловим ошибки прав или путей
        logOperation("MOVE", false, e.what()); // Пишем фейл
        throw; // Отправляем в UI
    } // Конец catch
} // Конец метода
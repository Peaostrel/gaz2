#include <iostream> // Ввод/вывод
#include <string> // Строки
#include <limits> // Лимиты

#ifdef _WIN32 // ОС Windows
#include <windows.h> // WinAPI
#endif // Конец

#include "ArchiveManager.h" // Менеджер
#include "FileSystemException.h" // Ошибки

void printMenu() { // Меню
    std::cout << "\n=== Меню Виртуального Архива ===\n"; // Шапка
    std::cout << "1. Создать папку\n"; // 1
    std::cout << "2. Создать файл\n"; // 2
    std::cout << "3. Вывести дерево архива\n"; // 3
    std::cout << "4. Глобальный аудит\n"; // 4
    std::cout << "5. Поиск по маске (Regex)\n"; // 5
    std::cout << "6. Сортировка (1-Имя, 2-Размер, 3-Дата)\n"; // 6
    std::cout << "7. Выгрузить в CSV\n"; // 7
    std::cout << "8. Удалить ресурс (рекурсивно)\n"; // 8
    std::cout << "9. Переместить ресурс (std::move)\n"; // 9
    std::cout << "10. Поиск текста внутри файлов папки\n"; // 10 НОВОЕ
    std::cout << "11. Сохранить архив (archive.dat)\n"; // 11
    std::cout << "12. Загрузить архив (archive.dat)\n"; // 12
    std::cout << "0. Выход\n"; // 0
    std::cout << "Выбор: "; // Ввод
} // Конец метода

void clearInput() { // Очистка
    std::cin.clear(); // Сброс
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Игнор
} // Конец метода

int main() { // Точка входа
#ifdef _WIN32 // Windows
    SetConsoleOutputCP(CP_UTF8); // Вывод UTF8
    SetConsoleCP(CP_UTF8); // Ввод UTF8
#else // Unix
    setlocale(LC_ALL, "ru_RU.UTF-8"); // Локаль
#endif // Конец

    ArchiveManager archive; // Инициализация
    archive.setCurrentUserLevel(AccessLevel::ADMIN); // Ставим фулл права
    int choice = -1; // Переменная

    std::cout << "Система запущена. Логирование активно.\n"; // Сообщение

    while (choice != 0) { // Цикл
        printMenu(); // Вывод
        if (!(std::cin >> choice)) { // Чтение
            clearInput(); // Чистка
            std::cout << "Ошибка ввода.\n"; // Ошибка
            continue; // Дальше
        } // Конец if

        try { // Блок перехвата
            switch (choice) { // Выбор
                case 1: { // Папка
                    std::string name; // Имя
                    int level; // Права
                    std::cout << "Имя папки: "; // Вывод
                    std::cin >> name; // Ввод
                    std::cout << "Уровень доступа (0-GUEST, 1-USER, 2-ADMIN): "; // Вывод
                    if (!(std::cin >> level) || level < 0 || level > 2) { // Проверка
                        clearInput(); // Очистка
                        throw FileSystemException("Некорректный уровень."); // Ошибка
                    } // Конец if
                    archive.addDirectory(name, static_cast<AccessLevel>(level)); // Добавляем
                    std::cout << "[+] Создано.\n"; // Успех
                    break; // Конец
                }
                case 2: { // Файл
                    std::string name, ext, content; // Переменные (добавлен контент)
                    size_t size; // Размер
                    std::cout << "Имя файла: "; // Запрос
                    std::cin >> name; // Ввод
                    std::cout << "Расширение (без точки): "; // Запрос
                    std::cin >> ext; // Ввод
                    std::cout << "Размер (байт): "; // Запрос
                    if (!(std::cin >> size)) { // Проверка
                        clearInput(); // Чистка
                        throw FileSystemException("Некорректный размер."); // Ошибка
                    } // Конец if
                    std::cout << "Введите текстовое содержимое (или '-' чтобы оставить пустым): "; // Запрос текста
                    std::cin >> content; // Читаем строку
                    archive.addFile(name, ext, size, content); // ОБНОВЛЕНО: Передаем текст в менеджер
                    std::cout << "[+] Создано.\n"; // Успех
                    break; // Выход
                }
                case 3: archive.printTree(); break; // Дерево
                case 4: archive.globalAudit(); break; // Аудит
                case 5: { // Маска
                    std::string mask; // Строка
                    std::cout << "Regex маска: "; // Запрос
                    std::cin >> mask; // Ввод
                    archive.searchByMask(mask); // Поиск
                    break; // Конец
                }
                case 6: { // Сортировка
                    int criteria; // Выбор
                    std::cout << "Критерий (1-Имя, 2-Размер, 3-Дата): "; // Запрос
                    if (!(std::cin >> criteria) || criteria < 1 || criteria > 3) throw FileSystemException("Ошибка"); // Проверка
                    archive.sortResources(criteria); // Применить
                    break; // Конец
                }
                case 7: archive.exportToCSV("export.csv"); break; // Экспорт
                case 8: { // Удаление
                    std::string name; // Имя
                    std::cout << "Имя для удаления: "; // Вывод
                    std::cin >> name; // Ввод
                    archive.deleteResource(name); // Вызов
                    break; // Конец
                }
                case 9: { // Перемещение
                    std::string res, dir; // Переменные
                    std::cout << "Что перемещаем: "; // Вывод
                    std::cin >> res; // Ввод
                    std::cout << "Куда: "; // Вывод
                    std::cin >> dir; // Ввод
                    archive.moveResource(res, dir); // Вызов
                    break; // Конец
                }
                case 10: { // НОВОЕ: Поиск по контенту
                    std::string dirName, query; // Директория и текст
                    std::cout << "В какой папке искать (введите 'root' для корня): "; // Запрос папки
                    std::cin >> dirName; // Ввод
                    std::cout << "Какой текст ищем: "; // Запрос текста
                    std::cin >> query; // Ввод
                    archive.searchContentInDir(dirName, query); // Запуск метода
                    break; // Конец
                }
                case 11: // Сохранить
                    archive.saveToFile("archive.dat"); // Вызов
                    std::cout << "[OK] Сохранено.\n"; // Успех
                    break; // Конец
                case 12: // Загрузить
                    archive.loadFromFile("archive.dat"); // Вызов
                    std::cout << "[OK] Загружено.\n"; // Успех
                    break; // Конец
                case 0: std::cout << "Выход...\n"; break; // Конец
                default: std::cout << "Неизвестная команда.\n"; break; // Ошибка
            } // Конец switch
        } catch (const FileSystemException& e) { std::cerr << "[ОШИБКА ФС]: " << e.what() << "\n"; } // Перехват
        catch (const std::exception& e) { std::cerr << "[КРИТИЧЕСКАЯ ОШИБКА]: " << e.what() << "\n"; } // Перехват
    } // Конец цикла
    return 0; // Завершение
} // Конец main
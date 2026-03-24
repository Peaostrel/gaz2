#define NOMINMAX // Защита от макросов Windows
#include <iostream> // Ввод вывод
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
    std::cout << "10. Поиск текста внутри файлов папки\n"; // 10
    std::cout << "13. Копировать ресурс (глубокая копия)\n"; // 13 НОВОЕ
    std::cout << "14. Фильтрация по дате\n"; // 14 НОВОЕ
    std::cout << "11. Сохранить архив (archive.dat)\n"; // 11
    std::cout << "12. Загрузить архив (archive.dat)\n"; // 12
    std::cout << "0. Выход\n"; // 0
    std::cout << "Выбор: "; // Приглашение
} // Конец функции

void clearInput() { // Очистка
    std::cin.clear(); // Сброс
    std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n'); // Игнор с защитой (max)()
} // Конец функции

int main() { // Старт
#ifdef _WIN32 // Фикс консоли
    SetConsoleOutputCP(CP_UTF8); // Вывод
    SetConsoleCP(CP_UTF8); // Ввод
#else // Unix
    setlocale(LC_ALL, "ru_RU.UTF-8"); // Локаль
#endif // Конец

    ArchiveManager archive; // Менеджер
    archive.setCurrentUserLevel(AccessLevel::ADMIN); // Права для тестов
    int choice = -1; // Переменная

    std::cout << "Система запущена. Логирование активно.\n"; // Сообщение

    while (choice != 0) { // Цикл
        printMenu(); // Вывод меню
        if (!(std::cin >> choice)) { // Проверка
            clearInput(); // Чистка
            std::cout << "Ошибка ввода.\n"; // Ошибка
            continue; // Пропуск
        } // Конец if

        try { // Перехват
            switch (choice) { // Выбор
                case 1: { // Папка
                    std::string name; int level; // Имя, права
                    std::cout << "Имя папки: "; std::cin >> name; // Ввод
                    std::cout << "Уровень доступа (0-GUEST, 1-USER, 2-ADMIN): "; // Ввод
                    if (!(std::cin >> level) || level < 0 || level > 2) { clearInput(); throw FileSystemException("Некорректный уровень."); } // Проверка
                    archive.addDirectory(name, static_cast<AccessLevel>(level)); std::cout << "[+] Создано.\n"; break; // Успех
                }
                case 2: { // Файл
                    std::string name, ext, content; size_t size; // Поля
                    std::cout << "Имя файла: "; std::cin >> name; // Ввод
                    std::cout << "Расширение: "; std::cin >> ext; // Ввод
                    std::cout << "Размер: "; 
                    if (!(std::cin >> size)) { clearInput(); throw FileSystemException("Некорректный размер."); } // Проверка
                    std::cout << "Текст (или '-'): "; std::cin >> content; // Текст
                    archive.addFile(name, ext, size, content); std::cout << "[+] Создано.\n"; break; // Успех
                }
                case 3: archive.printTree(); break; // Дерево
                case 4: archive.globalAudit(); break; // Аудит (Теперь с полным подсчетом)
                case 5: { // Маска
                    std::string mask; std::cout << "Regex маска: "; std::cin >> mask; archive.searchByMask(mask); break; // Поиск
                }
                case 6: { // Сортировка
                    int crit; std::cout << "Критерий (1-Имя, 2-Размер, 3-Дата): "; 
                    if (!(std::cin >> crit) || crit < 1 || crit > 3) throw FileSystemException("Ошибка"); // Проверка
                    archive.sortResources(crit); break; // Вызов
                }
                case 7: archive.exportToCSV("export.csv"); break; // Экспорт
                case 8: { // Удаление
                    std::string name; std::cout << "Имя для удаления: "; std::cin >> name; archive.deleteResource(name); break; // Вызов
                }
                case 9: { // Перемещение
                    std::string res, dir; std::cout << "Что: "; std::cin >> res; std::cout << "Куда: "; std::cin >> dir; archive.moveResource(res, dir); break; // Вызов
                }
                case 10: { // Контент
                    std::string dir, q; std::cout << "Папка ('root'): "; std::cin >> dir; std::cout << "Текст: "; std::cin >> q; archive.searchContentInDir(dir, q); break; // Вызов
                }
                case 13: { // НОВОЕ: Копирование
                    std::string res, dir; std::cout << "Что копируем: "; std::cin >> res; std::cout << "Куда: "; std::cin >> dir; archive.copyResource(res, dir); break; // Вызов
                }
                case 14: { // НОВОЕ: Фильтрация по дате
                    std::string start, end; std::cout << "Начало (YYYY-MM-DD): "; std::cin >> start; std::cout << "Конец (YYYY-MM-DD): "; std::cin >> end; archive.filterByDate(start, end); break; // Вызов
                }
                case 11: archive.saveToFile("archive.dat"); std::cout << "[OK] Сохранено.\n"; break; // Сохранить
                case 12: archive.loadFromFile("archive.dat"); std::cout << "[OK] Загружено.\n"; break; // Загрузить
                case 0: std::cout << "Выход...\n"; break; // Выход
                default: std::cout << "Неизвестная команда.\n"; break; // Неизвестно
            } // Конец switch
        } catch (const FileSystemException& e) { std::cerr << "[ОШИБКА ФС]: " << e.what() << "\n"; } // Перехват
        catch (const std::exception& e) { std::cerr << "[КРИТИЧЕСКАЯ ОШИБКА]: " << e.what() << "\n"; } // Перехват
    } // Конец цикла
    return 0; // Завершение
} // Конец main
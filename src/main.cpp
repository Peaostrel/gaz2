#include <iostream> // Ввод/вывод
#include <string> // Строки
#include <limits> // Лимиты типов (для очистки буфера)

#ifdef _WIN32 // Если компилируем под Windows
#include <windows.h> // Подключаем WinAPI для кодировок
#endif // Конец ifdef

#include "ArchiveManager.h" // Подключаем менеджер
#include "FileSystemException.h" // Подключаем ошибки

void printMenu() { // Вывод меню
    std::cout << "\n=== Меню Виртуального Архива ===\n"; // Шапка
    std::cout << "1. Создать папку\n"; // 1
    std::cout << "2. Создать файл\n"; // 2
    std::cout << "3. Вывести дерево архива\n"; // 3
    std::cout << "4. Глобальный аудит\n"; // 4
    std::cout << "5. Поиск по маске (Regex)\n"; // 5
    std::cout << "6. Сортировка (1-Имя, 2-Размер, 3-Дата)\n"; // 6
    std::cout << "7. Выгрузить в CSV\n"; // 7
    std::cout << "8. Удалить ресурс (рекурсивно)\n"; // 8
    std::cout << "9. Переместить ресурс (std::move)\n"; // 9 (Новая фича)
    std::cout << "10. Сохранить архив (archive.dat)\n"; // 10
    std::cout << "11. Загрузить архив (archive.dat)\n"; // 11
    std::cout << "0. Выход\n"; // 0
    std::cout << "Выбор: "; // Приглашение
} // Конец функции

void clearInput() { // Очистка потока
    std::cin.clear(); // Сброс флагов ошибок
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Игнор мусора до переноса строки
} // Конец функции

int main() { // Точка входа
#ifdef _WIN32 // Фикс кодировок для винды
    SetConsoleOutputCP(CP_UTF8); // Ставим UTF8 на вывод
    SetConsoleCP(CP_UTF8); // Ставим UTF8 на ввод
#else // Для Unix (Mac/Linux)
    setlocale(LC_ALL, "ru_RU.UTF-8"); // Ставим локаль
#endif // Конец фикса

    ArchiveManager archive; // Создаем объект менеджера (и корень внутри него)
    archive.setCurrentUserLevel(AccessLevel::ADMIN); // Ставим права админа (иначе не даст удалять или перемещать)
    int choice = -1; // Переменная выбора

    std::cout << "Система запущена. Логирование активно.\n"; // Приветствие

    while (choice != 0) { // Бесконечный цикл пока не 0
        printMenu(); // Печатаем меню
        if (!(std::cin >> choice)) { // Читаем выбор и проверяем на текст (буквы)
            clearInput(); // Чистим
            std::cout << "Ошибка ввода. Введите число.\n"; // Ругаемся
            continue; // Идем на новый круг
        } // Конец проверки

        try { // Блок перехвата
            switch (choice) { // Выбор действия
                case 1: { // Создание папки
                    std::string name; // Имя
                    int level; // Уровень
                    std::cout << "Имя папки: "; // Запрос
                    std::cin >> name; // Читаем
                    std::cout << "Уровень доступа (0-GUEST, 1-USER, 2-ADMIN): "; // Запрос прав
                    if (!(std::cin >> level) || level < 0 || level > 2) { // Проверка адекватности
                        clearInput(); // Очистка
                        throw FileSystemException("Некорректный уровень доступа."); // Бросок
                    } // Конец проверки
                    archive.addDirectory(name, static_cast<AccessLevel>(level)); // Добавляем (кастуем int в enum)
                    std::cout << "[+] Папка успешно создана.\n"; // Успех
                    break; // Выход из case
                }
                case 2: { // Создание файла
                    std::string name, ext; // Имя и формат
                    size_t size; // Вес
                    std::cout << "Имя файла: "; // Запрос
                    std::cin >> name; // Ввод
                    std::cout << "Расширение (без точки): "; // Запрос
                    std::cin >> ext; // Ввод
                    std::cout << "Размер (байт): "; // Запрос
                    if (!(std::cin >> size)) { // Проверка
                        clearInput(); // Очистка
                        throw FileSystemException("Некорректный размер файла."); // Ошибка
                    } // Конец проверки
                    archive.addFile(name, ext, size); // Добавляем
                    std::cout << "[+] Файл успешно создан.\n"; // Успех
                    break; // Выход
                }
                case 3: // Печать дерева
                    archive.printTree(); // Вызываем метод
                    break; // Выход
                case 4: // Аудит
                    archive.globalAudit(); // Вызываем метод
                    break; // Выход
                case 5: { // Поиск
                    std::string mask; // Строка
                    std::cout << "Введите маску Regex: "; // Запрос
                    std::cin >> mask; // Ввод
                    archive.searchByMask(mask); // Ищем
                    break; // Выход
                }
                case 6: { // Сортировка
                    int criteria; // Критерий
                    std::cout << "Критерий (1-Имя, 2-Размер, 3-Дата): "; // Запрос
                    if (!(std::cin >> criteria) || criteria < 1 || criteria > 3) { // Проверка
                        clearInput(); // Чистка
                        throw FileSystemException("Некорректный критерий сортировки."); // Ошибка
                    } // Конец проверки
                    archive.sortResources(criteria); // Вызов
                    break; // Выход
                }
                case 7: // Экспорт
                    archive.exportToCSV("export.csv"); // Вшитое имя файла
                    break; // Выход
                case 8: { // Удаление
                    std::string name; // Имя
                    std::cout << "Имя файла/папки для удаления: "; // Запрос
                    std::cin >> name; // Ввод
                    archive.deleteResource(name); // Удаляем
                    break; // Выход
                }
                case 9: { // Перемещение
                    std::string resName, dirName; // Исходник и цель
                    std::cout << "Что перемещаем (имя ресурса): "; // Запрос
                    std::cin >> resName; // Ввод
                    std::cout << "Куда перемещаем (имя целевой папки): "; // Запрос
                    std::cin >> dirName; // Ввод
                    archive.moveResource(resName, dirName); // Вызываем наш новый метод
                    break; // Выход
                }
                case 10: // Сохранение
                    archive.saveToFile("archive.dat"); // Вшитое имя архива
                    std::cout << "[OK] Данные сериализованы в archive.dat\n"; // Успех
                    break; // Выход
                case 11: // Загрузка
                    archive.loadFromFile("archive.dat"); // Вшитое имя
                    std::cout << "[OK] Данные успешно загружены из archive.dat\n"; // Успех
                    break; // Выход
                case 0: // Выход
                    std::cout << "Завершение работы...\n"; // Прощание
                    break; // Выход
                default: // Неизвестно
                    std::cout << "Неизвестная команда.\n"; // Ругаемся
                    break; // Выход
            } // Конец switch
        } catch (const FileSystemException& e) { // Ловим только наши ошибки системы
            std::cerr << "[ОШИБКА ФС]: " << e.what() << "\n"; // Красиво печатаем (cerr = поток ошибок)
        } catch (const std::exception& e) { // Ловим системные сбои (переполнение памяти и тд)
            std::cerr << "[КРИТИЧЕСКАЯ ОШИБКА]: " << e.what() << "\n"; // Красиво печатаем
        } // Конец try-catch
    } // Конец while

    return 0; // Завершаем программу кодом 0
} // Конец точки входа
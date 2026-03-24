#include <iostream>
#include <string>
#include <limits>

#ifdef _WIN32
#include <windows.h>
#endif

#include "ArchiveManager.h"
#include "FileSystemException.h"

void printMenu() {
    std::cout << "\n=== Меню Виртуального Архива ===\n";
    std::cout << "1. Создать папку\n";
    std::cout << "2. Создать файл\n";
    std::cout << "3. Вывести дерево архива\n";
    std::cout << "4. Глобальный аудит\n";
    std::cout << "5. Сохранить архив (archive.dat)\n";
    std::cout << "6. Загрузить архив (archive.dat)\n";
    std::cout << "0. Выход\n";
    std::cout << "Выбор: ";
}

void clearInput() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#else
    setlocale(LC_ALL, "ru_RU.UTF-8");
#endif

    ArchiveManager archive;
    int choice = -1;

    std::cout << "Система запущена. Логирование активно.\n";

    while (choice != 0) {
        printMenu();
        if (!(std::cin >> choice)) {
            clearInput();
            std::cout << "Ошибка ввода. Введите число.\n";
            continue;
        }

        try {
            switch (choice) {
                case 1: {
                    std::string name;
                    int level;
                    std::cout << "Имя папки: ";
                    std::cin >> name;
                    std::cout << "Уровень доступа (0-GUEST, 1-USER, 2-ADMIN): ";
                    if (!(std::cin >> level) || level < 0 || level > 2) {
                        clearInput();
                        throw FileSystemException("Некорректный уровень доступа.");
                    }
                    archive.addDirectory(name, static_cast<AccessLevel>(level));
                    std::cout << "[+] Папка успешно создана.\n";
                    break;
                }
                case 2: {
                    std::string name, ext;
                    size_t size;
                    std::cout << "Имя файла: ";
                    std::cin >> name;
                    std::cout << "Расширение (без точки): ";
                    std::cin >> ext;
                    std::cout << "Размер (байт): ";
                    if (!(std::cin >> size)) {
                        clearInput();
                        throw FileSystemException("Некорректный размер файла.");
                    }
                    archive.addFile(name, ext, size);
                    std::cout << "[+] Файл успешно создан.\n";
                    break;
                }
                case 3:
                    archive.printTree();
                    break;
                case 4:
                    archive.globalAudit();
                    break;
                case 5:
                    archive.saveToFile("archive.dat");
                    std::cout << "[OK] Данные сериализованы в archive.dat\n";
                    break;
                case 6:
                    archive.loadFromFile("archive.dat");
                    std::cout << "[OK] Данные успешно загружены из archive.dat\n";
                    break;
                case 0:
                    std::cout << "Завершение работы...\n";
                    break;
                default:
                    std::cout << "Неизвестная команда.\n";
                    break;
            }
        } catch (const FileSystemException& e) {
            std::cerr << "[ОШИБКА ФС]: " << e.what() << "\n";
        } catch (const std::exception& e) {
            std::cerr << "[КРИТИЧЕСКАЯ ОШИБКА]: " << e.what() << "\n";
        }
    }

    return 0;
}
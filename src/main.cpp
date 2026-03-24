#include <iostream>
#include "ArchiveManager.h"
#include "FileSystemException.h"

int main() {
    // Поддержка кириллицы в консоли
    setlocale(LC_ALL, "Russian");

    try {
        ArchiveManager archive;

        std::cout << "--- Наполнение архива ---\n";
        archive.addDirectory("Documents", AccessLevel::USER);
        archive.addDirectory("SystemTools", AccessLevel::ADMIN);
        
        archive.addFile("readme", "txt", 1024);
        archive.addFile("config", "cfg", 2048);
        
        archive.printTree();
        archive.globalAudit();

        std::cout << "--- Проверка системы валидации (ожидается перехват ошибки) ---\n";
        // Пытаемся создать файл с запрещенным символом '*'
        archive.addFile("virus*file", "exe", 9999); 

    } catch (const FileSystemException& e) {
        std::cerr << "[ОШИБКА ФС]: " << e.what() << "\n";
    } catch (const std::exception& e) {
        std::cerr << "[КРИТИЧЕСКАЯ ОШИБКА]: " << e.what() << "\n";
    }

    return 0;
}
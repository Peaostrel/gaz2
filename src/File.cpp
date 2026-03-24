#include "File.h"
#include "FileSystemException.h"
#include <iostream>
#include <regex>

File::File(const std::string& name, const std::string& ext, size_t fileSize) 
    : Resource(name), size(fileSize) {
    setExtension(ext);
}

const std::string& File::getExtension() const {
    return extension;
}

void File::setExtension(const std::string& ext) {
    // Валидация: расширение должно состоять только из английских букв и цифр
    std::regex validExt("^[a-zA-Z0-9]*$");
    if (!std::regex_match(ext, validExt)) {
        throw FileSystemException("Некорректный формат расширения файла: " + ext);
    }
    extension = ext;
}

size_t File::getSize() const {
    return size;
}

void File::setSize(size_t newSize) {
    size = newSize;
}

size_t File::calculateSize() const {
    return size;
}

void File::print(int depth) const {
    std::string indent(depth * 2, ' ');
    std::cout << indent << "- [File] " << getName();
    if (!extension.empty()) {
        std::cout << "." << extension;
    }
    std::cout << " (" << size << " bytes)\n";
}

bool File::isDirectory() const {
    return false;
}
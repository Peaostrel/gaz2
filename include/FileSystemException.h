#ifndef FILESYSTEMEXCEPTION_H
#define FILESYSTEMEXCEPTION_H

#include <exception>
#include <string>

class FileSystemException : public std::exception {
private:
    std::string message;
public:
    explicit FileSystemException(const std::string& msg) : message(msg) {}
    const char* what() const noexcept override {
        return message.c_str();
    }
};

#endif // FILESYSTEMEXCEPTION_H
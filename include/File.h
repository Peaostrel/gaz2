#ifndef FILE_H
#define FILE_H

#include "Resource.h"
#include <string>

class File : public Resource {
private:
    std::string extension;
    size_t size;

public:
    File(const std::string& name, const std::string& ext, size_t fileSize = 0);

    const std::string& getExtension() const;
    void setExtension(const std::string& ext);
    
    size_t getSize() const;
    void setSize(size_t newSize);

    size_t calculateSize() const override;
    void print(int depth = 0) const override;
    bool isDirectory() const override;
};

#endif // FILE_H

#include "FileProperties.h"
#include <chrono>

FileProperties::FileProperties() {
    touch();
}

void FileProperties::touch() {
    time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
}

std::string FileProperties::fullPath() const {
    if (directory.empty()) return filename;
    return directory + "/" + filename;
}
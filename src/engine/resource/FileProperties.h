#pragma once
#include <string>
#include <cstdint>

class FileProperties {
public:
    uint32_t header = 1;
    uint32_t size = 0;
    uint32_t cmpSize = 0;
    int64_t time = 0;
    uint64_t offset = 0;
    std::string directory;
    std::string filename;
    uint32_t checksum = 0;
    uint32_t footer = 0;

    FileProperties();
    void touch();
    std::string fullPath() const;
};

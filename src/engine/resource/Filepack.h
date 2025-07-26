#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <memory>

class Bundle;
class Path;

class Filepack {
public:
    Filepack(Bundle* bundle, uint64_t offset, uint32_t size, uint32_t compressedSize, const Path* path);
    ~Filepack();
    
    bool readFileToBuffer(uint32_t flags);
    uint32_t read(uint8_t* buffer, uint32_t size, uint32_t offset = 0);
    uint32_t getSize() const { return m_size; }
    uint32_t getCompressedSize() const { return m_compressedSize; }
    uint64_t getOffset() const { return m_offset; }
    const std::string& getPath() const { return m_path; }
    bool isCompressed() const { return m_compressedSize > 0; }
    
private:
    Bundle* m_bundle;
    uint64_t m_offset;
    uint32_t m_size;
    uint32_t m_compressedSize;
    std::string m_path;
    std::vector<uint8_t> m_buffer;
    bool m_isLoaded;
}; 
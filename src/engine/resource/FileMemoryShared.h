#pragma once
#include <cstdint>
#include <string>
#include <memory>

class FileMemoryShared {
public:
    FileMemoryShared(uint32_t size);
    ~FileMemoryShared();
    
    bool open(const std::string& path, int mode = 2, int flags = 5);
    bool read(uint8_t* buffer, uint32_t size);
    bool write(uint32_t offset, const uint8_t* data, uint32_t size);
    uint64_t getSize() const;
    uint64_t getPosition() const;
    bool seek(uint64_t position);
    void close();
    
    uint8_t* getData() { return m_data.get(); }
    const uint8_t* getData() const { return m_data.get(); }
    
private:
    std::unique_ptr<uint8_t[]> m_data;
    uint32_t m_size;
    uint64_t m_position;
    bool m_isOpen;
}; 
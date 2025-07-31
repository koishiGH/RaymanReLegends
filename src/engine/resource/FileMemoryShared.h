#pragma once
#include <cstdint>
#include <string>
#include <memory>

class File {
public:
    virtual ~File() = default;
    virtual bool open(const std::string& path, int mode, int flags) = 0;
    virtual bool read(uint8_t* buffer, uint32_t size) = 0;
    virtual bool seek(uint64_t position) = 0;
    virtual uint64_t getSize() const = 0;
    virtual uint64_t getPosition() const = 0;
    virtual void close() = 0;
};

class FileMemoryShared : public File {
public:
    FileMemoryShared(uint32_t size);
    ~FileMemoryShared() override;
    
    bool open(const std::string& path, int mode = 2, int flags = 5) override;
    bool read(uint8_t* buffer, uint32_t size) override;
    bool seek(uint64_t position) override;
    uint64_t getSize() const override;
    uint64_t getPosition() const override;
    void close() override;
    bool write(uint32_t offset, const uint8_t* data, uint32_t size);
    
    uint8_t* getData() { return m_data.get(); }
    const uint8_t* getData() const { return m_data.get(); }
    
private:
    std::unique_ptr<uint8_t[]> m_data;
    uint32_t m_size;
    uint64_t m_position;
    bool m_isOpen;
}; 
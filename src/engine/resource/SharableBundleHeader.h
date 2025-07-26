#pragma once
#include <cstdint>
#include <string>
#include <map>
#include <memory>

class FileHeaderRuntime;
class FilePackMaster;
class Path;

class SharableBundleHeader {
public:
    SharableBundleHeader();
    ~SharableBundleHeader();
    
    bool build(FilePackMaster* filePackMaster);
    bool fileExists(const Path* path) const;
    uint32_t fileGetSize(const Path* path) const;
    uint32_t fileGetCompressedSize(const Path* path) const;
    int64_t fileGetLastTimeWriteAccess(const Path* path) const;
    uint64_t fileGetPosition(const Path* path) const;
    FileHeaderRuntime* getHeaderRuntime(const Path* path) const;
    
    uint32_t getBaseOffset() const { return m_baseOffset; }
    uint32_t getDataOffset() const { return m_dataOffset; }
    uint32_t getHeaderSize() const { return m_headerSize; }
    uint32_t getCompressedSize() const { return m_compressedSize; }
    uint32_t getUncompressedSize() const { return m_uncompressedSize; }
    uint32_t getFlags() const { return m_flags; }
    
    void setBaseOffset(uint32_t offset) { m_baseOffset = offset; }
    void setDataOffset(uint32_t offset) { m_dataOffset = offset; }
    void setHeaderSize(uint32_t size) { m_headerSize = size; }
    void setCompressedSize(uint32_t size) { m_compressedSize = size; }
    void setUncompressedSize(uint32_t size) { m_uncompressedSize = size; }
    void setFlags(uint32_t flags) { m_flags = flags; }
    
private:
    std::map<std::string, FileHeaderRuntime*> m_fileHeaders;
    uint32_t m_baseOffset;
    uint32_t m_dataOffset;
    uint32_t m_headerSize;
    uint32_t m_compressedSize;
    uint32_t m_uncompressedSize;
    uint32_t m_flags;
    bool m_isBuilt;
}; 
#pragma once
#include <cstdint>
#include <vector>

class ArchiveMemory;

class BundleBootHeader {
public:
    BundleBootHeader();
    ~BundleBootHeader();
    
    bool serialize(ArchiveMemory* archive);
    bool deserialize(ArchiveMemory* archive);
    
    uint32_t magic;
    uint32_t version;
    uint32_t headerSize;
    uint32_t dataOffset;
    uint32_t compressedSize;
    uint32_t uncompressedSize;
    uint32_t flags;
    uint32_t checksum;
    
    bool isValid() const;
    static const uint32_t MAGIC = 0x50EC12BA;
}; 
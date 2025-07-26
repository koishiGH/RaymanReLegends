#pragma once
#include <cstdint>
#include <vector>
#include <memory>

class File;

class BundleCache {
public:
    BundleCache(File* file, uint32_t cacheSize = 0x20000);
    ~BundleCache();
    
    bool seek(uint64_t position);
    uint32_t read(uint8_t* buffer, uint32_t size);
    uint64_t getCurrentPosition() const;
    uint64_t getCacheStart() const;
    uint64_t getCacheEnd() const;
    uint32_t getCacheSize() const;
    
private:
    File* m_file;
    std::vector<uint8_t> m_cache;
    uint64_t m_cacheStart;
    uint64_t m_cacheEnd;
    uint64_t m_currentPosition;
    uint32_t m_cacheSize;
    
    bool loadCache(uint64_t position);
}; 
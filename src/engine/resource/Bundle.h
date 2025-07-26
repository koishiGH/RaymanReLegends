#pragma once
#include <string>
#include <vector>
#include <memory>
#include <cstdint>
#include <map>
#include <mutex>
#include "FileProperties.h"

class BundleCache;
class FileMemoryShared;
class BundleBootHeader;
class FilePackMaster;
class SharableBundleHeader;
class Filepack;

class Bundle {
public:
    Bundle();
    ~Bundle();
    
    bool openBundle(const std::string& path, int mode = 2, bool checkBootHeader = true, 
                   void* sharedMemory = nullptr, uint8_t flags = 0, uint32_t* result = nullptr, 
                   void* additionalData = nullptr);
    void closeBundle();
    
    bool fileExists(const std::string& path) const;
    uint32_t fileGetSize(const std::string& path) const;
    uint32_t fileGetCompressedSize(const std::string& path) const;
    int64_t fileGetLastTimeWriteAccess(const std::string& path) const;
    bool fileGetPosition(const std::string& path, uint64_t& position, uint64_t& size, bool& compressed) const;
    Filepack* fileOpen(const std::string& path, uint32_t flags = 0);
    bool fileRead(uint64_t offset, uint8_t* buffer, uint32_t size, uint32_t* bytesRead);
    
    bool isDLCBundleValid(const std::string& path) const;
    void uncompressBundle(const BundleBootHeader* bootHeader);
    Filepack* TrackReference(Filepack* filepack);
    uint64_t UntrackReference(Filepack* filepack);
    uint64_t checkNeedDeletion();
    
    std::vector<std::string> listFiles() const;
    const std::string& getPath() const { return m_path; }
    bool isOpen() const { return m_fileHandle != nullptr; }

private:
    std::string m_path;
    std::unique_ptr<FileMemoryShared> m_fileHandle;
    std::unique_ptr<SharableBundleHeader> m_bundleHeader;
    std::unique_ptr<BundleCache> m_cache;
    
    uint64_t m_globalPosition;
    uint64_t m_fileSize;
    uint64_t m_currentPosition;
    uint32_t m_mode;
    uint32_t m_flags;
    uint32_t m_baseOffset;
    uint32_t m_dataOffset;
    uint32_t m_headerSize;
    uint32_t m_compressedSize;
    uint32_t m_uncompressedSize;
    
    std::map<Filepack*, Filepack*> m_openFiles;
    std::mutex m_fileMutex;    
    std::vector<std::pair<FileProperties, std::streampos>> m_fileList;
    
    bool readBootHeader();
    bool readFilePackMaster();
    void clearOpenFiles();
    bool seekToPosition(uint64_t position);
    uint64_t registerGlobalPosition();
};
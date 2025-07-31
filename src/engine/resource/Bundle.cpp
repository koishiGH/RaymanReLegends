#include "Bundle.h"
#include "BundleCache.h"
#include "FileMemoryShared.h"
#include "BundleBootHeader.h"
#include "SharableBundleHeader.h"
#include "Filepack.h"
#include "FileProperties.h"
#include <fstream>
#include <algorithm>
#include <stdexcept>
#include <iostream>
#include <cstring>

uint32_t swapEndian(uint32_t value) {
    return ((value & 0xFF) << 24) |
           (((value >> 8) & 0xFF) << 16) |
           (((value >> 16) & 0xFF) << 8) |
           ((value >> 24) & 0xFF);
}

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

class ArchiveMemory {
public:
    ArchiveMemory(const uint8_t* data, uint32_t size);
    ~ArchiveMemory();
    bool read(uint8_t* buffer, uint32_t size);
    bool write(const uint8_t* buffer, uint32_t size);
    uint32_t getPosition() const;
    uint32_t getSize() const;
private:
    const uint8_t* m_data;
    uint32_t m_size;
    uint32_t m_position;
};

class FilePackMaster {
public:
    FilePackMaster();
    ~FilePackMaster();
    bool serialize(ArchiveMemory* archive);
    bool deserialize(ArchiveMemory* archive);
    std::map<std::string, FileHeaderRuntime*> m_files;
};

class FileHeaderRuntime {
public:
    uint32_t size;
    uint32_t compressedSize;
    uint64_t offset;
    int64_t timestamp;
    uint32_t checksum;
    uint32_t flags;
};

class Path {
public:
    Path();
    Path(const std::string& path);
    ~Path();
    std::string toString() const;
    const std::string& getPath() const { return m_path; }
private:
    std::string m_path;
};

Bundle::Bundle() 
    : m_globalPosition(0)
    , m_fileSize(0)
    , m_currentPosition(0)
    , m_mode(0)
    , m_flags(0)
    , m_baseOffset(0)
    , m_dataOffset(0)
    , m_headerSize(0)
    , m_compressedSize(0)
    , m_uncompressedSize(0)
{
}

Bundle::~Bundle() {
    closeBundle();
}

bool Bundle::openBundle(const std::string& path, int mode, bool checkBootHeader, 
                       void* sharedMemory, uint8_t flags, uint32_t* result, 
                       void* additionalData) {
    m_path = path;
    m_mode = mode;
    m_flags = flags;

    std::ifstream file(m_path, std::ios::binary);
    if (!file) {
        std::cerr << "Failed to open bundle: " << m_path << std::endl;
        return false;
    }

    uint32_t magic;
    file.read(reinterpret_cast<char*>(&magic), sizeof(magic));
    bool swapEndianness = false;
    if (magic != 0x50EC12BA) {
        uint32_t swappedMagic = swapEndian(magic);
        if (swappedMagic == 0x50EC12BA) {
            swapEndianness = true;
            std::cout << "Detected big-endian bundle, swapping byte order" << std::endl;
        } else {
            std::cerr << "Invalid bundle magic header: " << magic << " (swapped: " << swappedMagic << ")" << std::endl;
            return false;
        }
    }

    auto readU32 = [&file, swapEndianness]() -> uint32_t {
        uint32_t value;
        file.read(reinterpret_cast<char*>(&value), sizeof(value));
        return swapEndianness ? swapEndian(value) : value;
    };
    auto readU64 = [&file, swapEndianness]() -> uint64_t {
        uint64_t value;
        file.read(reinterpret_cast<char*>(&value), sizeof(value));
        if (swapEndianness) {
            return ((uint64_t)swapEndian((uint32_t)(value & 0xFFFFFFFF)) << 32) |
                   swapEndian((uint32_t)(value >> 32));
        }
        return value;
    };
    auto readI32 = [&file, swapEndianness]() -> int32_t {
        int32_t value;
        file.read(reinterpret_cast<char*>(&value), sizeof(value));
        return swapEndianness ? (int32_t)swapEndian((uint32_t)value) : value;
    };

    uint32_t version = readU32();
    std::cout << "Bundle version: " << version << std::endl;
    uint32_t unknown1 = readU32();
    uint32_t unknown2 = 0;
    if (version >= 8) {
        unknown2 = readU32();
    }
    uint32_t baseOffset = readU32();
    uint32_t filesCount = readU32();
    std::cout << "Base offset: " << baseOffset << ", Files count: " << filesCount << std::endl;
    uint32_t unknown3 = readU32();
    uint32_t unknown4 = readU32();
    uint32_t unknown5 = readU32();
    uint32_t unknown6 = readU32();
    uint32_t unknown7 = readU32();
    uint32_t engineVersion = readU32();
    uint32_t blockSize = 0, blockCompressedSize = 0;
    if (version >= 6) {
        blockSize = readU32();
        blockCompressedSize = readU32();
    }
    m_baseOffset = baseOffset;
    uint32_t fileCount = readU32();
    if (fileCount != filesCount) {
        std::cout << "Warning: Header file count (" << filesCount << ") doesn't match file list count (" << fileCount << ")" << std::endl;
    }

    m_fileList.clear();
    for (uint32_t i = 0; i < fileCount; ++i) {
        FileProperties props;
        std::streampos offset = file.tellg();

        if (version != 4) {
            props.header = readU32();
        } else {
            props.header = 1;
        }
        props.size = readU32();
        props.cmpSize = readU32();
        
        int64_t timeValue = 0;
        if (swapEndianness) {
            uint64_t rawTime;
            file.read(reinterpret_cast<char*>(&rawTime), sizeof(rawTime));
            timeValue = (int64_t)(((uint64_t)swapEndian((uint32_t)(rawTime & 0xFFFFFFFF)) << 32) |
                                 swapEndian((uint32_t)(rawTime >> 32)));
        } else {
            file.read(reinterpret_cast<char*>(&timeValue), sizeof(timeValue));
        }
        props.time = timeValue;
        
        uint32_t offsetsCount = (version != 4) ? props.header : 1;
        std::vector<uint64_t> offsets(offsetsCount);
        for (uint32_t j = 0; j < offsetsCount; ++j) {
            offsets[j] = readU64();
        }
        props.offset = offsets[0];

        if (version >= 5) {
            int dirLen = readI32();
            if (dirLen < 0 || dirLen > 10000) {
                throw std::runtime_error("Invalid directory length: " + std::to_string(dirLen));
            }
            props.directory.resize(dirLen);
            file.read(&props.directory[0], dirLen);
            
            int fileLen = readI32();
            if (fileLen < 0 || fileLen > 10000) {
                throw std::runtime_error("Invalid filename length: " + std::to_string(fileLen));
            }
            props.filename.resize(fileLen);
            file.read(&props.filename[0], fileLen);
            
            props.checksum = readU32();            
            props.footer = readU32();
        } else if (version == 4) {
            int pathLen = readI32();
            if (pathLen < 0 || pathLen > 10000) {
                throw std::runtime_error("Invalid path length: " + std::to_string(pathLen));
            }
            std::string fullPath(pathLen * 2, '\0');
            file.read(&fullPath[0], pathLen * 2);
            
            size_t lastSlash = fullPath.find_last_of('/');
            if (lastSlash != std::string::npos) {
                props.directory = fullPath.substr(0, lastSlash + 1);
                props.filename = fullPath.substr(lastSlash + 1);
            } else {
                props.directory = "";
                props.filename = fullPath;
            }
        }
        m_fileList.emplace_back(props, offset);
    }
    return true;
}

void Bundle::closeBundle() {
    clearOpenFiles();
    
    if (m_cache) {
        m_cache.reset();
    }
    
    if (m_bundleHeader) {
        m_bundleHeader.reset();
    }
    
    if (m_fileHandle) {
        m_fileHandle->close();
        m_fileHandle.reset();
    }
    
    m_path.clear();
    m_fileSize = 0;
    m_currentPosition = 0;
}

bool Bundle::fileExists(const std::string& path) const {
    for (const auto& entry : m_fileList) {
        if (entry.first.fullPath() == path) {
            return true;
        }
    }
    return false;
}

uint32_t Bundle::fileGetSize(const std::string& path) const {
    for (const auto& entry : m_fileList) {
        if (entry.first.fullPath() == path) {
            return entry.first.size;
        }
    }
    return 0;
}

uint32_t Bundle::fileGetCompressedSize(const std::string& path) const {
    for (const auto& entry : m_fileList) {
        if (entry.first.fullPath() == path) {
            return entry.first.cmpSize;
        }
    }
    return 0;
}

int64_t Bundle::fileGetLastTimeWriteAccess(const std::string& path) const {
    for (const auto& entry : m_fileList) {
        if (entry.first.fullPath() == path) {
            return entry.first.time;
        }
    }
    return 0;
}

bool Bundle::fileGetPosition(const std::string& path, uint64_t& position, uint64_t& size, bool& compressed) const {
    for (const auto& entry : m_fileList) {
        if (entry.first.fullPath() == path) {
            position = static_cast<uint64_t>(entry.second) + m_globalPosition;
            if (entry.first.cmpSize > 0) {
                size = entry.first.cmpSize;
                compressed = true;
            } else {
                size = entry.first.size;
                compressed = false;
            }
            return true;
        }
    }
    return false;
}

Filepack* Bundle::fileOpen(const std::string& path, uint32_t flags) {
    for (const auto& entry : m_fileList) {
        if (entry.first.fullPath() == path) {
            uint64_t offset = static_cast<uint64_t>(entry.second) + m_globalPosition;
            Path pathObj(path);
            
            Filepack* filepack = new Filepack(this, offset, entry.first.size, entry.first.cmpSize, &pathObj);
            if (entry.first.cmpSize > 0) {
                filepack->readFileToBuffer(flags);
            }
            
            return filepack;
        }
    }
    return nullptr;
}

bool Bundle::fileRead(uint64_t offset, uint8_t* buffer, uint32_t size, uint32_t* bytesRead) {
    if (!m_cache || !m_fileHandle) return false;
    
    *bytesRead = 0;
    
    if (offset != m_cache->getCurrentPosition()) {
        m_cache->seek(offset);
        if (m_cache->getCacheStart() == m_cache->getCacheEnd()) {
            m_currentPosition = m_fileHandle->seek(offset);
        }
    }
    
    if (size > m_cache->getCacheSize()) {
        uint64_t cacheStart = m_cache->getCacheStart();
        uint64_t cacheEnd = m_cache->getCacheEnd();
        
        if (offset >= cacheStart && offset < cacheEnd) {
            uint32_t cachedBytes = m_cache->read(buffer, static_cast<uint32_t>(cacheEnd - offset));
            buffer += cachedBytes;
            size -= cachedBytes;
            *bytesRead += cachedBytes;
            m_currentPosition = m_cache->getCurrentPosition();
        }
        
        if (size > 0) {
            uint32_t directBytes;
            m_fileHandle->read(buffer, size);
            *bytesRead += size;
            m_currentPosition += size;
        }
    } else {
        uint32_t readBytes = m_cache->read(buffer, size);
        *bytesRead = readBytes;
        m_currentPosition = m_cache->getCurrentPosition();
    }
    
    return true;
}

bool Bundle::isDLCBundleValid(const std::string& path) const {
    return true;
}

void Bundle::uncompressBundle(const BundleBootHeader* bootHeader) {
    if (!bootHeader || !m_fileHandle) return;
    m_uncompressedSize = bootHeader->uncompressedSize;
    m_compressedSize = bootHeader->compressedSize;
}

Filepack* Bundle::TrackReference(Filepack* filepack) {
    std::lock_guard<std::mutex> lock(m_fileMutex);
    m_openFiles[filepack] = filepack;
    return filepack;
}

uint64_t Bundle::UntrackReference(Filepack* filepack) {
    std::lock_guard<std::mutex> lock(m_fileMutex);
    auto it = m_openFiles.find(filepack);
    if (it != m_openFiles.end()) {
        m_openFiles.erase(it);
    }
    return checkNeedDeletion();
}

uint64_t Bundle::checkNeedDeletion() {
    if (m_flags & 0x01) {
        std::lock_guard<std::mutex> lock(m_fileMutex);
        if (m_openFiles.empty()) {
            return 1;
        }
    }
    return 0;
}

std::vector<std::string> Bundle::listFiles() const {
    std::vector<std::string> files;
    for (const auto& entry : m_fileList) {
        files.push_back(entry.first.fullPath());
    }
    return files;
}

bool Bundle::readBootHeader() {
    if (!m_fileHandle) return false;
    
    BundleBootHeader bootHeader;
    uint8_t headerData[56];
    
    if (!m_fileHandle->read(headerData, 56)) return false;
    
    ArchiveMemory archive(headerData, 56);
    if (!bootHeader.deserialize(&archive)) return false;
    
    if (!bootHeader.isValid()) return false;
    
    m_headerSize = bootHeader.headerSize;
    m_dataOffset = bootHeader.dataOffset;
    m_compressedSize = bootHeader.compressedSize;
    m_uncompressedSize = bootHeader.uncompressedSize;
    
    return readFilePackMaster();
}

bool Bundle::readFilePackMaster() {
    if (!m_fileHandle) return false;
    
    uint32_t masterSize = m_headerSize - 56;
    std::vector<uint8_t> masterData(masterSize);
    
    if (!m_fileHandle->read(masterData.data(), masterSize)) return false;
    
    ArchiveMemory archive(masterData.data(), masterSize);
    FilePackMaster filePackMaster;
    
    if (!filePackMaster.deserialize(&archive)) return false;
    
    return m_bundleHeader->build(&filePackMaster);
}

void Bundle::clearOpenFiles() {
    std::lock_guard<std::mutex> lock(m_fileMutex);
    for (auto& [filepack, _] : m_openFiles) {
        delete filepack;
    }
    m_openFiles.clear();
}

bool Bundle::seekToPosition(uint64_t position) {
    if (!m_fileHandle) return false;
    return m_fileHandle->seek(position);
}

uint64_t Bundle::registerGlobalPosition() {
    uint64_t hash = 0;
    for (char c : m_path) {
        hash = hash * 31 + c;
    }
    return hash;
}

ArchiveMemory::ArchiveMemory(const uint8_t* data, uint32_t size) 
    : m_data(data), m_size(size), m_position(0) {}

ArchiveMemory::~ArchiveMemory() = default;

bool ArchiveMemory::read(uint8_t* buffer, uint32_t size) {
    if (m_position + size > m_size) return false;
    std::memcpy(buffer, m_data + m_position, size);
    m_position += size;
    return true;
}

bool ArchiveMemory::write(const uint8_t* buffer, uint32_t size) {
    if (m_position + size > m_size) return false;
    std::memcpy(const_cast<uint8_t*>(m_data + m_position), buffer, size);
    m_position += size;
    return true;
}

uint32_t ArchiveMemory::getPosition() const { return m_position; }
uint32_t ArchiveMemory::getSize() const { return m_size; }

BundleBootHeader::BundleBootHeader() 
    : magic(0), version(0), headerSize(0), dataOffset(0), 
      compressedSize(0), uncompressedSize(0), flags(0), checksum(0) {}

BundleBootHeader::~BundleBootHeader() = default;

bool BundleBootHeader::serialize(ArchiveMemory* archive) {
    if (!archive) return false;
    return archive->write(reinterpret_cast<const uint8_t*>(this), sizeof(*this));
}

bool BundleBootHeader::deserialize(ArchiveMemory* archive) {
    if (!archive) return false;
    return archive->read(reinterpret_cast<uint8_t*>(this), sizeof(*this));
}

bool BundleBootHeader::isValid() const {
    return magic == MAGIC;
}

Path::Path() = default;
Path::Path(const std::string& path) : m_path(path) {}
Path::~Path() = default;
std::string Path::toString() const { return m_path; }

FilePackMaster::FilePackMaster() = default;
FilePackMaster::~FilePackMaster() = default;

bool FilePackMaster::serialize(ArchiveMemory* archive) {
    return true;
}

bool FilePackMaster::deserialize(ArchiveMemory* archive) {
    if (!archive) return false;
    
    uint32_t fileCount;
    if (!archive->read(reinterpret_cast<uint8_t*>(&fileCount), sizeof(fileCount))) return false;
    
    for (uint32_t i = 0; i < fileCount; i++) {
        uint32_t pathLength;
        if (!archive->read(reinterpret_cast<uint8_t*>(&pathLength), sizeof(pathLength))) return false;
        
        std::string path(pathLength, '\0');
        if (!archive->read(reinterpret_cast<uint8_t*>(&path[0]), pathLength)) return false;
        
        FileHeaderRuntime* header = new FileHeaderRuntime();
        
        if (!archive->read(reinterpret_cast<uint8_t*>(&header->size), sizeof(header->size))) return false;
        if (!archive->read(reinterpret_cast<uint8_t*>(&header->compressedSize), sizeof(header->compressedSize))) return false;
        if (!archive->read(reinterpret_cast<uint8_t*>(&header->offset), sizeof(header->offset))) return false;
        if (!archive->read(reinterpret_cast<uint8_t*>(&header->timestamp), sizeof(header->timestamp))) return false;
        if (!archive->read(reinterpret_cast<uint8_t*>(&header->checksum), sizeof(header->checksum))) return false;
        if (!archive->read(reinterpret_cast<uint8_t*>(&header->flags), sizeof(header->flags))) return false;        
        m_files[path] = header;
    }
    return true;
}

SharableBundleHeader::SharableBundleHeader() 
    : m_baseOffset(0), m_dataOffset(0), m_headerSize(0), 
      m_compressedSize(0), m_uncompressedSize(0), m_flags(0), m_isBuilt(false) {}

SharableBundleHeader::~SharableBundleHeader() {
    for (auto& [_, header] : m_fileHeaders) {
        delete header;
    }
}

bool SharableBundleHeader::build(FilePackMaster* filePackMaster) {
    if (!filePackMaster) return false;
    
    for (const auto& [path, header] : filePackMaster->m_files) {
        m_fileHeaders[path] = header;
    }
    m_isBuilt = true;
    return true;
}

bool SharableBundleHeader::fileExists(const Path* path) const {
    return m_fileHeaders.find(path->getPath()) != m_fileHeaders.end();
}

uint32_t SharableBundleHeader::fileGetSize(const Path* path) const {
    auto it = m_fileHeaders.find(path->getPath());
    return it != m_fileHeaders.end() ? it->second->size : 0;
}

uint32_t SharableBundleHeader::fileGetCompressedSize(const Path* path) const {
    auto it = m_fileHeaders.find(path->getPath());
    return it != m_fileHeaders.end() ? it->second->compressedSize : 0;
}

int64_t SharableBundleHeader::fileGetLastTimeWriteAccess(const Path* path) const {
    auto it = m_fileHeaders.find(path->getPath());
    return it != m_fileHeaders.end() ? it->second->timestamp : 0;
}

uint64_t SharableBundleHeader::fileGetPosition(const Path* path) const {
    auto it = m_fileHeaders.find(path->getPath());
    return it != m_fileHeaders.end() ? it->second->offset : 0;
}

FileHeaderRuntime* SharableBundleHeader::getHeaderRuntime(const Path* path) const {
    auto it = m_fileHeaders.find(path->getPath());
    return it != m_fileHeaders.end() ? it->second : nullptr;
}

Filepack::Filepack(Bundle* bundle, uint64_t offset, uint32_t size, uint32_t compressedSize, const Path* path)
    : m_bundle(bundle), m_offset(offset), m_size(size), m_compressedSize(compressedSize), 
      m_path(path->getPath()), m_isLoaded(false) {}

Filepack::~Filepack() = default;

bool Filepack::readFileToBuffer(uint32_t flags) {
    if (m_isLoaded) return true;
    
    m_buffer.resize(m_size);
    uint32_t bytesRead;
    if (m_bundle->fileRead(m_offset, m_buffer.data(), m_size, &bytesRead)) {
        m_isLoaded = true;
        return bytesRead == m_size;
    }
    return false;
}

uint32_t Filepack::read(uint8_t* buffer, uint32_t size, uint32_t offset) {
    if (!m_isLoaded && !readFileToBuffer(0)) return 0;
    
    uint32_t readSize = std::min(size, m_size - offset);
    if (readSize > 0) {
        std::memcpy(buffer, m_buffer.data() + offset, readSize);
    }
    return readSize;
}

BundleCache::BundleCache(File* file, uint32_t cacheSize)
    : m_file(file), m_cacheSize(cacheSize), m_cacheStart(0), m_cacheEnd(0), m_currentPosition(0) {
    m_cache.resize(cacheSize);
}

BundleCache::~BundleCache() = default;

bool BundleCache::seek(uint64_t position) {
    m_currentPosition = position;
    return loadCache(position);
}

uint32_t BundleCache::read(uint8_t* buffer, uint32_t size) {
    if (m_currentPosition >= m_cacheEnd || m_currentPosition < m_cacheStart) {
        if (!loadCache(m_currentPosition)) return -1;
    }
    
    uint64_t availableInCache = m_cacheEnd - m_currentPosition;
    uint32_t readSize = static_cast<uint32_t>(std::min(static_cast<uint64_t>(size), availableInCache));
    
    if (readSize > 0) {
        uint64_t cacheOffset = m_currentPosition - m_cacheStart;
        std::memcpy(buffer, m_cache.data() + cacheOffset, readSize);
        m_currentPosition += readSize;
    }
    
    return readSize;
}

uint64_t BundleCache::getCurrentPosition() const { return m_currentPosition; }
uint64_t BundleCache::getCacheStart() const { return m_cacheStart; }
uint64_t BundleCache::getCacheEnd() const { return m_cacheEnd; }
uint32_t BundleCache::getCacheSize() const { return m_cacheSize; }

bool BundleCache::loadCache(uint64_t position) {
    m_cacheStart = position;
    m_cacheEnd = position + m_cacheSize;
    return true;
}

FileMemoryShared::FileMemoryShared(uint32_t size)
    : m_size(size), m_position(0), m_isOpen(false) {
    if (size > 0) {
        m_data = std::make_unique<uint8_t[]>(size);
    }
}

FileMemoryShared::~FileMemoryShared() = default;

bool FileMemoryShared::open(const std::string& path, int mode, int flags) {
    if (m_size == 0) {
        std::ifstream file(path, std::ios::binary);
        if (!file) return false;
        file.seekg(0, std::ios::end);
        m_size = static_cast<uint32_t>(file.tellg());
        file.close();
        
        m_data = std::make_unique<uint8_t[]>(m_size);
        
        file.open(path, std::ios::binary);
        if (!file) return false;
        file.read(reinterpret_cast<char*>(m_data.get()), m_size);
        file.close();
    }
    
    m_isOpen = true;
    return true;
}

bool FileMemoryShared::read(uint8_t* buffer, uint32_t size) {
    if (!m_isOpen || m_position + size > m_size) return false;
    std::memcpy(buffer, m_data.get() + m_position, size);
    m_position += size;
    return true;
}

bool FileMemoryShared::write(uint32_t offset, const uint8_t* data, uint32_t size) {
    if (!m_isOpen || offset + size > m_size) return false;
    std::memcpy(m_data.get() + offset, data, size);
    return true;
}

uint64_t FileMemoryShared::getSize() const { return m_size; }
uint64_t FileMemoryShared::getPosition() const { return m_position; }

bool FileMemoryShared::seek(uint64_t position) {
    if (position > m_size) return false;
    m_position = position;
    return true;
}

void FileMemoryShared::close() {
    m_isOpen = false;
    m_position = 0;
}
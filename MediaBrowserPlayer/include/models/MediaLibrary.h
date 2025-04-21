#ifndef MEDIALIBRARY_H
#define MEDIALIBRARY_H

#include <vector>
#include <string>
#include "MediaFile.h"
#include "Playlist.h"
#include "../Constants.h"
#include "../services/MetadataService.h"

class MediaLibrary {
public:
    MediaLibrary();
    
    // Scan a directory for media files
    void scanDirectory(const std::string& directoryPath, bool recursive = true);
    
    // Scan a mounted USB device
    void scanUSBDevice(const std::string& mountPoint, bool recursive = true);
    
    // Get all media files
    const Playlist& getRoot() const;
    
    // Filter media files by type
    std::vector<MediaFile> getMediaFilesByType(Constants::FileType type) const;
    
    // Search media files by name
    std::vector<MediaFile> searchMediaFiles(const std::string& query) const;
    
    // Get media file by index
    MediaFile getMediaFile(size_t index) const;
    
    // Get total count of media files
    size_t getMediaFileCount() const;
    
    // Clear all media files
    void clear();
    
    // Add a single media file
    void addMediaFile(const MediaFile& file);
    
    // Update a media file's metadata
    bool updateMediaFileMetadata(size_t index, const Metadata& metadata);
    
private:
    Playlist root;
    MetadataService metadataService;
};

#endif // MEDIALIBRARY_H

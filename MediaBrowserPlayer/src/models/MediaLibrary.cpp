#include "../../include/models/MediaLibrary.h"
#include <taglib/fileref.h>
#include <taglib/audioproperties.h>
#include <algorithm>
#include <stdexcept>
#include <filesystem>

MediaLibrary::MediaLibrary() {
    root.setName("root");
}

void MediaLibrary::scanDirectory(const std::string& directoryPath, bool recursive) {

    // Xem directory co ton tai khong?
    if (!std::filesystem::exists(directoryPath)) {
        return;
    }
    
    std::filesystem::path dir(directoryPath);
    
    try {
        if (recursive) {
            for (const auto& entry : std::filesystem::recursive_directory_iterator(dir)) {
                if (entry.is_regular_file()) {
                    std::string path = entry.path().string();
                    Metadata scanMetadata = metadataService.extractMetadata(path);
                    Constants::FileType type = metadataService.detectMediaType(path);
                    MediaFile file(path, scanMetadata, type);

                    // Check if the file is a valid media file
                    if (file.getType() != Constants::FileType::UNKNOWN) {
                        root.addTrack(file);
                    }
                }
            }
        }
        else {
            for (const auto& entry : std::filesystem::directory_iterator(dir)) {
                if (entry.is_regular_file()) {
                    std::string path = entry.path().string();
                    Metadata scanMetadata = metadataService.extractMetadata(path);
                    Constants::FileType type = metadataService.detectMediaType(path);
                    MediaFile file(path, scanMetadata, type);
                    
                    // Check if the file is a valid media file
                    if (file.getType() != Constants::FileType::UNKNOWN) {
                        root.addTrack(file);
                    }
                }
            }
        }
    } catch (const std::filesystem::filesystem_error& e) {
        // Handle filesystem errors
    }
}

void MediaLibrary::scanUSBDevice(const std::string& mountPoint, bool recursive) {
    scanDirectory(mountPoint, recursive);
}

const Playlist& MediaLibrary::getRoot() const {
    return root;
}

std::vector<MediaFile> MediaLibrary::getMediaFilesByType(Constants::FileType type) const {
    std::vector<MediaFile> result;
    std::vector<MediaFile> mediaFiles = root.getTracks();
    
    for (const auto& file : mediaFiles) {
        if (file.getType() == type) {
            result.push_back(file);
        }
    }
    
    return result;
}

std::vector<MediaFile> MediaLibrary::searchMediaFiles(const std::string& query) const {
    std::vector<MediaFile> result;
    std::vector<MediaFile> mediaFiles = root.getTracks();
    std::string lowerQuery = query;
    std::transform(lowerQuery.begin(), lowerQuery.end(), lowerQuery.begin(), ::tolower);
    
    for (const auto& file : mediaFiles) {
        std::string fileName = file.getFileName();
        std::transform(fileName.begin(), fileName.end(), fileName.begin(), ::tolower);
        
        if (fileName.find(lowerQuery) != std::string::npos) {
            result.push_back(file);
        }
    }
    
    return result;
}

MediaFile MediaLibrary::getMediaFile(size_t index) const {
    std::vector<MediaFile> mediaFiles = root.getTracks();
    if (index < mediaFiles.size()) {
        return mediaFiles[index];
    }
    throw std::out_of_range("Media file index out of range");
}

size_t MediaLibrary::getMediaFileCount() const {
    return root.getTracks().size();
}

void MediaLibrary::clear() {
    root.clear();
}

void MediaLibrary::addMediaFile(const MediaFile& file) {
    root.addTrack(file);
}

bool MediaLibrary::updateMediaFileMetadata(size_t index, const Metadata& metadata) {
    std::vector<MediaFile> mediaFiles = root.getTracks();
    if (index < mediaFiles.size()) {
        mediaFiles[index].setMetadata(metadata);
        return true;
    }
    return false;
}

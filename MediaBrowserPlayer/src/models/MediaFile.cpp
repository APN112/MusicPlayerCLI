#include "../../include/models/MediaFile.h"
#include <filesystem>
#include <sstream>
#include <algorithm>

MediaFile::MediaFile() : type(Constants::FileType::UNKNOWN) {
}

MediaFile::MediaFile(const std::string& filePath) 
    : filePath(filePath), type(determineFileType(filePath)) {
    // Extract file name as metadata name
    metadata.setName(std::filesystem::path(filePath).filename().string());
}

MediaFile::MediaFile(const std::string& filePath, const Metadata& metadata, Constants::FileType type)
    : filePath(filePath), metadata(metadata), type(type) {
}

const std::string& MediaFile::getFilePath() const {
    return filePath;
}

void MediaFile::setFilePath(const std::string& filePath) {
    this->filePath = filePath;
    this->type = determineFileType(filePath);
}

const Metadata& MediaFile::getMetadata() const {
    return metadata;
}

void MediaFile::setMetadata(const Metadata& metadata) {
    this->metadata = metadata;
}

Constants::FileType MediaFile::getType() const {
    return type;
}

void MediaFile::setType(Constants::FileType type) {
    this->type = type;
}

std::string MediaFile::getFileName() const {
    return std::filesystem::path(filePath).filename().string();
}

bool MediaFile::exists() const {
    return std::filesystem::exists(filePath);
}

std::string MediaFile::toString() const {
    std::stringstream ss;
    ss << filePath << "|" << static_cast<int>(type) << "|" << metadata.toString();
    return ss.str();
}

MediaFile MediaFile::fromString(const std::string& serialized) {
    std::stringstream ss(serialized);
    std::string path, metadataStr;
    int typeInt;
    
    std::getline(ss, path, '|');
    ss >> typeInt;
    ss.ignore(); // Skip the '|'
    std::getline(ss, metadataStr);
    
    Constants::FileType type = static_cast<Constants::FileType>(typeInt);
    Metadata metadata = Metadata::fromString(metadataStr);
    
    return MediaFile(path, metadata, type);
}

Constants::FileType MediaFile::determineFileType(const std::string& filePath) {
    std::string extension = std::filesystem::path(filePath).extension().string();

    // Chuyen phan extension sang lowercase
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    
    // Check audio extensions
    for (const auto& audioExt : Constants::AUDIO_EXTENSIONS) {
        if (extension == audioExt) {
            return Constants::FileType::AUDIO;
        }
    }
    
    // Check video extensions
    for (const auto& videoExt : Constants::VIDEO_EXTENSIONS) {
        if (extension == videoExt) {
            return Constants::FileType::VIDEO;
        }
    }
    
    return Constants::FileType::UNKNOWN;
}

#include "../../include/services/FileService.h"
#include <filesystem>
#include <algorithm>
#include <fstream>
#include <stdexcept>
#include <unistd.h>
#include <limits.h>

FileService::FileService() {
}

FileService::~FileService() {
}

std::vector<std::string> FileService::findMediaFiles(const std::string& directoryPath, bool recursive) {
    std::vector<std::string> mediaFiles;
    
    try {
        if (recursive) {
            for (const auto& entry : std::filesystem::recursive_directory_iterator(directoryPath)) {
                if (entry.is_regular_file() && isMediaFile(entry.path().string())) {
                    mediaFiles.push_back(entry.path().string());
                }
            }
        } else {
            for (const auto& entry : std::filesystem::directory_iterator(directoryPath)) {
                if (entry.is_regular_file() && isMediaFile(entry.path().string())) {
                    mediaFiles.push_back(entry.path().string());
                }
            }
        }
    } catch (const std::filesystem::filesystem_error& e) {
        // Handle filesystem errors, possibly by logging or throwing
    }
    
    return mediaFiles;
}

bool FileService::isDirectory(const std::string& path) {
    return std::filesystem::is_directory(path);
}

bool FileService::fileExists(const std::string& path) {
    return std::filesystem::exists(path);
}

std::string FileService::getFileExtension(const std::string& filePath) {
    return std::filesystem::path(filePath).extension().string();
}

bool FileService::isMediaFile(const std::string& filePath) {
    return isAudioFile(filePath) || isVideoFile(filePath);
}

bool FileService::createDirectory(const std::string& path) {
    try {
        return std::filesystem::create_directories(path);
    } catch (const std::filesystem::filesystem_error& e) {
        return false;
    }
}

long FileService::getFileSize(const std::string& filePath) {
    try {
        return std::filesystem::file_size(filePath);
    } catch (const std::filesystem::filesystem_error& e) {
        return -1;
    }
}

std::string FileService::getFileName(const std::string& filePath) {
    return std::filesystem::path(filePath).filename().string();
}

std::string FileService::getDirectoryPath(const std::string& filePath) {
    return std::filesystem::path(filePath).parent_path().string();
}

std::string FileService::getAbsolutePath(const std::string& relativePath) {
    try {
        return std::filesystem::absolute(relativePath).string();
    } catch (const std::filesystem::filesystem_error& e) {
        return relativePath;
    }
}

std::string FileService::getCurrentDirectory() {
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) != nullptr) {
        return std::string(cwd);
    }
    return ".";
}

bool FileService::changeDirectory(const std::string& path) {
    return chdir(path.c_str()) == 0;
}

std::vector<std::string> FileService::listDirectories(const std::string& path) {
    std::vector<std::string> directories;
    
    try {
        for (const auto& entry : std::filesystem::directory_iterator(path)) {
            if (entry.is_directory()) {
                directories.push_back(entry.path().string());
            }
        }
    } catch (const std::filesystem::filesystem_error& e) {
        // Handle filesystem errors
    }
    
    return directories;
}

bool FileService::isAudioFile(const std::string& filePath) {
    std::string extension = getFileExtension(filePath);
    
    // Convert to lowercase for case-insensitive comparison
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    
    for (const auto& audioExt : Constants::AUDIO_EXTENSIONS) {
        if (extension == audioExt) {
            return true;
        }
    }
    
    return false;
}

bool FileService::isVideoFile(const std::string& filePath) {
    std::string extension = getFileExtension(filePath);
    
    // Convert to lowercase for case-insensitive comparison
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    
    for (const auto& videoExt : Constants::VIDEO_EXTENSIONS) {
        if (extension == videoExt) {
            return true;
        }
    }
    
    return false;
}

#ifndef FILESERVICE_H
#define FILESERVICE_H

#include <string>
#include <vector>
#include "../models/MediaFile.h"
#include "../Constants.h"

class FileService {
public:
    FileService();
    ~FileService();
    
    // Find all media files in a directory
    std::vector<std::string> findMediaFiles(const std::string& directoryPath, bool recursive = true);
    
    // Check if a path is a directory
    bool isDirectory(const std::string& path);
    
    // Check if a file exists
    bool fileExists(const std::string& path);
    
    // Get file extension
    std::string getFileExtension(const std::string& filePath);
    
    // Check if a file is a media file
    bool isMediaFile(const std::string& filePath);
    
    // Create directory if it doesn't exist
    bool createDirectory(const std::string& path);
    
    // Get file size
    long getFileSize(const std::string& filePath);
    
    // Get filename from path
    std::string getFileName(const std::string& filePath);
    
    // Get directory path from file path
    std::string getDirectoryPath(const std::string& filePath);
    
    // Get absolute path
    std::string getAbsolutePath(const std::string& relativePath);
    
    // Get the current working directory
    std::string getCurrentDirectory();
    
    // Change the current working directory
    bool changeDirectory(const std::string& path);
    
    // List directories
    std::vector<std::string> listDirectories(const std::string& path);
    
    // Check if file is audio
    bool isAudioFile(const std::string& filePath);
    
    // Check if file is video
    bool isVideoFile(const std::string& filePath);
};

#endif // FILESERVICE_H

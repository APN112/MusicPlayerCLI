#ifndef METADATASERVICE_H
#define METADATASERVICE_H

#include <string>
#include "../models/Metadata.h"
#include "../models/MediaFile.h"

class MetadataService {
public:
    MetadataService();
    ~MetadataService();
    
    // Extract metadata from a file
    Metadata extractMetadata(const std::string& filePath);
    
    // Update metadata in a file
    bool updateMetadata(const std::string& filePath, const Metadata& metadata);
    
    // Detect media type from file extension
    Constants::FileType detectMediaType(const std::string& filePath);
    
    // Get file duration in seconds
    double getFileDuration(const std::string& filePath);
    
    // Get file format details (codec, bitrate, etc.)
    std::map<std::string, std::string> getFormatDetails(const std::string& filePath);
    
private:
    // Initialize TagLib if needed
    void initializeTagLib();
    
    // Clean up TagLib resources
    void cleanupTagLib();
    
    // Extract audio file metadata
    Metadata extractAudioMetadata(const std::string& filePath);
    
    // Extract video file metadata
    Metadata extractVideoMetadata(const std::string& filePath);
    
    // Update audio file metadata
    bool updateAudioMetadata(const std::string& filePath, const Metadata& metadata);
    
    // // Update video file metadata
    // bool updateVideoMetadata(const std::string& filePath, const Metadata& metadata);
};

#endif // METADATASERVICE_H

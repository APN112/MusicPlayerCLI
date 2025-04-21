#ifndef MEDIAFILE_H
#define MEDIAFILE_H

#include <string>
#include "../Constants.h"
#include "Metadata.h"

class MediaFile {
public:
    MediaFile();
    MediaFile(const std::string& filePath);
    MediaFile(const std::string& filePath, const Metadata& metadata, Constants::FileType type);
    
    const std::string& getFilePath() const;
    void setFilePath(const std::string& filePath);
    
    const Metadata& getMetadata() const;
    void setMetadata(const Metadata& metadata);
    
    Constants::FileType getType() const;
    void setType(Constants::FileType type);
    
    // Extract file name from path
    std::string getFileName() const;
    
    // Check if the file exists
    bool exists() const;
    
    // Serialize to string
    std::string toString() const;
    
    // Deserialize from string
    static MediaFile fromString(const std::string& serialized);
    
private:
    std::string filePath;
    Metadata metadata;
    Constants::FileType type;
    
    // Determine file type based on extension
    Constants::FileType determineFileType(const std::string& filePath);
};

#endif // MEDIAFILE_H

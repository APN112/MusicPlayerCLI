#include "../../include/services/MetadataService.h"
#include "../../include/Constants.h"
#include <taglib/fileref.h>
#include <taglib/tag.h>
#include <taglib/tpropertymap.h>
#include <taglib/mpegfile.h>
#include <taglib/id3v2tag.h>
#include <taglib/flacfile.h>
#include <taglib/oggfile.h>
#include <taglib/vorbisfile.h>
#include <taglib/wavfile.h>
#include <filesystem>
#include <iostream>

MetadataService::MetadataService() {
    initializeTagLib();
}

MetadataService::~MetadataService() {
    cleanupTagLib();
}

Metadata MetadataService::extractMetadata(const std::string& filePath) {
    Constants::FileType type = detectMediaType(filePath);
    
    if (type == Constants::FileType::AUDIO) {
        return extractAudioMetadata(filePath);
    } else if (type == Constants::FileType::VIDEO) {
        return extractVideoMetadata(filePath);
    } else {
        // For unknown types, just return basic metadata with the filename
        std::string fileName = std::filesystem::path(filePath).filename().string();
        return Metadata(fileName);
    }
}

bool MetadataService::updateMetadata(const std::string& filePath, const Metadata& metadata) {
    Constants::FileType type = detectMediaType(filePath);
    
    if (type == Constants::FileType::AUDIO) {
        return updateAudioMetadata(filePath, metadata);
    } else if (type == Constants::FileType::VIDEO) {
        // return updateVideoMetadata(filePath, metadata);
        return false;
    }
    
    return false;
}

Constants::FileType MetadataService::detectMediaType(const std::string& filePath) {
    std::string extension = std::filesystem::path(filePath).extension().string();
    
    // Convert to lowercase for case-insensitive comparison
    for (auto& c : extension) c = std::tolower(c);
    
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

double MetadataService::getFileDuration(const std::string& filePath) {
    // Use TagLib to get audio duration
    if (detectMediaType(filePath) == Constants::FileType::AUDIO) {
        TagLib::FileRef f(filePath.c_str());
        if (!f.isNull() && f.audioProperties()) {
            return f.audioProperties()->lengthInSeconds();
        }
    }
    
    // For video files, this would need to be implemented with a video library
    // For now, return 0 for non-audio files
    return 0.0;
}

std::map<std::string, std::string> MetadataService::getFormatDetails(const std::string& filePath) {
    std::map<std::string, std::string> details;
    
    if (detectMediaType(filePath) == Constants::FileType::AUDIO) {
        TagLib::FileRef f(filePath.c_str());
        if (!f.isNull() && f.audioProperties()) {
            auto props = f.audioProperties();
            details["bitrate"] = std::to_string(props->bitrate()) + " kbps";
            details["channels"] = std::to_string(props->channels());
            details["sample_rate"] = std::to_string(props->sampleRate()) + " Hz";
            
            // Get file format
            std::string ext = std::filesystem::path(filePath).extension().string();
            for (auto& c : ext) c = std::tolower(c);
            if (ext == ".mp3") details["format"] = "MP3";
            else if (ext == ".wav") details["format"] = "WAV";
            else if (ext == ".ogg") details["format"] = "OGG Vorbis";
            else if (ext == ".flac") details["format"] = "FLAC";
            else details["format"] = ext.substr(1); // Remove the dot
        }
    }
    
    return details;
}

void MetadataService::initializeTagLib() {
    // TagLib doesn't require explicit initialization
}

void MetadataService::cleanupTagLib() {
    // TagLib doesn't require explicit cleanup
}

Metadata MetadataService::extractAudioMetadata(const std::string& filePath) {
    TagLib::FileRef f(filePath.c_str());
    
    if (f.isNull() || !f.tag()) {
        // If TagLib can't read the file, just return basic metadata with the filename
        std::string fileName = std::filesystem::path(filePath).filename().string();
        return Metadata(fileName);
    }
    
    TagLib::Tag* tag = f.tag();
    double duration = 0.0;
    
    if (f.audioProperties()) {
        duration = f.audioProperties()->lengthInSeconds();
    }
    
    std::string title = tag->title().to8Bit(true);
    if (title.empty()) {
        // Use filename as title if no title tag
        title = std::filesystem::path(filePath).filename().string();
    }
    
    Metadata metadata(title, duration);
    
    // Add standard tags
    if (!tag->artist().isEmpty()) {
        metadata.setAttribute(Constants::MetadataKeys::ARTIST, tag->artist().to8Bit(true));
    }
    
    if (!tag->album().isEmpty()) {
        metadata.setAttribute(Constants::MetadataKeys::ALBUM, tag->album().to8Bit(true));
    }
    
    if (tag->year() > 0) {
        metadata.setAttribute(Constants::MetadataKeys::YEAR, std::to_string(tag->year()));
    }
    
    if (!tag->genre().isEmpty()) {
        metadata.setAttribute(Constants::MetadataKeys::GENRE, tag->genre().to8Bit(true));
    }
    
    if (tag->track() > 0) {
        metadata.setAttribute(Constants::MetadataKeys::TRACK_NUMBER, std::to_string(tag->track()));
    }
    
    // Add audio format details
    if (f.audioProperties()) {
        auto props = f.audioProperties();
        metadata.setAttribute(Constants::MetadataKeys::BITRATE, std::to_string(props->bitrate()) + " kbps");
    }
    
    // Attempt to get extended properties
    try {
        TagLib::PropertyMap properties = f.file()->properties();
        for (const auto& property : properties) {
            const std::string key = property.first.to8Bit(true);
            if (!property.second.isEmpty()) {
                const std::string value = property.second.front().to8Bit(true);
                if (!key.empty() && !value.empty() && 
                    key != "TITLE" && key != "ARTIST" && key != "ALBUM" && 
                    key != "YEAR" && key != "GENRE" && key != "TRACKNUMBER") {
                    metadata.setAttribute(key, value);
                }
            }
        }
    } catch (...) {
        // Ignore errors when getting extended properties
    }
    
    return metadata;
}

Metadata MetadataService::extractVideoMetadata(const std::string& filePath) {
    // For video files, we'd need a video metadata library
    // For now, we'll just create basic metadata with the filename
    
    std::string fileName = std::filesystem::path(filePath).filename().string();
    Metadata metadata(fileName);
    
    // Add basic file information
    metadata.setAttribute(Constants::MetadataKeys::CODEC, "Unknown");
    metadata.setAttribute(Constants::MetadataKeys::BITRATE, "Unknown");
    
    return metadata;
}

bool MetadataService::updateAudioMetadata(const std::string& filePath, const Metadata& metadata) {
    TagLib::FileRef f(filePath.c_str());
    
    if (f.isNull() || !f.tag()) {
        return false;
    }
    
    TagLib::Tag* tag = f.tag();
    
    // Update standard tags
    tag->setTitle(TagLib::String(metadata.getName(), TagLib::String::UTF8));
    
    if (metadata.hasAttribute(Constants::MetadataKeys::ARTIST)) {
        tag->setArtist(TagLib::String(metadata.getAttribute(Constants::MetadataKeys::ARTIST), TagLib::String::UTF8));
    }
    
    if (metadata.hasAttribute(Constants::MetadataKeys::ALBUM)) {
        tag->setAlbum(TagLib::String(metadata.getAttribute(Constants::MetadataKeys::ALBUM), TagLib::String::UTF8));
    }
    
    if (metadata.hasAttribute(Constants::MetadataKeys::YEAR)) {
        try {
            tag->setYear(std::stoi(metadata.getAttribute(Constants::MetadataKeys::YEAR)));
        } catch (...) {
            // Ignore conversion errors
        }
    }
    
    if (metadata.hasAttribute(Constants::MetadataKeys::GENRE)) {
        tag->setGenre(TagLib::String(metadata.getAttribute(Constants::MetadataKeys::GENRE), TagLib::String::UTF8));
    }
    
    if (metadata.hasAttribute(Constants::MetadataKeys::TRACK_NUMBER)) {
        try {
            tag->setTrack(std::stoi(metadata.getAttribute(Constants::MetadataKeys::TRACK_NUMBER)));
        } catch (...) {
            // Ignore conversion errors
        }
    }
    
    // Update extended properties
    TagLib::PropertyMap properties;
    
    for (const auto& attr : metadata.getAllAttributes()) {
        if (attr.first != Constants::MetadataKeys::ARTIST &&
            attr.first != Constants::MetadataKeys::ALBUM &&
            attr.first != Constants::MetadataKeys::YEAR &&
            attr.first != Constants::MetadataKeys::GENRE &&
            attr.first != Constants::MetadataKeys::TRACK_NUMBER &&
            attr.first != Constants::MetadataKeys::DURATION &&
            attr.first != Constants::MetadataKeys::BITRATE) {
            
            TagLib::StringList values;
            values.append(TagLib::String(attr.second, TagLib::String::UTF8));
            properties.insert(TagLib::String(attr.first, TagLib::String::UTF8), values);
        }
    }
    
    try {
        if (!properties.isEmpty()) {
            f.file()->setProperties(properties);
        }
    } catch (...) {
        // Ignore errors when setting extended properties
    }
    
    // Save the file
    return f.save();
}

// bool MetadataService::updateVideoMetadata(const std::string& filePath, const Metadata& metadata) {
//     // For video files, we'd need a video metadata library
//     // For now, we'll return false to indicate that updating video metadata is not supported
//     filePath;
//     metadata;
//     return false;
// }

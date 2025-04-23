#ifndef PLAYLIST_H
#define PLAYLIST_H

#include <string>
#include <vector>
#include "MediaFile.h"

class Playlist {
public:
    Playlist();
    Playlist(const std::string& name);
    Playlist(const std::string& name, std::vector<MediaFile> trackList);
    
    const std::string& getName() const;
    void setName(const std::string& name);
    
    void addTrack(const MediaFile& track);
    void removeTrack(size_t index);
    void moveTrack(size_t fromIndex, size_t toIndex);
    
    const std::vector<MediaFile>& getTracks() const;
    MediaFile getTrack(size_t index) const;
    size_t getTrackCount() const;
    bool isEmpty() const;
    
    void clear();
    
    // Save playlist to file
    bool save(const std::string& directory = "") const;
    
    // Load playlist from file
    static Playlist load(const std::string& filePath);
    
    // Serialize to string
    std::string toString() const;
    
    // Get total playlist duration
    double getTotalDuration() const;
    std::string getTotalDurationString() const;
    
private:
    std::string name;
    std::vector<MediaFile> tracks;
};

#endif // PLAYLIST_H

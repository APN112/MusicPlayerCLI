#include "../../include/models/Playlist.h"
#include <fstream>
#include <filesystem>
#include <sstream>
#include <stdexcept>
#include "../../include/Constants.h"

Playlist::Playlist() : name("New Playlist") {
}

Playlist::Playlist(const std::string& name) : name(name) {
}

Playlist::Playlist(const std::string& name, std::vector<MediaFile> trackList) : name(name), tracks(trackList) {
}

const std::string& Playlist::getName() const {
    return name;
}

void Playlist::setName(const std::string& name) {
    this->name = name;
}

void Playlist::addTrack(const MediaFile& track) {
    tracks.push_back(track);
}

void Playlist::removeTrack(size_t index) {
    if (index < tracks.size()) {
        tracks.erase(tracks.begin() + index);
    }
}

void Playlist::moveTrack(size_t fromIndex, size_t toIndex) {
    if (fromIndex < tracks.size() && toIndex < tracks.size() && fromIndex != toIndex) {
        MediaFile track = tracks[fromIndex];
        tracks.erase(tracks.begin() + fromIndex);
        
        if (toIndex > fromIndex) {
            toIndex--; // Adjust for the removal
        }
        
        tracks.insert(tracks.begin() + toIndex, track);
    }
}

const std::vector<MediaFile>& Playlist::getTracks() const {
    return tracks;
}

MediaFile Playlist::getTrack(size_t index) const {
    if (index < tracks.size()) {
        return tracks[index];
    }
    throw std::out_of_range("Track index out of range");
}

size_t Playlist::getTrackCount() const {
    return tracks.size();
}

bool Playlist::isEmpty() const {
    return tracks.empty();
}

void Playlist::clear() {
    tracks.clear();
}

bool Playlist::save(const std::string& directory) const {
    std::string dir = directory.empty() ? Constants::PLAYLISTS_DIR : directory;
    
    // Create directory if it doesn't exist
    if (!std::filesystem::exists(dir)) {
        std::filesystem::create_directories(dir);
    }
    
    std::string filename = name + Constants::PLAYLIST_EXT;
    std::string filepath = dir + "/" + filename;
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Write playlist name
    file << name << std::endl;
    
    // Write number of tracks
    file << tracks.size() << std::endl;
    
    // Write each track
    for (const auto& track : tracks) {
        file << track.toString() << std::endl;
    }
    
    file.close();
    return true;
}

Playlist Playlist::load(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open playlist file: " + filePath);
    }
    
    std::string name;
    std::getline(file, name);
    
    Playlist playlist(name);
    
    size_t trackCount;
    file >> trackCount;
    file.ignore(); // Skip newline
    
    for (size_t i = 0; i < trackCount; ++i) {
        std::string trackStr;
        std::getline(file, trackStr);
        playlist.addTrack(MediaFile::fromString(trackStr));
    }
    
    file.close();
    return playlist;
}

std::string Playlist::toString() const {
    std::stringstream ss;
    ss << name << " (" << tracks.size() << " tracks, " << getTotalDurationString() << ")";
    return ss.str();
}

double Playlist::getTotalDuration() const {
    double total = 0.0;
    for (const auto& track : tracks) {
        total += track.getMetadata().getDuration();
    }
    return total;
}

std::string Playlist::getTotalDurationString() const {
    int totalSeconds = static_cast<int>(getTotalDuration());
    int hours = totalSeconds / 3600;
    int minutes = (totalSeconds % 3600) / 60;
    int seconds = totalSeconds % 60;
    
    std::stringstream ss;
    if (hours > 0) {
        ss << hours << ":";
    }
    ss << minutes << ":" << std::setw(2) << std::setfill('0') << seconds;
    return ss.str();
}

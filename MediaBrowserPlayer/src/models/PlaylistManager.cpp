#include "../../include/models/PlaylistManager.h"
#include <filesystem>
#include <stdexcept>
#include "../../include/Constants.h"

PlaylistManager::PlaylistManager() {
}

const std::vector<Playlist>& PlaylistManager::getPlaylists() const {
    return playlists;
}

Playlist PlaylistManager::getPlaylist(size_t index) const {
    if (index < playlists.size()) {
        return playlists[index];
    }
    throw std::out_of_range("Playlist index out of range");
}

Playlist* PlaylistManager::getPlaylistByName(const std::string& name) {
    for (auto& playlist : playlists) {
        if (playlist.getName() == name) {
            return &playlist;
        }
    }
    return nullptr;
}

void PlaylistManager::addPlaylist(const Playlist& playlist) {
    playlists.push_back(playlist);
}

bool PlaylistManager::updatePlaylist(size_t index, const Playlist& playlist) {
    if (index < playlists.size()) {
        playlists[index] = playlist;
        return true;
    }
    return false;
}

bool PlaylistManager::deletePlaylist(size_t index) {
    if (index < playlists.size()) {
        playlists.erase(playlists.begin() + index);
        return true;
    }
    return false;
}

bool PlaylistManager::playlistExists(const std::string& name) const {
    for (const auto& playlist : playlists) {
        if (playlist.getName() == name) {
            return true;
        }
    }
    return false;
}

void PlaylistManager::loadPlaylists(const std::string& directory) {
    playlists.clear();
    
    std::string dir = directory.empty() ? Constants::PLAYLISTS_DIR : directory;
    
    if (!std::filesystem::exists(dir)) {
        return;
    }
    
    for (const auto& entry : std::filesystem::directory_iterator(dir)) {
        if (entry.is_regular_file()) {
            std::string ext = entry.path().extension().string();
            if (ext == Constants::PLAYLIST_EXT) {
                try {
                    Playlist playlist = Playlist::load(entry.path().string());
                    playlists.push_back(playlist);
                } catch (const std::exception& e) {
                    // Skip invalid playlist files
                }
            }
        }
    }
}

void PlaylistManager::savePlaylists(const std::string& directory) const {
    std::string dir = directory.empty() ? Constants::PLAYLISTS_DIR : directory;
    
    // Create directory if it doesn't exist
    if (!std::filesystem::exists(dir)) {
        std::filesystem::create_directories(dir);
    }
    
    for (const auto& playlist : playlists) {
        playlist.save(dir);
    }
}

size_t PlaylistManager::getPlaylistCount() const {
    return playlists.size();
}

#ifndef PLAYLISTMANAGER_H
#define PLAYLISTMANAGER_H

#include <vector>
#include <string>
#include "Playlist.h"

class PlaylistManager {
public:
    PlaylistManager();
    
    // Get all playlists
    const std::vector<Playlist>& getPlaylists() const;
    
    // Get playlist by index
    Playlist getPlaylist(size_t index) const;
    
    // Get playlist by name
    Playlist* getPlaylistByName(const std::string& name);
    
    // Add new playlist
    void addPlaylist(const Playlist& playlist);
    
    // Update existing playlist
    bool updatePlaylist(size_t index, const Playlist& playlist);
    
    // Delete playlist
    bool deletePlaylist(size_t index);
    
    // Check if a playlist with the given name exists
    bool playlistExists(const std::string& name) const;
    
    // Load all playlists from the playlists directory
    void loadPlaylists(const std::string& directory = "");
    
    // Save all playlists to the playlists directory
    void savePlaylists(const std::string& directory = "") const;
    
    // Get count of playlists
    size_t getPlaylistCount() const;
    
private:
    std::vector<Playlist> playlists;
};

#endif // PLAYLISTMANAGER_H

#ifndef PLAYLISTCONTROLLER_H
#define PLAYLISTCONTROLLER_H

#include <memory>
#include "../models/PlaylistManager.h"
#include "PlayerController.h"
#include "MediaController.h"
#include "../views/PlaylistView.h"
#include "../services/FileService.h"
#include "../models/MediaLibrary.h"
#include "../views/IView.h"

class PlaylistController {
public:
    PlaylistController(std::shared_ptr<IView> parentView, std::shared_ptr<MediaLibrary> medLib,
        std::shared_ptr<PlaylistManager> playlistManager, std::shared_ptr<PlayerController> playerCtrl,
        std::shared_ptr<MediaController> mediaController);
    ~PlaylistController();
    
    // Set the media library
    void setMediaLibrary(MediaLibrary& mediaLibrary);
    
    // Show all playlists
    void showPlaylists();
    
    // Show a specific playlist with pagination
    void showPlaylist(size_t index, int page = 0);
    
    // Create a new playlist
    void createPlaylist();
    
    // Edit an existing playlist
    void editPlaylist(size_t index);
    
    // Delete a playlist
    void deletePlaylist(size_t index);
    
    // Add a track to a playlist
    void addTrackToPlaylist(size_t playlistIndex, size_t mediaFileIndex);
    
    // Remove a track from a playlist
    void removeTrackFromPlaylist(size_t playlistIndex, size_t trackIndex);
    
    // Move a track within a playlist
    void moveTrackInPlaylist(size_t playlistIndex, size_t fromIndex, size_t toIndex);
    
    // Show menu
    void showMenu();
    
    // Handle playlist menu option
    void handlePlaylistMenuOption(int option);
    
    // Get playlist manager
    std::shared_ptr<PlaylistManager>& getPlaylistManager();
    
private:
    // View
    std::shared_ptr<PlaylistView> playlistView;
    
    // Model
    std::shared_ptr<MediaLibrary> mediaLibrary;
    std::shared_ptr<PlaylistManager> playlistManager;

    // Controller
    FileService fileService;
    std::shared_ptr<PlayerController> playerController;
    std::shared_ptr<MediaController> mediaController;
    
    int currentPage;
    
    // Calculate total pages based on items per page
    int calculateTotalPages(const Playlist& playlist) const;
    
    // Display playlist options menu
    void showPlaylistOptionsMenu();
    
    // Load playlists from files
    void loadPlaylists();
    
    // Save playlists to files
    void savePlaylists();
};

#endif // PLAYLISTCONTROLLER_H

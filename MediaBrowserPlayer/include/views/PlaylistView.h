#ifndef PLAYLISTVIEW_H
#define PLAYLISTVIEW_H

#include "IView.h"
#include "../models/Playlist.h"
#include <vector>

class PlaylistView : public IView {
public:
    PlaylistView();
    
    // IView implementation
    void displayMessage(const std::string& message) override;
    void displayError(const std::string& message) override;
    std::string getInput(const std::string& prompt) override;
    int getMenuChoice(int min, int max) override;
    void clearScreen() override;
    void waitForInput() override;
    void displayHeader() override;
    void displayCurrentDirectory(const std::string& directory) override;
    void displayMainMenu() override;
    
    // Display list of playlists
    void displayPlaylists(const std::vector<Playlist>& playlists);

    // Display list of playlists to add
    void displayPlaylistsToAdd(const std::vector<Playlist>& playlists);
    
    // Display a single playlist's tracks with pagination
    void displayPlaylist(const Playlist& playlist, int page);
    
    // Display playlist creation form
    void displayPlaylistCreateForm();
    
    // Display playlist edit menu
    void displayPlaylistEditMenu(const Playlist& playlist);
    
    // Display playlist options menu
    void displayPlaylistOptionsMenu();
    
private:
    // Helper method for pagination controls
    void displayPaginationControls(int currentPage, int totalPages);
    
    // Format a track entry for display
    std::string formatTrackEntry(const MediaFile& track, int index);
};

#endif // PLAYLISTVIEW_H

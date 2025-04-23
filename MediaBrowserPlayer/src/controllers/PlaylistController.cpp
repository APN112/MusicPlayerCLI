#include "../../include/controllers/PlaylistController.h"
#include <iostream>
#include <cctype>
#include <cmath>
#include <filesystem>

PlaylistController::PlaylistController(std::shared_ptr<IView> parentView,
    std::shared_ptr<MediaLibrary> medLib, std::shared_ptr<PlaylistManager> playlistManager,
    std::shared_ptr<PlayerController> playerCtrl, std::shared_ptr<MediaController> mediaController)
    :  playlistView(std::make_shared<PlaylistView>()), mediaLibrary(medLib), playlistManager(playlistManager),
    fileService(), playerController(playerCtrl), mediaController(mediaController),
      currentPage(0) {
    // Convert parent view to PlaylistView if possible, otherwise create a new one
    if (parentView) {
        playlistView = std::dynamic_pointer_cast<PlaylistView>(parentView);
        if (!playlistView) {
            playlistView = std::make_shared<PlaylistView>();
        }
    }
    
    // Load existing playlists
    loadPlaylists();
}

PlaylistController::~PlaylistController() {
    // Save playlists before destruction
    savePlaylists();
}

void PlaylistController::setMediaLibrary(MediaLibrary& mediaLib) {
    mediaLibrary = std::make_shared<MediaLibrary>(mediaLib);
}

void PlaylistController::showPlaylists() {
    while(true){
        const auto& playlists = playlistManager->getPlaylists();
        
        playlistView->displayPlaylists(playlists);
        
        int choice = playlistView->getMenuChoice(0, 5);
        
        switch (choice) {
            case 1: { // View playlist
                std::string indexStr = playlistView->getInput("Enter playlist number: ");
                try {
                    int index = std::stoi(indexStr) - 1; // Convert to 0-based index
                    if (index >= 0 && index < static_cast<int>(playlists.size())) {
                        showPlaylist(index);
                    } else {
                        playlistView->displayError("Invalid playlist number");
                        playlistView->waitForInput();
                    }
                } catch (const std::exception& e) {
                    playlistView->displayError("Invalid input");
                    playlistView->waitForInput();
                }
                break;
            }
                
            case 2: // Create new playlist
                createPlaylist();
                continue;
                
            case 3: { // Edit playlist
                std::string indexStr = playlistView->getInput("Enter playlist number to edit: ");
                try {
                    int index = std::stoi(indexStr) - 1; // Convert to 0-based index
                    if (index >= 0 && index < static_cast<int>(playlists.size())) {
                        editPlaylist(index);
                    } else {
                        playlistView->displayError("Invalid playlist number");
                        playlistView->waitForInput();
                    }
                } catch (const std::exception& e) {
                    playlistView->displayError("Invalid input");
                    playlistView->waitForInput();
                }
                break;
            }
                
            case 4: { // Delete playlist
                std::string indexStr = playlistView->getInput("Enter playlist number to delete: ");
                try {
                    int index = std::stoi(indexStr) - 1; // Convert to 0-based index
                    if (index >= 0 && index < static_cast<int>(playlists.size())) {
                        deletePlaylist(index);
                    } else {
                        playlistView->displayError("Invalid playlist number");
                        playlistView->waitForInput();
                    }
                } catch (const std::exception& e) {
                    playlistView->displayError("Invalid input");
                    playlistView->waitForInput();
                }
                break;
            }
                
            case 5: { // Play playlist
                std::string indexStr = playlistView->getInput("Enter playlist number to play: ");
                try {
                    int index = std::stoi(indexStr) - 1; // Convert to 0-based index
                    if (index >= 0 && index < static_cast<int>(playlists.size())) {
                        Playlist playlist = playlistManager->getPlaylist(index);
                        playerController->getAudioState().setCurrentPlaylist(playlist);
                        playerController->getAudioState().setCurrentTrackIndex(0);
                        playerController->getAudioState().setPlayerState(Constants::PlayerState::PLAYING);
                        playerController->playPlaylist();
                        playlistView->displayMessage("Playing playlist: " + playlist.getName());
                        playlistView->waitForInput();
                    } else {
                        playlistView->displayError("Invalid playlist number");
                        playlistView->waitForInput();
                    }
                } catch (const std::exception& e) {
                    playlistView->displayError("Invalid input");
                    playlistView->waitForInput();
                }
                break;
            }
                
            case 0: // Back to main menu
                return;
        }
    }
}

void PlaylistController::showPlaylist(size_t index, int page) {
    const Playlist& playlist = playlistManager->getPlaylist(index);

    // Calculate total pages
    int totalPages = calculateTotalPages(playlist);

    // Ensure page is valid
    if (page < 0) page = 0;
    if (page >= totalPages) page = totalPages - 1;
    
    currentPage = page;

    while(true){
        try {
            // Display playlist with pagination
            playlistView->displayPlaylist(playlist, currentPage);
            
            char choice;
            std::string input = playlistView->getInput("\nEnter option (0-4) or P/N for pagination: ");
            
            if (input.empty()) {
                showPlaylist(index, currentPage);
                return;
            }
            
            choice = std::toupper(input[0]);
            
            if (choice == 'P' && currentPage > 0) {
                // Previous page
                currentPage--;
                continue;
            } else if (choice == 'N' && currentPage < totalPages - 1) {
                // Next page
                currentPage++;
                continue;
            } else if (std::isdigit(choice)) {
                int option = choice - '0';
                
                switch (option) {
                    case 1: // Play playlist
                        playerController->getAudioState().setCurrentPlaylist(playlist);
                        playerController->getAudioState().setCurrentTrackIndex(0);
                        playerController->getAudioState().setPlayerState(Constants::PlayerState::PLAYING);
                        playerController->playPlaylist();
                        playlistView->displayMessage("Playing playlist: " + playlist.getName());
                        playlistView->waitForInput();
                        break;
                        
                    case 2: // Edit playlist
                        editPlaylist(index);
                        break;
                        
                    case 3: { // View track details
                        std::string trackIndexStr = playlistView->getInput("Enter track number: ");
                        try {
                            int trackIndex = std::stoi(trackIndexStr) - 1; // Convert to 0-based index
                            if (trackIndex >= 0 && trackIndex < static_cast<int>(playlist.getTrackCount())) {
                                const MediaFile& track = playlist.getTrack(trackIndex);
                                    int mediaIndex = mediaController->MediaFileExists(track);
                                    if (mediaIndex != -1) {
                                        mediaController->showMediaFileDetailsInPlaylist(mediaIndex, index);
                                    } else {
                                        playlistView->displayError("Media file not found in library");
                                    }
                                playlistView->waitForInput();
                            } else {
                                playlistView->displayError("Invalid track number");
                                playlistView->waitForInput();
                            }
                        } catch (const std::exception& e) {
                            playlistView->displayError("Invalid input");
                            playlistView->waitForInput();
                        }
                        break;
                    }
                        
                    case 4: { // Change page
                        std::string pageStr = playlistView->getInput("Enter page number (1-" + std::to_string(totalPages) + "): ");
                        try {
                            int pageNum = std::stoi(pageStr) - 1; // Convert to 0-based index
                            if (pageNum >= 0 && pageNum < totalPages) {
                                currentPage = pageNum;
                                continue;
                            } else {
                                playlistView->displayError("Invalid page number");
                                playlistView->waitForInput();
                                continue;
                            }
                        } catch (const std::exception& e) {
                            playlistView->displayError("Invalid input");
                            playlistView->waitForInput();
                            continue;
                        }
                        break;
                    }
                        
                    case 0: // Back to playlists
                        return;
                        
                    default:
                        playlistView->displayError("Invalid option");
                        playlistView->waitForInput();
                        continue;
                }
            } else {
                playlistView->displayError("Invalid input");
                playlistView->waitForInput();
                continue;
            }
        } catch (const std::exception& e) {
            playlistView->displayError("Error displaying playlist: " + std::string(e.what()));
            playlistView->waitForInput();
            return;
        }
    }
}

void PlaylistController::createPlaylist() {
    playlistView->displayPlaylistCreateForm();
    
    std::string name = playlistView->getInput("Enter playlist name: ");
    
    if (name.empty()) {
        playlistView->displayError("Playlist name cannot be empty");
        playlistView->waitForInput();
        return;
    }
    
    // Check if playlist with this name already exists
    if (playlistManager->playlistExists(name)) {
        playlistView->displayError("A playlist with this name already exists");
        playlistView->waitForInput();
        return;
    }
    
    // Create new playlist
    Playlist newPlaylist(name);
    
    // Add to playlist manager
    playlistManager->addPlaylist(newPlaylist);
    
    // Save playlists
    savePlaylists();
    
    playlistView->displayMessage("Playlist created successfully");
    playlistView->waitForInput();
    
    return;
}

void PlaylistController::editPlaylist(size_t index) {
    try {
        Playlist playlist = playlistManager->getPlaylist(index);
        
        playlistView->displayPlaylistEditMenu(playlist);
        
        int choice = playlistView->getMenuChoice(0, 5);
        
        switch (choice) {
            case 1: { // Rename playlist
                std::string newName = playlistView->getInput("Enter new name: ");
                
                if (newName.empty()) {
                    playlistView->displayError("Playlist name cannot be empty");
                    playlistView->waitForInput();
                    editPlaylist(index);
                    return;
                }
                
                // Check if playlist with this name already exists (excluding the current one)
                if (playlistManager->playlistExists(newName) && 
                    playlist.getName() != newName) {
                    playlistView->displayError("A playlist with this name already exists");
                    playlistView->waitForInput();
                    editPlaylist(index);
                    return;
                }
                
                playlist.setName(newName);
                playlistManager->updatePlaylist(index, playlist);
                savePlaylists();
                
                playlistView->displayMessage("Playlist renamed successfully");
                playlistView->waitForInput();
                break;
            }
                
            case 2: { // Add tracks
                // Show media library for selection
                playlistView->displayMessage("Showing media library for track selection");
                
                // Get media files
                const auto& files = mediaLibrary->getRoot().getTracks();
                
                if (files.empty()) {
                    playlistView->displayError("No media files available");
                    playlistView->waitForInput();
                    break;
                }
                
                // Display simple list of files
                playlistView->clearScreen();
                playlistView->displayMessage("Available Media Files:");
                
                for (size_t i = 0; i < files.size(); i++) {
                    playlistView->displayMessage(std::to_string(i + 1) + ". " + files[i].getMetadata().getName());
                }
                
                std::string fileIndexStr = playlistView->getInput("\nEnter file number to add (0 to cancel): ");
                
                try {
                    int fileIndex = std::stoi(fileIndexStr) - 1; // Convert to 0-based index
                    
                    if (fileIndex == -1) {
                        // User cancelled
                        break;
                    }
                    
                    if (fileIndex >= 0 && fileIndex < static_cast<int>(files.size())) {
                        // Add the track to the playlist
                        playlist.addTrack(files[fileIndex]);
                        
                        // Update the playlist
                        playlistManager->updatePlaylist(index, playlist);
                        savePlaylists();
                        
                        playlistView->displayMessage("Track added to playlist");
                        playlistView->waitForInput();
                    } else {
                        playlistView->displayError("Invalid file number");
                        playlistView->waitForInput();
                    }
                } catch (const std::exception& e) {
                    playlistView->displayError("Invalid input");
                    playlistView->waitForInput();
                }
                break;
            }
                
            case 3: { // Remove tracks
                if (playlist.isEmpty()) {
                    playlistView->displayError("Playlist is empty");
                    playlistView->waitForInput();
                    break;
                }
                
                // Display simple list of tracks
                playlistView->clearScreen();
                playlistView->displayMessage("Tracks in Playlist:");
                
                const auto& tracks = playlist.getTracks();
                for (size_t i = 0; i < tracks.size(); i++) {
                    playlistView->displayMessage(std::to_string(i + 1) + ". " + tracks[i].getMetadata().getName());
                }
                
                std::string trackIndexStr = playlistView->getInput("\nEnter track number to remove (0 to cancel): ");
                
                try {
                    int trackIndex = std::stoi(trackIndexStr) - 1; // Convert to 0-based index
                    
                    if (trackIndex == -1) {
                        // User cancelled
                        break;
                    }
                    
                    if (trackIndex >= 0 && trackIndex < static_cast<int>(tracks.size())) {
                        // Remove the track from the playlist
                        playlist.removeTrack(trackIndex);
                        
                        // Update the playlist
                        playlistManager->updatePlaylist(index, playlist);
                        savePlaylists();
                        
                        playlistView->displayMessage("Track removed from playlist");
                        playlistView->waitForInput();
                    } else {
                        playlistView->displayError("Invalid track number");
                        playlistView->waitForInput();
                    }
                } catch (const std::exception& e) {
                    playlistView->displayError("Invalid input");
                    playlistView->waitForInput();
                }
                break;
            }
                
            case 4: { // Reorder tracks
                if (playlist.isEmpty() || playlist.getTrackCount() < 2) {
                    playlistView->displayError("Playlist has fewer than 2 tracks");
                    playlistView->waitForInput();
                    break;
                }
                
                // Display simple list of tracks
                playlistView->clearScreen();
                playlistView->displayMessage("Tracks in Playlist:");
                
                const auto& tracks = playlist.getTracks();
                for (size_t i = 0; i < tracks.size(); i++) {
                    playlistView->displayMessage(std::to_string(i + 1) + ". " + tracks[i].getMetadata().getName());
                }
                
                std::string fromIndexStr = playlistView->getInput("\nEnter track number to move (0 to cancel): ");
                
                try {
                    int fromIndex = std::stoi(fromIndexStr) - 1; // Convert to 0-based index
                    
                    if (fromIndex == -1) {
                        // User cancelled
                        break;
                    }
                    
                    if (fromIndex >= 0 && fromIndex < static_cast<int>(tracks.size())) {
                        std::string toIndexStr = playlistView->getInput("Enter new position (1-" + 
                                                                      std::to_string(tracks.size()) + "): ");
                        
                        try {
                            int toIndex = std::stoi(toIndexStr) - 1; // Convert to 0-based index
                            
                            if (toIndex >= 0 && toIndex < static_cast<int>(tracks.size())) {
                                // Move the track
                                playlist.moveTrack(fromIndex, toIndex);
                                
                                // Update the playlist
                                playlistManager->updatePlaylist(index, playlist);
                                savePlaylists();
                                
                                playlistView->displayMessage("Track moved successfully");
                                playlistView->waitForInput();
                            } else {
                                playlistView->displayError("Invalid position");
                                playlistView->waitForInput();
                            }
                        } catch (const std::exception& e) {
                            playlistView->displayError("Invalid input");
                            playlistView->waitForInput();
                        }
                    } else {
                        playlistView->displayError("Invalid track number");
                        playlistView->waitForInput();
                    }
                } catch (const std::exception& e) {
                    playlistView->displayError("Invalid input");
                    playlistView->waitForInput();
                }
                break;
            }
                
            case 5: { // Clear playlist
                std::string confirm = playlistView->getInput("Are you sure you want to clear this playlist? (y/n): ");
                
                if (confirm == "y" || confirm == "Y") {
                    playlist.clear();
                    
                    // Update the playlist
                    playlistManager->updatePlaylist(index, playlist);
                    savePlaylists();
                    
                    playlistView->displayMessage("Playlist cleared");
                    playlistView->waitForInput();
                }
                break;
            }
                
            case 0: // Save and return
                // Ensure the playlist is saved
                playlistManager->updatePlaylist(index, playlist);
                savePlaylists();
                break;
        }
        
    } catch (const std::exception& e) {
        playlistView->displayError("Error editing playlist: " + std::string(e.what()));
        playlistView->waitForInput();
    }
}

void PlaylistController::deletePlaylist(size_t index) {
    try {
        const Playlist& playlist = playlistManager->getPlaylist(index);
        
        std::string confirm = playlistView->getInput("Are you sure you want to delete playlist '" + 
                                                   playlist.getName() + "'? (y/n): ");
        
        if (confirm == "y" || confirm == "Y") {
            playlistManager->deletePlaylist(index);
            savePlaylists();
            
            playlistView->displayMessage("Playlist deleted");
            playlistView->waitForInput();
        }
        
        showPlaylists();
        
    } catch (const std::exception& e) {
        playlistView->displayError("Error deleting playlist: " + std::string(e.what()));
        playlistView->waitForInput();
    }
}

void PlaylistController::addTrackToPlaylist(size_t playlistIndex, size_t mediaFileIndex) {
    try {
        // Get the playlist
        Playlist playlist = playlistManager->getPlaylist(playlistIndex);
        
        // Get the media file
        const MediaFile& mediaFile = mediaLibrary->getMediaFile(mediaFileIndex);
        
        // Add the track to the playlist
        playlist.addTrack(mediaFile);
        
        // Update the playlist
        playlistManager->updatePlaylist(playlistIndex, playlist);
        savePlaylists();
        
        playlistView->displayMessage("Track added to playlist");
        playlistView->waitForInput();
        
    } catch (const std::exception& e) {
        playlistView->displayError("Error adding track to playlist: " + std::string(e.what()));
        playlistView->waitForInput();
    }
}

void PlaylistController::removeTrackFromPlaylist(size_t playlistIndex, size_t trackIndex) {
    try {
        // Get the playlist
        Playlist playlist = playlistManager->getPlaylist(playlistIndex);
        
        // Remove the track
        playlist.removeTrack(trackIndex);
        
        // Update the playlist
        playlistManager->updatePlaylist(playlistIndex, playlist);
        savePlaylists();
        
        playlistView->displayMessage("Track removed from playlist");
        playlistView->waitForInput();
        
    } catch (const std::exception& e) {
        playlistView->displayError("Error removing track from playlist: " + std::string(e.what()));
        playlistView->waitForInput();
    }
}

void PlaylistController::moveTrackInPlaylist(size_t playlistIndex, size_t fromIndex, size_t toIndex) {
    try {
        // Get the playlist
        Playlist playlist = playlistManager->getPlaylist(playlistIndex);
        
        // Move the track
        playlist.moveTrack(fromIndex, toIndex);
        
        // Update the playlist
        playlistManager->updatePlaylist(playlistIndex, playlist);
        savePlaylists();
        
        playlistView->displayMessage("Track moved successfully");
        playlistView->waitForInput();
        
    } catch (const std::exception& e) {
        playlistView->displayError("Error moving track in playlist: " + std::string(e.what()));
        playlistView->waitForInput();
    }
}

void PlaylistController::handlePlaylistMenuOption(int option) {
    switch (option) {
        case 1: // Show playlists
            showPlaylists();
            break;
            
        case 2: // Create new playlist
            createPlaylist();
            showPlaylists();
            break;
            
        case 3: { // Import playlist
            playlistView->displayMessage("Playlist import functionality not implemented");
            playlistView->waitForInput();
            break;
        }
            
        case 4: { // Export playlist
            playlistView->displayMessage("Playlist export functionality not implemented");
            playlistView->waitForInput();
            break;
        }
            
        case 0: // Back to main menu
            break;
            
        default:
            playlistView->displayError("Invalid playlist menu option");
            playlistView->waitForInput();
    }
}

std::shared_ptr<PlaylistManager>& PlaylistController::getPlaylistManager() {
    return playlistManager;
}

int PlaylistController::calculateTotalPages(const Playlist& playlist) const {
    int totalTracks = playlist.getTrackCount();
    return static_cast<int>(std::ceil(static_cast<double>(totalTracks) / Constants::ITEMS_PER_PAGE));
}

void PlaylistController::showPlaylistOptionsMenu() {
    playlistView->displayPlaylistOptionsMenu();
    
    int choice = playlistView->getMenuChoice(0, 4);
    
    handlePlaylistMenuOption(choice);
}

void PlaylistController::loadPlaylists() {
    playlistManager->loadPlaylists(Constants::PLAYLISTS_DIR);
}

void PlaylistController::savePlaylists() {
    playlistManager->savePlaylists(Constants::PLAYLISTS_DIR);
}

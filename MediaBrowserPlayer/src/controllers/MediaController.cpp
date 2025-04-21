#include "../../include/controllers/MediaController.h"
#include "../../include/models/Playlist.h"
#include <iostream>
#include <cctype>
#include <cmath>
#include <filesystem>
#include <stdexcept>

// Use filesystem namespace for current_path() calls
namespace fs = std::filesystem;

MediaController::MediaController(std::shared_ptr<IView> parentView, std::shared_ptr<MediaLibrary> medLib,
            std::shared_ptr<PlaylistManager> playlistManager, std::shared_ptr<PlayerController> playerCtrl)
    : mediaListView(std::make_shared<MediaListView>()), mediaLibrary(medLib),
    playlistManager(playlistManager), playerController(playerCtrl), currentPage(0)
{
    // Convert parent view to MediaListView and PlaylistView if possible, otherwise create a new one
    if (parentView) {
        mediaListView = std::dynamic_pointer_cast<MediaListView>(parentView);
        if (!mediaListView) {
            mediaListView = std::make_shared<MediaListView>();
        }
        playlistView = std::dynamic_pointer_cast<PlaylistView>(parentView);
        if (!playlistView) {
            playlistView = std::make_shared<PlaylistView>();
        }
    }
}

MediaController::~MediaController() {
}

void MediaController::scanCurrentDirectory(const std::string& directoryPath, bool recursive) {
    mediaListView->displayMessage("Scanning directory: " + directoryPath);
    
    // Clear existing media files
    mediaLibrary->clear();
    
    // Scan directory
    mediaLibrary->scanDirectory(directoryPath, recursive);
    
    mediaListView->displayMessage("Found " + std::to_string(mediaLibrary->getMediaFileCount()) + " media files.");
    mediaListView->waitForInput();
}

void MediaController::scanUSBDevice(const std::string& mountPoint, bool recursive) {
    mediaListView->displayMessage("Scanning USB device: " + mountPoint);
    
    // Clear existing media files
    mediaLibrary->clear();
    
    // Scan USB device
    mediaLibrary->scanUSBDevice(mountPoint, recursive);
    
    mediaListView->displayMessage("Found " + std::to_string(mediaLibrary->getMediaFileCount()) + " media files on USB device.");
    mediaListView->waitForInput();
}

void MediaController::showMediaLibrary(int page) {
    if (mediaLibrary->getRoot().isEmpty()) {
        mediaListView->displayMessage("Media library is empty. Try scanning a directory first.");
        mediaListView->waitForInput();
        return;
    }
    std::vector<MediaFile> files = mediaLibrary->getRoot().getTracks();

    // Ensure page is valid
    int totalPages = calculateTotalPages();
    if (page < 0) page = 0;
    if (page >= totalPages) page = totalPages - 1;
    
    currentPage = page;
    
    while (true) {
        // Display media files with pagination
        mediaListView->displayMediaFiles(files, currentPage);
        
        char choice;
        std::string input;
        
        // Handle pagination and options
        input = mediaListView->getInput("\nEnter option (0-5) or P/N for pagination: ");
        
        if (input.empty()) continue;
        
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
                case 1: { // View file details
                    std::string indexStr = mediaListView->getInput("Enter file number: ");
                    try {
                        int index = std::stoi(indexStr) - 1; // Convert to 0-based index
                        if (index >= 0 && index < static_cast<int>(files.size())) {
                            showMediaFileDetails(index);
                        } else {
                            mediaListView->displayError("Invalid file number");
                        }
                    } catch (const std::exception& e) {
                        mediaListView->displayError("Invalid input");
                    }
                    continue;
                }
                
                case 2: { // Search files
                    std::string query = mediaListView->getInput("Enter search term: ");
                    searchMediaFiles(query);
                    continue;
                }
                
                case 3: { // Filter by type
                    std::string typeStr = mediaListView->getInput("Filter by type (1=Audio, 2=Video): ");
                    try {
                        int typeChoice = std::stoi(typeStr);
                        if (typeChoice == 1) {
                            filterMediaFilesByType(Constants::FileType::AUDIO);
                        } else if (typeChoice == 2) {
                            filterMediaFilesByType(Constants::FileType::VIDEO);
                        } else {
                            mediaListView->displayError("Invalid type");
                        }
                    } catch (const std::exception& e) {
                        mediaListView->displayError("Invalid input");
                    }
                    continue;
                }
                
                case 4: { // Add to playlist
                    std::string typeStr = mediaListView->getInput("Enter file index to add to playlist: ");
                    try {
                        int index = std::stoi(typeStr) - 1; // Convert to 0-based index
                        if (index >= 0 && index < static_cast<int>(files.size())) {
                            showPlaylistsToAdd(index);
                        } else {
                            mediaListView->displayError("Invalid file number");
                        }
                    } catch (const std::exception& e) {
                        mediaListView->displayError("Invalid input");
                    }
                    continue;
                }
                
                case 5: { // Change page
                    std::string pageStr = mediaListView->getInput("Enter page number (1-" + std::to_string(totalPages) + "): ");
                    try {
                        int pageNum = std::stoi(pageStr) - 1; // Convert to 0-based index
                        if (pageNum >= 0 && pageNum < totalPages) {
                            currentPage = pageNum;
                        } else {
                            mediaListView->displayError("Invalid page number");
                        }
                    } catch (const std::exception& e) {
                        mediaListView->displayError("Invalid input");
                    }
                    continue;
                }
                
                case 0: // Back to main menu
                    return;
                
                default:
                    mediaListView->displayError("Invalid option");
            }
        } else {
            mediaListView->displayError("Invalid input");
        }
    }
}

void MediaController::showMediaFileDetails(size_t index) {
    try {
        const MediaFile& file = mediaLibrary->getMediaFile(index);
        
        // Display file details
        mediaListView->displayMediaFileDetails(file);
        
        int choice = mediaListView->getMenuChoice(0, 3);
        
        switch (choice) {
            case 1: // Edit metadata
                editMediaFileMetadata(index);
                break;
                
            case 2: { // Play this file
                // Set Audio State to the current file
                playerController->getAudioState().setCurrentPlaylist(mediaLibrary->getRoot());
                playerController->getAudioState().setCurrentTrackIndex(index);
                playerController->getAudioState().setCurrentPosition(0);
                playerController->getAudioState().setPlayerState(Constants::PlayerState::PLAYING);

                // Check if the file is playable
                if (playerController->playPlaylist()) {
                    mediaListView->displayMessage("Playing: " + file.getFileName());
                } else {
                    mediaListView->displayError("Failed to play file");
                }
                mediaListView->waitForInput();
                break;
            }
                
            case 3: { // Add to playlist
                showPlaylistsToAdd(index);
                break;
            }
                
            case 0: // Back to file list
                showMediaLibrary(currentPage);               
                break;
        }
    } catch (const std::exception& e) {
        mediaListView->displayError("Error displaying file details: " + std::string(e.what()));
        mediaListView->waitForInput();
    }
}

void MediaController::showMediaFileDetailsInPlaylist(size_t index, size_t playlistIndex) {
    try {
        const MediaFile& file = mediaLibrary->getMediaFile(index);
        
        // Display file details
        mediaListView->displayMediaFileDetails(file);
        
        int choice = mediaListView->getMenuChoice(0, 3);
        
        switch (choice) {
            case 1: // Edit metadata
                editMediaFileMetadata(index);
                break;
                
            case 2: { // Play this file
                // Set Audio State to the current file
                playerController->getAudioState().setCurrentPlaylist(mediaLibrary->getRoot());
                playerController->getAudioState().setCurrentTrackIndex(index);
                playerController->getAudioState().setCurrentPosition(0);
                playerController->getAudioState().setPlayerState(Constants::PlayerState::PLAYING);

                // Check if the file is playable
                if (playerController->playPlaylist()) {
                    mediaListView->displayMessage("Playing: " + file.getFileName());
                } else {
                    mediaListView->displayError("Failed to play file");
                }
                mediaListView->waitForInput();
                break;
            }
                
            case 3: { // Remove from playlist
                playlistManager->getPlaylist(playlistIndex).removeTrack(index);
                break;
            }
                
            case 0: // Back to playlist
                break;
        }
    } catch (const std::exception& e) {
        mediaListView->displayError("Error displaying file details: " + std::string(e.what()));
        mediaListView->waitForInput();
    }
}

void MediaController::editMediaFileMetadata(size_t index) {
    try {
        MediaFile file = mediaLibrary->getMediaFile(index);
        Metadata metadata = file.getMetadata();
        
        while (true) {
            mediaListView->displayMediaFileEditMenu(file);
            
            std::string choiceStr = mediaListView->getInput("");
            int choice;
            
            try {
                choice = std::stoi(choiceStr);
            } catch (const std::exception& e) {
                mediaListView->displayError("Invalid input");
                continue;
            }
            
            if (choice == 0) {
                // Save and return
                if (updateMetadata(index, metadata)) {
                    mediaListView->displayMessage("Metadata updated successfully");
                } else {
                    mediaListView->displayError("Failed to update metadata");
                }
                mediaListView->waitForInput();
                break;
            } else if (choice == 1) {
                // Edit title
                std::string newTitle = mediaListView->getInput("Enter new title: ");
                if (!newTitle.empty()) {
                    metadata.setName(newTitle);
                }
            } else {
                // Edit other attributes or add new attribute
                const auto& attributes = metadata.getAllAttributes();
                int attrIndex = choice - 2;
                
                if (attrIndex >= 0 && attrIndex < static_cast<int>(attributes.size())) {
                    // Edit existing attribute
                    auto it = attributes.begin();
                    std::advance(it, attrIndex);
                    
                    std::string newValue = mediaListView->getInput("Enter new value for " + it->first + ": ");
                    metadata.setAttribute(it->first, newValue);
                } else if (attrIndex == static_cast<int>(attributes.size())) {
                    // Add new attribute
                    std::string key = mediaListView->getInput("Enter attribute name: ");
                    if (!key.empty()) {
                        std::string value = mediaListView->getInput("Enter attribute value: ");
                        metadata.setAttribute(key, value);
                    }
                } else {
                    mediaListView->displayError("Invalid choice");
                }
            }
            
            // Update file with new metadata for display purposes
            file.setMetadata(metadata);
        }
        
        // Return to file details
        showMediaFileDetails(index);
        
    } catch (const std::exception& e) {
        mediaListView->displayError("Error editing metadata: " + std::string(e.what()));
        mediaListView->waitForInput();
    }
}

void MediaController::searchMediaFiles(const std::string& query) {
    if (query.empty()) {
        showMediaLibrary(0);
        return;
    }
    
    std::vector<MediaFile> results = mediaLibrary->searchMediaFiles(query);
    
    if (results.empty()) {
        mediaListView->displayMessage("No files found matching: " + query);
        mediaListView->waitForInput();
        showMediaLibrary(0);
        return;
    }
    
    // Create a temporary copy of the current media library
    std::vector<MediaFile> originalFiles = mediaLibrary->getRoot().getTracks();
    
    // Replace media library with search results for display
    mediaLibrary->clear();
    for (const auto& file : results) {
        mediaLibrary->addMediaFile(file);
    }
    
    // Display search results
    mediaListView->displayMessage("Showing " + std::to_string(results.size()) + " results for: " + query);
    showMediaLibrary(0);
    
    // Restore original media library
    mediaLibrary->clear();
    for (const auto& file : originalFiles) {
        mediaLibrary->addMediaFile(file);
    }
}

void MediaController::filterMediaFilesByType(Constants::FileType type) {
    std::vector<MediaFile> filtered = mediaLibrary->getMediaFilesByType(type);
    
    if (filtered.empty()) {
        mediaListView->displayMessage("No files found of the requested type");
        mediaListView->waitForInput();
        showMediaLibrary(0);
        return;
    }
    
    // Create a temporary copy of the current media library
    std::vector<MediaFile> originalFiles = mediaLibrary->getRoot().getTracks();
    
    // Replace media library with filtered results for display
    mediaLibrary->clear();
    for (const auto& file : filtered) {
        mediaLibrary->addMediaFile(file);
    }
    
    // Display filtered results
    std::string typeStr = (type == Constants::FileType::AUDIO) ? "Audio" : "Video";
    mediaListView->displayMessage("Showing " + std::to_string(filtered.size()) + " " + typeStr + " files");
    showMediaLibrary(0);
    
    // Restore original media library
    mediaLibrary->clear();
    for (const auto& file : originalFiles) {
        mediaLibrary->addMediaFile(file);
    }
}

void MediaController::handleMediaMenuOption(int option) {
    switch (option) {
        case 1: // Show media library
            showMediaLibrary(0);
            break;
            
        case 2: // Show media options menu
            showMediaOptionsMenu();
            break;
            
        default:
            mediaListView->displayError("Invalid media menu option");
            mediaListView->waitForInput();
    }
}

int MediaController::MediaFileExists(const MediaFile& file) const {
    // Check if the file exists in the media library
    for (size_t i = 0; i < mediaLibrary->getMediaFileCount(); ++i) {
        if (mediaLibrary->getMediaFile(i).getFilePath() == file.getFilePath()) {
            return static_cast<int>(i);
        }
    }
    return -1; // Not found
}

int MediaController::calculateTotalPages() const {
    int totalFiles = mediaLibrary->getMediaFileCount();
    return static_cast<int>(std::ceil(static_cast<double>(totalFiles) / Constants::ITEMS_PER_PAGE));
}

bool MediaController::updateMetadata(size_t index, const Metadata& metadata) {
    // First update the in-memory metadata
    bool updated = mediaLibrary->updateMediaFileMetadata(index, metadata);
    
    if (updated) {
        // Then try to update the actual file's metadata
        try {
            const MediaFile& file = mediaLibrary->getMediaFile(index);
            updated = metadataService.updateMetadata(file.getFilePath(), metadata);
        } catch (const std::exception& e) {
            mediaListView->displayError("Error updating file metadata: " + std::string(e.what()));
            return false;
        }
    }
    
    return updated;
}

void MediaController::showMediaOptionsMenu() {
    mediaListView->displayMediaOptionsMenu();
    
    int choice = mediaListView->getMenuChoice(0, 7);
    
    switch (choice) {
        case 1: // Scan current directory
            scanCurrentDirectory(fs::current_path().string(), false);
            showMediaLibrary(0);
            break;
            
        case 2: // Scan recursively
            scanCurrentDirectory(fs::current_path().string(), true);
            showMediaLibrary(0);
            break;
            
        case 3: { // Scan USB device
            mediaListView->displayMessage("This option should be accessed from the main menu");
            mediaListView->waitForInput();
            break;
        }
            
        case 4: // Show all media files
            showMediaLibrary(0);
            break;
            
        case 5: // Show audio files only
            filterMediaFilesByType(Constants::FileType::AUDIO);
            break;
            
        case 6: // Show video files only
            filterMediaFilesByType(Constants::FileType::VIDEO);
            break;
            
        case 7: { // Search by name
            std::string query = mediaListView->getInput("Enter search term: ");
            searchMediaFiles(query);
            break;
        }
            
        case 0: // Back to main menu
            break;
    }
}

void MediaController::browseFiles(const std::string& directoryPath) {
    try {
        mediaListView->clearScreen();
        mediaListView->displayHeader();
        mediaListView->displayCurrentDirectory(directoryPath);
        
        // Scan the directory for files
        scanCurrentDirectory(directoryPath, false);
        
        // Display the files
        showMediaLibrary(0);
    }
    catch (const std::exception& e) {
        mediaListView->displayError("Error browsing files: " + std::string(e.what()));
        mediaListView->waitForInput();
    }
}

void MediaController::showPlaylistsToAdd(int mediaIndex) {
    const auto& playlists = playlistManager->getPlaylists();
    
    playlistView->displayPlaylistsToAdd(playlists);
    
    int choice = playlistView->getMenuChoice(0, 2);
    
    switch (choice) {
        case 1: { // Choose a playlist
            std::string indexStr = playlistView->getInput("Enter playlist number: ");
            try {
                int index = std::stoi(indexStr) - 1; // Convert to 0-based index
                if (index >= 0 && index < static_cast<int>(playlists.size())) {
                    Playlist playlist = playlistManager->getPlaylist(index);
                    playlist.addTrack(mediaLibrary->getMediaFile(mediaIndex));
                    playlistManager->updatePlaylist(index, playlist);
                    playlistView->displayMessage("Track added to playlist: " + playlists[index].getName());
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
            
        case 2: {
            // Create new playlist
            std::string name = playlistView->getInput("Enter new playlist name: ");
            if (name.empty()) {
                playlistView->displayError("Playlist name cannot be empty");
                playlistView->waitForInput();
                return;
            }
            if (playlistManager->playlistExists(name)) {
                playlistView->displayError("Playlist already exists");
                playlistView->waitForInput();
                return;
            }
            Playlist newPlaylist(name);

            // Add the selected media file to the new playlist
            newPlaylist.addTrack(mediaLibrary->getMediaFile(mediaIndex));

            // Add the new playlist to the manager
            playlistManager->addPlaylist(newPlaylist);
            playlistView->displayMessage("Playlist created: " + name);
            playlistView->waitForInput();
            break;
        }
            
        case 0: // Back to main menu
            break;
    }
}
#ifndef MEDIACONTROLLER_H
#define MEDIACONTROLLER_H

#include <memory>
#include "../models/MediaLibrary.h"
#include "../models/PlaylistManager.h"
#include "../views/MediaListView.h"
#include "../views/PlaylistView.h"
#include "../controllers/PlayerController.h"
#include "../services/MetadataService.h"

class PlaylistController;

class MediaController {
public:
    MediaController(std::shared_ptr<IView> parentView, std::shared_ptr<MediaLibrary> medLib,
            std::shared_ptr<PlaylistManager> playlistManager,
            std::shared_ptr<PlayerController> playerCtrl);
    ~MediaController();
    
    // Browse files in a directory
    void browseFiles(const std::string& directoryPath);
    
    // Scan current directory for media files
    void scanCurrentDirectory(const std::string& directoryPath, bool recursive = true);
    
    // Scan a USB device for media files
    void scanUSBDevice(const std::string& mountPoint, bool recursive = true);
    
    // Show media library with pagination
    void showMediaLibrary(int page = 0);
    
    // Show media file details
    void showMediaFileDetails(size_t index);

    // Show media file details in playlist
    void showMediaFileDetailsInPlaylist(size_t index, size_t playlistIndex);
    
    // Edit media file metadata
    void editMediaFileMetadata(size_t index);
    
    // Search for media files
    void searchMediaFiles(const std::string& query);
    
    // Filter media files by type
    void filterMediaFilesByType(Constants::FileType type);
    
    // Handle media menu options
    void handleMediaMenuOption(int option);

    // Check if Media File exists and return its index
    int MediaFileExists(const MediaFile& file) const;
    
private:
    // View pointer
    std::shared_ptr<MediaListView> mediaListView;
    std::shared_ptr<PlaylistView> playlistView;

    // Model pointers
    std::shared_ptr<MediaLibrary> mediaLibrary;
    std::shared_ptr<PlaylistManager> playlistManager;

    // Controller pointers
    std::shared_ptr<PlayerController> playerController; // Use weak ptr to control lifetime of object when in bigger project
    MetadataService metadataService;
    
    int currentPage;
    
    // Calculate total pages based on items per page
    int calculateTotalPages() const;
    
    // Update media file metadata
    bool updateMetadata(size_t index, const Metadata& metadata);
    
    // Display media options menu
    void showMediaOptionsMenu();

    // Show playlist to add media file
    void showPlaylistsToAdd(int index);
};

#endif // MEDIACONTROLLER_H

#ifndef APPLICATIONCONTROLLER_H
#define APPLICATIONCONTROLLER_H

#include <memory>
#include "../views/IView.h"
#include "MediaController.h"
#include "PlaylistController.h"
#include "PlayerController.h"
#include "HardwareController.h"
#include "../models/MediaLibrary.h"
#include "../models/PlaylistManager.h"

class ApplicationController {
public:
    ApplicationController();
    ~ApplicationController();
    
    // Start the application
    void run();
    
    // Initialize application components
    bool initialize();
    
    // Shutdown application components
    void shutdown();
    
    // Method to show media Library
    void showMediaLibrary();

    // Method to show List of Playlsits
    void showListOfPlaylists();

    // Method to show now playing
    void nowPlaying();
    
    // Method to play a media file
    void playMediaFromDirectory();
    
    // Methods to change the current directory
    void changeDirectory();
    void changeDirectory(const std::string& directory);
    
    // Method to configure hardware settings
    void configureHardware();
    
    // Handle menu choice
    void handleMenuChoice(int choice);
    
private:
    // Views pointer
    std::shared_ptr<IView> view;

    // Controllers pointers
    std::shared_ptr<MediaController> mediaController;
    std::shared_ptr<PlaylistController> playlistController;
    std::shared_ptr<PlayerController> playerController;
    std::unique_ptr<HardwareController> hardwareController;

    // Model pointers
    std::shared_ptr<MediaLibrary> mediaLibrary;
    std::shared_ptr<PlaylistManager> playlistManager;
    
    // Current directory
    std::string currentDirectory;

    // Signal handler
    bool running;
    
    // Show main menu and get user choice
    int showMainMenu();
};

#endif // APPLICATIONCONTROLLER_H

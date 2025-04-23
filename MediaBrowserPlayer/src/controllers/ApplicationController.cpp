#include "../../include/controllers/ApplicationController.h"
#include "../../include/views/MainView.h"
#include <iostream>
#include <csignal>
#include <cstdlib>
#include <unistd.h>
#include <filesystem>

// Global pointer for signal handling
static ApplicationController* g_appController = nullptr;

// Signal handler function
void signalHandler(int signal) {
    if (g_appController) {
        std::cout << "\nReceived signal " << signal << ". Exiting gracefully..." << std::endl;
        g_appController->shutdown();
        exit(signal);
    }
}

ApplicationController::ApplicationController() : running(false) {
    // Set global pointer for signal handling
    g_appController = this;
    
    // Initialize current directory
    currentDirectory = std::filesystem::current_path().string();
    
    // Create views and controllers
    view = std::make_shared<MainView>();

    // Create media library
    mediaLibrary = std::make_shared<MediaLibrary>();

    // Create playlist list
    playlistManager = std::make_shared<PlaylistManager>();
    
    // Controllers will be initialized in the initialize() method
}

ApplicationController::~ApplicationController() {
    // Clean up resources
    shutdown();
}

void ApplicationController::run() {
    if (!initialize()) {
        view->displayError("Failed to initialize application components");
        return;
    }
    
    // Register signal handlers
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
    
    running = true;
    
    while (running) {
        int choice = showMainMenu();
        handleMenuChoice(choice);
    }
}

void ApplicationController::shutdown() {
    running = false;
    
    // Clean up controllers
    mediaController.reset();
    playlistController.reset();
    hardwareController.reset();
    playerController.reset();
    
    // Display shutdown message
    if (view) {
        view->displayMessage("\nShutting down...");
    }
}

bool ApplicationController::initialize() {
    try {
        //Input initial directory
        std::string inputDirectory;
        while (true) {
            view->clearScreen();
            view->displayHeader();
            inputDirectory = view->getInitialDirectory();
        
            if (std::filesystem::exists(inputDirectory) && std::filesystem::is_directory(inputDirectory)) {
                break; 
            } else {
                view->displayError("Invalid directory. Please try again.");
            }
        }
        currentDirectory = inputDirectory;

        // Create controllers - must create playerController first for HardwareController
        playerController = std::make_shared<PlayerController>(view); 
        
        // Need to create MediaController and HardwareController after PlayerController and need to pass a shared_ptr
        mediaController = std::make_shared<MediaController>(view, mediaLibrary, playlistManager, playerController);
        playlistController = std::make_shared<PlaylistController>(view, mediaLibrary, playlistManager, playerController, mediaController);
        hardwareController = std::make_unique<HardwareController>(playerController);

        // Initialize the Player controller
        playerController->initialize();
        
        // Initialize the media library with init directory
        mediaLibrary->scanDirectory(inputDirectory);
        
        // Connect to the hardware (if available)
        hardwareController->initialize();
        
        return true;
    }
    catch (const std::exception& e) {
        view->displayError(std::string("Initialization error: ") + e.what());
        return false;
    }
}

int ApplicationController::showMainMenu() {
    view->clearScreen();
    view->displayHeader();
    
    // Show current directory
    view->displayCurrentDirectory(currentDirectory);
    view->displayMainMenu();
    
    // Get user choice
    return view->getMenuChoice(0, 9);
}

void ApplicationController::handleMenuChoice(int choice) {
    switch (choice) {
        case 1: // Show Media Library
            showMediaLibrary();
            break;
        case 2: // Show List of Playlists
            showListOfPlaylists();
            break;
        case 3: // Now Playing
            nowPlaying();
            break;
        case 4: // Play
            playMediaFromDirectory();
            break;
        case 5: // Change Directory
            changeDirectory();
            break;
        case 6: // MountUSBDevice
            
            break;
        case 0: // Exit
            running = false;
            break;
        default:
            view->displayError("Invalid option. Please try again.");
            view->waitForInput();
            break;
    }
}

void ApplicationController::showMediaLibrary() {
    mediaController->showMediaLibrary();
}

void ApplicationController::showListOfPlaylists() {
    playlistController->showPlaylists();
}

void ApplicationController::nowPlaying() {
    if (playerController->isPlaying()) {
        playerController->showPlayer();
    } else {
        view->displayMessage("No media is currently playing.");
        view->waitForInput();
    }
}

void ApplicationController::playMediaFromDirectory() {
    // Check if media library is empty
    if(mediaLibrary->getMediaFileCount() == 0) {
        view->displayError("No media files found in the current directory.");
        view->waitForInput();
        return;
    }

    // Load Playlist
    playerController->getAudioState().setCurrentPlaylist(mediaLibrary->getRoot());
    playerController->getAudioState().setCurrentTrackIndex(0);
    
    // Play the media file
    if (!playerController->playPlaylist()) {
        view->displayError("Failed to play from root!");
    }
    
    view->displayMessage("Playing: " + mediaLibrary->getMediaFile(0).getFileName());
    view->waitForInput();
}

void ApplicationController::changeDirectory() {
    // Prompt for the new directory
    std::string newDir = view->getInput("Enter new directory (or leave empty to cancel): ");
    
    if (newDir.empty()) {
        return;
    }
    
    // Check if it's a relative path
    if (newDir[0] != '/') {
        // Append to current directory
        newDir = currentDirectory + "/" + newDir;
    }
    
    // Check if directory exists
    if (!std::filesystem::is_directory(newDir)) {
        view->displayError("Invalid directory: " + newDir);
        view->waitForInput();
        return;
    }
    
    // Update current directory
    currentDirectory = newDir;
    
    // Rescan the directory
    mediaLibrary->scanDirectory(currentDirectory);
    
    view->displayMessage("Directory changed to: " + currentDirectory);
    view->waitForInput();
}

void ApplicationController::changeDirectory(const std::string& directory) {
    // Check if directory exists
    if (!std::filesystem::is_directory(directory)) {
        view->displayError("Invalid directory: " + directory);
        return;
    }
    
    // Update current directory
    currentDirectory = directory;
    
    // Rescan the directory
    mediaLibrary->scanDirectory(currentDirectory);
    
    view->displayMessage("Directory changed to: " + currentDirectory);
}

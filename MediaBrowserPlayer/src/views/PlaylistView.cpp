#include "../../include/views/PlaylistView.h"
#include "../../include/Constants.h"
#include <iostream>
#include <iomanip>
#include <limits>
#include <cstdlib>

PlaylistView::PlaylistView() {
}

void PlaylistView::displayMessage(const std::string& message) {
    std::cout << message << std::endl;
}

void PlaylistView::displayError(const std::string& message) {
    std::cerr << "ERROR: " << message << std::endl;
}

std::string PlaylistView::getInput(const std::string& prompt) {
    std::string input;
    std::cout << prompt;
    std::getline(std::cin, input);
    return input;
}

int PlaylistView::getMenuChoice(int min, int max) {
    int choice;
    while (true) {
        std::cout << "Enter your choice (" << min << "-" << max << "): ";
        std::cin >> choice;
        
        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid input. Please enter a number." << std::endl;
        } else if (choice < min || choice > max) {
            std::cout << "Choice out of range. Please try again." << std::endl;
        } else {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return choice;
        }
    }
}

void PlaylistView::clearScreen() {
    #ifdef _WIN32
        if(std::system("cls") != 0){
            displayError("Failed to clear the terminal!");
        }
    #else
        if(std::system("clear") != 0){
            displayError("Failed to clear the terminal!");
        }
    #endif
}

void PlaylistView::waitForInput() {
    std::cout << "\nPress Enter to continue...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

void PlaylistView::displayPlaylists(const std::vector<Playlist>& playlists) {
    clearScreen();
    
    std::cout << std::string(80, '=') << std::endl;
    std::cout << std::setw(40) << std::right << "Playlists" << std::endl;
    std::cout << std::string(80, '=') << std::endl;
    
    if (playlists.empty()) {
        std::cout << "\nNo playlists found." << std::endl;
    } else {
        // Playlist header
        std::cout << std::left << std::setw(4) << "#" 
                  << std::setw(40) << "Name" 
                  << std::setw(15) << "Tracks" 
                  << std::setw(10) << "Duration" 
                  << std::endl;
        std::cout << std::string(80, '-') << std::endl;
        
        // List playlists
        for (size_t i = 0; i < playlists.size(); ++i) {
            const Playlist& playlist = playlists[i];
            std::cout << std::left << std::setw(4) << (i + 1) 
                      << std::setw(40) << (playlist.getName().length() > 37 
                                          ? playlist.getName().substr(0, 37) + "..." 
                                          : playlist.getName())
                      << std::setw(15) << playlist.getTrackCount() 
                      << std::setw(10) << playlist.getTotalDurationString() 
                      << std::endl;
        }
    }
    
    std::cout << "\nOptions:" << std::endl;
    std::cout << "  1. View playlist" << std::endl;
    std::cout << "  2. Create new playlist" << std::endl;
    std::cout << "  3. Edit playlist" << std::endl;
    std::cout << "  4. Delete playlist" << std::endl;
    std::cout << "  5. Play playlist" << std::endl;
    std::cout << "  0. Back to main menu" << std::endl;
}

void PlaylistView::displayPlaylistsToAdd(const std::vector<Playlist>& playlists){
    clearScreen();
    
    std::cout << std::string(80, '=') << std::endl;
    std::cout << std::setw(40) << std::right << "Playlists" << std::endl;
    std::cout << std::string(80, '=') << std::endl;
    
    if (playlists.empty()) {
        std::cout << "\nNo playlists found." << std::endl;
    } else {
        // Playlist header
        std::cout << std::left << std::setw(4) << "#" 
                  << std::setw(40) << "Name" 
                  << std::setw(15) << "Tracks" 
                  << std::setw(10) << "Duration" 
                  << std::endl;
        std::cout << std::string(80, '-') << std::endl;
        
        // List playlists
        for (size_t i = 0; i < playlists.size(); ++i) {
            const Playlist& playlist = playlists[i];
            std::cout << std::left << std::setw(4) << (i + 1) 
                      << std::setw(40) << (playlist.getName().length() > 37 
                                          ? playlist.getName().substr(0, 37) + "..." 
                                          : playlist.getName())
                      << std::setw(15) << playlist.getTrackCount() 
                      << std::setw(10) << playlist.getTotalDurationString() 
                      << std::endl;
        }
    }
    
    std::cout << "\nOptions:" << std::endl;
    std::cout << "  1. Choose a playlist" << std::endl;
    std::cout << "  2. Create a new playlist" << std::endl;
    std::cout << "  0. Back to main menu" << std::endl;
}

void PlaylistView::displayPlaylist(const Playlist& playlist, int page) {
    clearScreen();
    
    std::cout << std::string(80, '=') << std::endl;
    std::cout << std::setw(40) << std::right << "Playlist: " + playlist.getName() << std::endl;
    std::cout << std::string(80, '=') << std::endl;
    
    // Calculate pagination values
    const auto& tracks = playlist.getTracks();
    int totalTracks = tracks.size();
    int totalPages = (totalTracks + Constants::ITEMS_PER_PAGE - 1) / Constants::ITEMS_PER_PAGE;
    int startIndex = page * Constants::ITEMS_PER_PAGE;
    int endIndex = std::min(startIndex + Constants::ITEMS_PER_PAGE, totalTracks);
    
    if (totalTracks == 0) {
        std::cout << "\nThis playlist is empty." << std::endl;
    } else {
        // Track list header
        std::cout << std::left << std::setw(4) << "#" 
                  << std::setw(40) << "Title" 
                  << std::setw(15) << "Type" 
                  << std::setw(10) << "Duration" 
                  << std::endl;
        std::cout << std::string(80, '-') << std::endl;
        
        // List tracks for current page
        for (int i = startIndex; i < endIndex; ++i) {
            std::cout << formatTrackEntry(tracks[i], i) << std::endl;
        }
        
        std::cout << "\nTotal tracks: " << totalTracks << std::endl;
        std::cout << "Total duration: " << playlist.getTotalDurationString() << std::endl;
        
        std::cout << std::endl;
        displayPaginationControls(page, totalPages);
    }
    
    std::cout << "\nOptions:" << std::endl;
    std::cout << "  1. Play playlist" << std::endl;
    std::cout << "  2. Edit playlist" << std::endl;
    std::cout << "  3. View track details" << std::endl;
    std::cout << "  4. Change page" << std::endl;
    std::cout << "  0. Back to playlists" << std::endl;
}

void PlaylistView::displayPlaylistCreateForm() {
    clearScreen();
    
    std::cout << std::string(80, '=') << std::endl;
    std::cout << std::setw(40) << std::right << "Create New Playlist" << std::endl;
    std::cout << std::string(80, '=') << std::endl;
}

void PlaylistView::displayPlaylistEditMenu(const Playlist& playlist) {
    clearScreen();
    
    std::cout << std::string(80, '=') << std::endl;
    std::cout << std::setw(40) << std::right << "Edit Playlist: " + playlist.getName() << std::endl;
    std::cout << std::string(80, '=') << std::endl;
    
    std::cout << "1. Rename playlist" << std::endl;
    std::cout << "2. Add tracks" << std::endl;
    std::cout << "3. Remove tracks" << std::endl;
    std::cout << "4. Reorder tracks" << std::endl;
    std::cout << "5. Clear playlist" << std::endl;
    std::cout << "0. Save and return" << std::endl;
}

void PlaylistView::displayPlaylistOptionsMenu() {
    clearScreen();
    
    std::cout << std::string(80, '=') << std::endl;
    std::cout << std::setw(40) << std::right << "Playlist Options" << std::endl;
    std::cout << std::string(80, '=') << std::endl;
    
    std::cout << "1. Show all playlists" << std::endl;
    std::cout << "2. Create new playlist" << std::endl;
    std::cout << "3. Import playlist" << std::endl;
    std::cout << "4. Export playlist" << std::endl;
    std::cout << "0. Back to main menu" << std::endl;
}

void PlaylistView::displayPaginationControls(int currentPage, int totalPages) {
    std::cout << "Page " << (currentPage + 1) << " of " << totalPages << std::endl;
    
    if (currentPage > 0) {
        std::cout << "  [P] Previous page" << std::endl;
    }
    
    if (currentPage < totalPages - 1) {
        std::cout << "  [N] Next page" << std::endl;
    }
}

std::string PlaylistView::formatTrackEntry(const MediaFile& track, int index) {
    std::stringstream ss;
    
    // Format the listing with fixed width columns
    ss << std::left << std::setw(4) << (index + 1) 
       << std::setw(40) << (track.getMetadata().getName().length() > 37 
                            ? track.getMetadata().getName().substr(0, 37) + "..." 
                            : track.getMetadata().getName())
       << std::setw(15) << (track.getType() == Constants::FileType::AUDIO ? "Audio" : "Video") 
       << std::setw(10) << track.getMetadata().getDurationString();
    
    return ss.str();
}
void PlaylistView::displayHeader() {
    clearScreen();
    std::cout << std::string(80, '*') << std::endl;
    std::cout << std::setw(40) << std::right << "Playlist Management" << std::endl;
    std::cout << std::string(80, '*') << std::endl;
    std::cout << std::endl;
}

void PlaylistView::displayCurrentDirectory(const std::string& directory) {
    std::cout << "Current Directory: " << directory << std::endl;
    std::cout << std::string(80, '-') << std::endl;
}

void PlaylistView::displayMainMenu() {
    std::cout << "\nMain Menu:" << std::endl;
    std::cout << "  1. Browse Media Files" << std::endl;
    std::cout << "  2. Play Media" << std::endl;
    std::cout << "  3. Manage Playlists" << std::endl;
    std::cout << "  4. Change Directory" << std::endl;
    std::cout << "  5. Hardware Controls" << std::endl;
    std::cout << "  6. Help" << std::endl;
    std::cout << "  7. About" << std::endl;
    std::cout << "  0. Exit" << std::endl;
    std::cout << std::endl;
}

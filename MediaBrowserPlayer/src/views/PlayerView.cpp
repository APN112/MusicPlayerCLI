#include "../../include/views/PlayerView.h"
#include <iostream>
#include <iomanip>
#include <limits>
#include <cstdlib>
#include <thread>
#include <chrono>

PlayerView::PlayerView() {
}

void PlayerView::displayMessage(const std::string& message) {
    std::cout << message << std::endl;
}

void PlayerView::displayError(const std::string& message) {
    std::cerr << "ERROR: " << message << std::endl;
}

std::string PlayerView::getInput(const std::string& prompt) {
    std::string input;
    std::cout << prompt;
    std::getline(std::cin, input);
    return input;
}

int PlayerView::getMenuChoice(int min, int max) {
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

void PlayerView::clearScreen() {
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

void PlayerView::waitForInput() {
    std::cout << "\nPress Enter to continue...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

void PlayerView::displayPlayer(const AudioState& audioState) {
    clearScreen();
    
    std::cout << std::string(80, '=') << std::endl;
    std::cout << std::setw(40) << std::right << "Now Playing" << std::endl;
    std::cout << std::string(80, '=') << std::endl;
    
    if (audioState.hasValidTrack()) {
        const MediaFile& currentTrack = audioState.getCurrentTrack();
        const Metadata& metadata = currentTrack.getMetadata();
        
        displayNowPlaying(currentTrack);
        
        // Display progress bar
        double duration = metadata.getDuration();
        double position = audioState.getCurrentPosition();
        displayProgressBar(position, duration);
        
        // Display current position and duration
        std::cout << formatTime(position) << " / " << formatTime(duration) << std::endl;
        
        // Display volume
        displayVolume(audioState.getVolume());
        
        // Display player state
        std::string stateString;
        switch (audioState.getPlayerState()) {
            case Constants::PlayerState::PLAYING:
                stateString = "Playing";
                break;
            case Constants::PlayerState::PAUSED:
                stateString = "Paused";
                break;
            case Constants::PlayerState::STOPPED:
                stateString = "Stopped";
                break;
        }
        std::cout << "State: " << stateString << std::endl;
        
        // Display playlist info
        std::cout << "\nPlaylist: " << audioState.getCurrentPlaylist().getName() << std::endl;
        std::cout << "Track " << (audioState.getCurrentTrackIndex() + 1) << " of " 
                  << audioState.getCurrentPlaylist().getTrackCount() << std::endl;
    } else {
        std::cout << "\nNo track is currently playing." << std::endl;
    }
    
    displayPlayerControls();
}

void PlayerView::displayPlayerControls() {
    std::cout << "\nControls:" << std::endl;
    std::cout << "  [Space] Play/Pause" << std::endl;
    std::cout << "  [S] Stop" << std::endl;
    std::cout << "  [N] Next track" << std::endl;
    std::cout << "  [P] Previous track" << std::endl;
    std::cout << "  [+] Volume up" << std::endl;
    std::cout << "  [-] Volume down" << std::endl;
    std::cout << "  [Q] Back to main menu" << std::endl;
    std::cout << "Enter Command: " << std::endl;
}

void PlayerView::displayProgressBar(double current, double total) {
    double percentage = (total > 0) ? (current / total) * 100.0 : 0.0;
    std::cout << drawProgressBar(percentage, 60) << std::endl;
}

void PlayerView::displayVolume(int volume) {
    std::cout << "Volume: " << drawVolumeBar(volume, 20) << " " << volume << "%" << std::endl;
}

void PlayerView::displayNowPlaying(const MediaFile& track) {
    const Metadata& metadata = track.getMetadata();
    
    std::cout << "\nTitle: " << metadata.getName() << std::endl;
    
    // Display artist if available
    std::string artist = metadata.getAttribute(Constants::MetadataKeys::ARTIST);
    if (!artist.empty()) {
        std::cout << "Artist: " << artist << std::endl;
    }
    
    // Display album if available
    std::string album = metadata.getAttribute(Constants::MetadataKeys::ALBUM);
    if (!album.empty()) {
        std::cout << "Album: " << album << std::endl;
    }
    
    // Display year if available
    std::string year = metadata.getAttribute(Constants::MetadataKeys::YEAR);
    if (!year.empty()) {
        std::cout << "Year: " << year << std::endl;
    }
    
    std::cout << "File: " << track.getFileName() << std::endl;
    std::cout << std::endl;
}

void PlayerView::displayQueue(const Playlist& playlist, int currentIndex) {
    const auto& tracks = playlist.getTracks();
    int tracksToShow = 5; // Show 5 tracks before and after current
    
    std::cout << "\nQueue:" << std::endl;
    
    int startIdx = std::max(0, currentIndex - tracksToShow);
    int endIdx = std::min(static_cast<int>(tracks.size()), currentIndex + tracksToShow + 1);
    
    for (int i = startIdx; i < endIdx; ++i) {
        const MediaFile& track = tracks[i];
        std::string prefix = (i == currentIndex) ? "▶ " : "  ";
        
        std::cout << prefix << (i + 1) << ". " << track.getMetadata().getName() 
                  << " (" << track.getMetadata().getDurationString() << ")" << std::endl;
    }
}

void PlayerView::flashMessage(const std::string& message) {
    std::cout << "\r" << message << std::string(20, ' ') << '\n' << std::flush;
    std::this_thread::sleep_for(std::chrono::milliseconds(800));
}

std::string PlayerView::formatTime(double seconds) {
    int totalSeconds = static_cast<int>(seconds);
    int minutes = totalSeconds / 60;
    int secs = totalSeconds % 60;
    
    std::stringstream ss;
    ss << minutes << ":" << std::setw(2) << std::setfill('0') << secs;
    return ss.str();
}

std::string PlayerView::drawProgressBar(double percentage, int width) {
    int pos = static_cast<int>((width * percentage) / 100.0);
    
    std::string bar;
    bar += "[";
    for (int i = 0; i < width; ++i) {
        if (i < pos) bar += "=";
        else if (i == pos) bar += ">";
        else bar += " ";
    }
    bar += "] " + std::to_string(static_cast<int>(percentage)) + "%";
    
    return bar;
}

std::string PlayerView::drawVolumeBar(int volume, int width) {
    int pos = static_cast<int>((width * volume) / 100.0);
    
    std::string bar;
    bar += "[";
    for (int i = 0; i < width; ++i) {
        if (i < pos) bar += "█";
        else bar += " ";
    }
    bar += "]";
    
    return bar;
}
void PlayerView::displayHeader() {
    clearScreen();
    std::cout << std::string(80, '*') << std::endl;
    std::cout << std::setw(40) << std::right << "Media Player Application" << std::endl;
    std::cout << std::string(80, '*') << std::endl;
    std::cout << std::endl;
}

void PlayerView::displayCurrentDirectory(const std::string& directory) {
    std::cout << "Current Directory: " << directory << std::endl;
    std::cout << std::string(80, '-') << std::endl;
}

void PlayerView::displayMainMenu() {
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

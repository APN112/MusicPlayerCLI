#include "../../include/views/MediaListView.h"
#include "../../include/Constants.h"
#include <iostream>
#include <iomanip>
#include <limits>
#include <cstdlib>

MediaListView::MediaListView() {
}

void MediaListView::displayMessage(const std::string& message) {
    std::cout << message << std::endl;
}

void MediaListView::displayError(const std::string& message) {
    std::cerr << "ERROR: " << message << std::endl;
}

std::string MediaListView::getInput(const std::string& prompt) {
    std::string input;
    std::cout << prompt;
    std::getline(std::cin, input);
    return input;
}

int MediaListView::getMenuChoice(int min, int max) {
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

void MediaListView::clearScreen() {
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

void MediaListView::waitForInput() {
    std::cout << "\nPress Enter to continue...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

void MediaListView::displayMediaFiles(const std::vector<MediaFile>& files, int page) {
    clearScreen();
    
    std::cout << std::string(80, '=') << std::endl;
    std::cout << std::setw(40) << std::right << "Media Files" << std::endl;
    std::cout << std::string(80, '=') << std::endl;
    
    // Calculate pagination values
    int totalFiles = files.size();
    int totalPages = (totalFiles + Constants::ITEMS_PER_PAGE - 1) / Constants::ITEMS_PER_PAGE;
    int startIndex = page * Constants::ITEMS_PER_PAGE;
    int endIndex = std::min(startIndex + Constants::ITEMS_PER_PAGE, totalFiles);
    
    if (totalFiles == 0) {
        std::cout << "\nNo media files found in this location." << std::endl;
    } else {
        // File list header
        std::cout << std::left << std::setw(4) << "#" 
                  << std::setw(40) << "Name" 
                  << std::setw(15) << "Type" 
                  << std::setw(10) << "Duration" 
                  << std::endl;
        std::cout << std::string(80, '-') << std::endl;
        
        // List files for current page
        for (int i = startIndex; i < endIndex; ++i) {
            std::cout << formatMediaFileEntry(files[i], i) << std::endl;
        }
        
        std::cout << std::endl;
        displayPaginationControls(page, totalPages);
    }
    
    std::cout << "\nOptions:" << std::endl;
    std::cout << "  1. View file details" << std::endl;
    std::cout << "  2. Search files" << std::endl;
    std::cout << "  3. Filter by type" << std::endl;
    std::cout << "  4. Add to playlist" << std::endl;
    std::cout << "  5. Change page" << std::endl;
    std::cout << "  0. Back to main menu" << std::endl;
}

void MediaListView::displayMediaFileDetails(const MediaFile& file) {
    clearScreen();
    
    std::cout << std::string(80, '=') << std::endl;
    std::cout << std::setw(40) << std::right << "Media File Details" << std::endl;
    std::cout << std::string(80, '=') << std::endl;
    
    const Metadata& metadata = file.getMetadata();
    
    std::cout << "File: " << file.getFileName() << std::endl;
    std::cout << "Path: " << file.getFilePath() << std::endl;
    std::cout << "Type: " << (file.getType() == Constants::FileType::AUDIO ? "Audio" : "Video") << std::endl;
    std::cout << std::endl;
    
    std::cout << "Metadata:" << std::endl;
    std::cout << "  Title: " << metadata.getName() << std::endl;
    std::cout << "  Duration: " << metadata.getDurationString() << std::endl;
    
    // Display other metadata attributes
    const auto& attributes = metadata.getAllAttributes();
    if (!attributes.empty()) {
        for (const auto& attr : attributes) {
            std::cout << "  " << attr.first << ": " << attr.second << std::endl;
        }
    }
    
    std::cout << "\nOptions:" << std::endl;
    std::cout << "  1. Edit metadata" << std::endl;
    std::cout << "  2. Play this file" << std::endl;
    std::cout << "  3. Add to playlist" << std::endl;
    std::cout << "  0. Back to file list" << std::endl;
}

void MediaListView::displayMediaFileEditMenu(const MediaFile& file) {
    clearScreen();
    
    std::cout << std::string(80, '=') << std::endl;
    std::cout << std::setw(40) << std::right << "Edit Metadata" << std::endl;
    std::cout << std::string(80, '=') << std::endl;
    
    const Metadata& metadata = file.getMetadata();
    
    std::cout << "File: " << file.getFileName() << std::endl;
    std::cout << std::endl;
    
    std::cout << "Current Metadata:" << std::endl;
    std::cout << "  1. Title: " << metadata.getName() << std::endl;
    
    // Display editable metadata attributes
    int option = 2;
    const auto& attributes = metadata.getAllAttributes();
    for (const auto& attr : attributes) {
        std::cout << "  " << option << ". " << attr.first << ": " << attr.second << std::endl;
        option++;
    }
    
    std::cout << "  " << option << ". Add new attribute" << std::endl;
    std::cout << "  0. Save and return" << std::endl;
    
    std::cout << "\nSelect attribute to edit: ";
}

void MediaListView::displayScanningProgress(const std::string& directory, int filesFound) {
    clearScreen();
    std::cout << "Scanning directory: " << directory << std::endl;
    std::cout << "Files found: " << filesFound << std::endl;
    std::cout << "Please wait..." << std::endl;
}

void MediaListView::displayMediaOptionsMenu() {
    clearScreen();
    
    std::cout << std::string(80, '=') << std::endl;
    std::cout << std::setw(40) << std::right << "Media Options" << std::endl;
    std::cout << std::string(80, '=') << std::endl;
    
    std::cout << "1. Scan current directory" << std::endl;
    std::cout << "2. Scan recursively" << std::endl;
    std::cout << "3. Scan USB device" << std::endl;
    std::cout << "4. Show all media files" << std::endl;
    std::cout << "5. Show audio files only" << std::endl;
    std::cout << "6. Show video files only" << std::endl;
    std::cout << "7. Search by name" << std::endl;
    std::cout << "0. Back to main menu" << std::endl;
}

std::string MediaListView::formatMediaFileEntry(const MediaFile& file, int index) {
    std::stringstream ss;
    
    // Format the listing with fixed width columns
    ss << std::left << std::setw(4) << (index + 1) 
       << std::setw(40) << (file.getMetadata().getName().length() > 37 
                            ? file.getMetadata().getName().substr(0, 37) + "..." 
                            : file.getMetadata().getName())
       << std::setw(15) << (file.getType() == Constants::FileType::AUDIO ? "Audio" : "Video") 
       << std::setw(10) << file.getMetadata().getDurationString();
    
    return ss.str();
}

void MediaListView::displayPaginationControls(int currentPage, int totalPages) {
    std::cout << "Page " << (currentPage + 1) << " of " << totalPages << std::endl;
    
    if (currentPage > 0) {
        std::cout << "  [P] Previous page" << std::endl;
    }
    
    if (currentPage < totalPages - 1) {
        std::cout << "  [N] Next page" << std::endl;
    }
}

void MediaListView::displayHeader() {
    clearScreen();
    std::cout << std::string(80, '*') << std::endl;
    std::cout << std::setw(40) << std::right << "Media Browser Application" << std::endl;
    std::cout << std::string(80, '*') << std::endl;
    std::cout << std::endl;
}

void MediaListView::displayCurrentDirectory(const std::string& directory) {
    std::cout << "Current Directory: " << directory << std::endl;
    std::cout << std::string(80, '-') << std::endl;
}

void MediaListView::displayMainMenu() {
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

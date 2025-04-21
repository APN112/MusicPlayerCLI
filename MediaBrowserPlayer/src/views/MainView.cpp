#include "../../include/views/MainView.h"
#include <iostream>
#include <iomanip>
#include <limits>
#include <cstdlib>

MainView::MainView() {
}

void MainView::displayMessage(const std::string& message) {
    std::cout << message << std::endl;
}

void MainView::displayError(const std::string& message) {
    std::cerr << "ERROR: " << message << std::endl;
}

std::string MainView::getInput(const std::string& prompt) {
    std::string input;
    std::cout << prompt;
    std::getline(std::cin, input);
    return input;
}

std::string MainView::getInitialDirectory() {
    std::cout << "Please enter the directory to scan for media files: ";
    std::string dir;
    std::getline(std::cin, dir);
    return dir;
}

int MainView::getMenuChoice(int min, int max) {
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

void MainView::clearScreen() {
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

void MainView::waitForInput() {
    std::cout << "\nPress Enter to continue...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

void MainView::displayMainMenu() {
    displayHeader();
    
    std::cout << "\n";
    displayMenuItem(1, "Show Media Library");
    displayMenuItem(2, "Show List of Playlists");
    displayMenuItem(3, "Now Playing");
    displayMenuItem(4, "Play");
    displayMenuItem(5, "Change Directory");
    displayMenuItem(6, "Mount USB Device");
    displayMenuItem(0, "Exit");
    std::cout << "\n";
}

void MainView::displayCurrentDirectory(const std::string& directory) {
    std::cout << "Current Directory: " << directory << std::endl;
}

void MainView::displayHeader() {
    std::cout << std::string(80, '=') << std::endl;
    std::cout << std::setw(40) << std::right << "Media Player CLI" << std::endl;
    std::cout << std::string(80, '=') << std::endl;
}

void MainView::displayMenuItem(int number, const std::string& text) {
    std::cout << "  " << number << ". " << text << std::endl;
}

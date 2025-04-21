#ifndef PLAYERVIEW_H
#define PLAYERVIEW_H

#include "IView.h"
#include "../models/AudioState.h"
#include "../models/MediaFile.h"
#include "../models/Playlist.h"

class PlayerView : public IView {
public:
    PlayerView();
    
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

    
    // Display player interface with current track info and controls
    void displayPlayer(const AudioState& audioState);
    
    // Display player controls
    void displayPlayerControls();
    
    // Display progress bar
    void displayProgressBar(double current, double total);
    
    // Display volume level
    void displayVolume(int volume);
    
    // Display currently playing track info
    void displayNowPlaying(const MediaFile& track);
    
    // Display queue/playlist information
    void displayQueue(const Playlist& playlist, int currentIndex);
    
    // Flash message (e.g., "Paused", "Next Track")
    void flashMessage(const std::string& message);
    
private:
    // Format time (seconds to MM:SS)
    std::string formatTime(double seconds);
    
    // Helper to draw ASCII progress bar
    std::string drawProgressBar(double percentage, int width);
    
    // Helper to draw ASCII volume bar
    std::string drawVolumeBar(int volume, int width);
};

#endif // PLAYERVIEW_H

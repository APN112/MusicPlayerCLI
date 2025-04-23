#ifndef AUDIOSERVICE_H
#define AUDIOSERVICE_H

#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include "../models/MediaFile.h"

class AudioService {
public:
    AudioService();
    ~AudioService();
    
    // Initialize SDL audio system
    bool initialize();
    
    // Clean up resources
    void cleanup();
    
    // Load and play an audio file
    bool loadAndPlay(const std::string& filePath);
    
    // Pause playback
    void pause();
    
    // Resume playback
    void resume();
    
    // Stop playback
    void stop();
    
    // Set volume (0-100)
    void setVolume(int volume);
    
    // Get current position (in seconds)
    double getCurrentPosition() const;
    
    // Is audio currently playing?
    bool isPlaying() const;
    
    // Is audio paused?
    bool isPaused() const;
    
    // Get the duration of the current audio file (in seconds)
    double getDuration() const;
    
    // // Set playback position (in seconds)
    // void setPosition(double position);
    
private:
    Mix_Music* music;
    double duration;
    Uint32 startTime;
    Uint32 pauseTime;
    bool paused;
    
    // Calculate current position based on SDL ticks
    double calculatePosition() const;
    
    // Reset timer when starting playback
    void resetTimer();
    
    // Convert SDL mixer volume (0-128) to app volume (0-100)
    int convertToSDLVolume(int appVolume) const;
};

#endif // AUDIOSERVICE_H

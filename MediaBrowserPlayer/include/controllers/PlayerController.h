#ifndef PLAYERCONTROLLER_H
#define PLAYERCONTROLLER_H

#include <memory>
#include <thread>
#include <atomic>
#include <condition_variable>
#include <mutex>
#include "../models/AudioState.h"
#include "../views/PlayerView.h"
#include "../services/AudioService.h"
#include "../models/Playlist.h"
#include "../models/MediaLibrary.h"

class PlayerController {
public:
    PlayerController(std::shared_ptr<IView> parentView);
    ~PlayerController();
    
    // Initialize the player
    bool initialize();
    
    // Clean up resources
    void cleanup();
    
    // Play a specific track from a playlist
    bool playPlaylist();
    
    // Play/pause toggle
    void togglePlayPause();
    
    // Stop playback
    void stop();
    
    // Skip to next track
    void next();
    
    // Skip to previous track
    void previous();
    
    // Adjust volume
    void setVolume(int volume);
    void increaseVolume();
    void decreaseVolume();
    
    // Show player interface
    void showPlayer();
    
    // Update player view (called from playback thread)
    void updatePlayerView();
    
    // Get audio state
    AudioState& getAudioState();
    
    // Check if player is active
    bool isPlaying() const;

    // Check if player view is active
    bool _isDisplaying() const;
    
private:
    // Model
    AudioState audioState;

    // View
    std::shared_ptr<PlayerView> playerView;
    
    // Controller
    AudioService audioService;
    
    // Music thread and its flags
    SDL_Thread* musicThread;
    std::atomic<bool> stopMusicThread = false;
    std::atomic<bool> playMusic = false;
    std::atomic<bool> stopMusic = false;
    std::atomic<bool> toggleState = false;
    std::atomic<bool> nextMusic = false;
    std::atomic<bool> preMusic = false;
    std::atomic<bool> loadNewSource = false;
    std::mutex audioStateMutex;

    // Display thread
    SDL_Thread* updateViewThread;
    std::atomic<bool> isRunning = true;
    std::atomic<bool> isDisplaying = false;
    // Create SDL mutex for thread synchronization
    SDL_mutex* mutex;
    SDL_cond* condition;

    // Function to play music in music thread
    static int musicThreadFunc(void* data);

    // Function to update view in updateView thread
    static int updateViewThreadFunc(void *data);
};

#endif // PLAYERCONTROLLER_H

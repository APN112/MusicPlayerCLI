#include "../../include/controllers/PlayerController.h"
#include <iostream>
#include <chrono>
#include <algorithm>
#include <cmath>

PlayerController::PlayerController(std::shared_ptr<IView> parentView)
    : playerView(std::make_shared<PlayerView>()) {
    // Convert parent view to PlayerView if possible, otherwise create a new one
    if (parentView) {
        playerView = std::dynamic_pointer_cast<PlayerView>(parentView);
        if (!playerView) {
            playerView = std::make_shared<PlayerView>();
        }
    }
    mutex = SDL_CreateMutex();
    condition = SDL_CreateCond();
}

PlayerController::~PlayerController() {
    cleanup();
}

bool PlayerController::initialize() {
    // Initialize the audio service
    if (!audioService.initialize()) { // Init SDL
        playerView->displayError("Failed to initialize audio system");
        return false;
    }
    
    // Set initial volume
    audioService.setVolume(audioState.getVolume());

    // Create music Thread
    musicThread = SDL_CreateThread(musicThreadFunc, "MusicThread", this);

    // Create display Thread
    updateViewThread = SDL_CreateThread(updateViewThreadFunc, "ViewThread", this);
    
    return true;
}

int PlayerController::updateViewThreadFunc(void* data){
    PlayerController* self = static_cast<PlayerController*>(data);

    while(self->isRunning){
        SDL_LockMutex(self->mutex);

        // Wait for the signal if not displaying but still running
        while(self->isRunning && !self->isDisplaying) {
            SDL_CondWait(self->condition, self->mutex);
        }
        
        // Break if app is about to end
        if(!self->isRunning) {
            SDL_UnlockMutex(self->mutex);
            break;
        }
        
        // Update view if both flags are set
        if(self->isRunning && self->isDisplaying) {
            // Temporarily unlock while updating
            SDL_UnlockMutex(self->mutex);
            
            self->updatePlayerView();
            SDL_Delay(500); // 500ms delay between updates
            
            SDL_LockMutex(self->mutex);
        }
        
        SDL_UnlockMutex(self->mutex);
    }
    return 0;
}

int PlayerController::musicThreadFunc(void *data){
    PlayerController* self = static_cast<PlayerController*>(data);
    
    while(!self->stopMusicThread){
        // Reload playlist if a new "Play" command is executed
        if(self->loadNewSource && self->playMusic){
            self->audioService.loadAndPlay(self->audioState.getCurrentTrack().getFilePath());
            self->loadNewSource = false;
        }

        // Load next music in the playlist when the current one is done
        if(!Mix_PlayingMusic() && self->playMusic){
            if(self->audioState.nextTrack()){
                self->audioService.loadAndPlay(self->audioState.getCurrentTrack().getFilePath());
            }

            // Update View
            self->updatePlayerView();
        }

        // Toggle play/pause when toggleState flag triggered
        if(self->toggleState){
            if (self->audioState.getPlayerState() == Constants::PlayerState::PLAYING) {
                self->audioService.pause();
                self->audioState.setPlayerState(Constants::PlayerState::PAUSED);
                self->playerView->flashMessage("Paused");
                self->playMusic = false;
            } else if (self->audioState.getPlayerState() == Constants::PlayerState::PAUSED) {
                self->audioService.resume();
                self->audioState.setPlayerState(Constants::PlayerState::PLAYING);
                self->playerView->flashMessage("Playing");
                self->playMusic = true;
            } else {
                if(self->audioState.hasValidTrack()){
                    self->audioService.loadAndPlay(self->audioState.getCurrentTrack().getFilePath());
                    self->audioState.setPlayerState(Constants::PlayerState::PLAYING);
                    self->playerView->flashMessage("Playing");
                    self->playMusic = true;
                }
            }

            // Update View
            self->updatePlayerView();

            // Reset flag when done
            self->toggleState = false;
        }

        // Next track when nextMusic flag triggered
        if(self->nextMusic){
            if (self->audioState.nextTrack()) {
                const MediaFile& track = self->audioState.getCurrentTrack();
                
                if (self->audioService.loadAndPlay(track.getFilePath())) {
                    self->audioState.setPlayerState(Constants::PlayerState::PLAYING);
                    self->audioState.setCurrentPosition(0.0);
                    self->playerView->flashMessage("Next track");
                    self->playMusic =true;
                    self->stopMusic = false;
                } else {
                    self->playerView->displayError("Failed to play next track");
                    self->audioState.setPlayerState(Constants::PlayerState::STOPPED);
                    self->playMusic = false;
                    self->stopMusic = true;
                }
            } else {
                self->playerView->displayError("No next track available");
            }

            // Update View
            self->updatePlayerView();

            // Reset flag when done
            self->nextMusic = false;
        }

        // Previous track when preMusic flag triggered
        if(self->preMusic){
            if (self->audioState.previousTrack()) {
                const MediaFile& track = self->audioState.getCurrentTrack();
                
                if (self->audioService.loadAndPlay(track.getFilePath())) {
                    self->audioState.setPlayerState(Constants::PlayerState::PLAYING);
                    self->audioState.setCurrentPosition(0.0);
                    self->playerView->flashMessage("Previous track");
                    self->playMusic = true;
                    self->stopMusic = false;
                } else {
                    self->playerView->displayError("Failed to play previous track");
                    self->audioState.setPlayerState(Constants::PlayerState::STOPPED);
                    self->playMusic = false;
                    self->stopMusic = true;
                }
            } else {
                self->playerView->displayError("No previous track available");
                self->playMusic = false;
                self->stopMusic = true;
            }

            self->updatePlayerView();

            // Reset flag when done
            self->preMusic = false;
        }

        // Stop music when flag playMusic reset
        if(self->stopMusic){
            // Reset play flag
            self->playMusic = false;

            // Stop playing music
            self->audioService.stop();

            self->playerView->flashMessage("Stopped");

            // Update the current Track to the next Track
            if(!self->audioState.nextTrack()){
                self->playerView->displayError("There's no media in the current playlist");
            }

            // Update player state to stopped
            self->audioState.setPlayerState(Constants::PlayerState::STOPPED);

            // Reset current position
            self->audioState.setCurrentPosition(0.0);

            // Update View
            self->updatePlayerView();

            // Reset stop flag when done
            self->stopMusic = false;
        }

        // Execute while loop once per 500ms
        SDL_Delay(500);
    }
    return 0;
}

void PlayerController::cleanup() {

    // Stop display thread
    SDL_LockMutex(mutex);
    isRunning = false;
    isDisplaying = false;
    SDL_CondSignal(condition); // Signal thread to check condition
    SDL_UnlockMutex(mutex);

    if(updateViewThread != nullptr) {
        SDL_WaitThread(updateViewThread, nullptr); // Wait for thread to end
        updateViewThread = nullptr;
    }

    // Stop music thread
    stopMusicThread = true;

    if (musicThread != nullptr) {
        SDL_WaitThread(musicThread, nullptr);  // Wait for thread to end
        musicThread = nullptr;  // reset pointer
    }
    
    // Stop playing music and clean up
    stop();
    audioService.cleanup();

    // Clean up synchronization objects
    if(condition != nullptr) {
        SDL_DestroyCond(condition);
        condition = nullptr;
    }

    if(mutex != nullptr) {
        SDL_DestroyMutex(mutex);
        mutex = nullptr;
    }
}

bool PlayerController::playPlaylist() {
    std::lock_guard<std::mutex> lock(audioStateMutex);

    if(audioState.getCurrentPlaylist().isEmpty()) {
        playerView->displayError("There's no media file to play!");
        return false;
    }

    // Stop any current playback
    audioService.stop();

    // Set playMusic flag to true
    playMusic = true;

    // Set loadMusicSource to true (everytime user enter "Play" command need to check source again)
    loadNewSource = true;

    // Reset stop flag
    stopMusic = false;

    // Set playerState to Playing
    audioState.setPlayerState(Constants::PlayerState::PLAYING);

    return true;
}

void PlayerController::togglePlayPause() {
    std::lock_guard<std::mutex> lock(audioStateMutex);
    toggleState = true;
}

void PlayerController::stop() {
    std::lock_guard<std::mutex> lock(audioStateMutex);
    playMusic = false;
    stopMusic = true;
}

void PlayerController::next() {
    std::lock_guard<std::mutex> lock(audioStateMutex);
    nextMusic = true;
}

void PlayerController::previous() {
    std::lock_guard<std::mutex> lock(audioStateMutex);
    preMusic = true;
}

void PlayerController::setVolume(int volume) {
    std::lock_guard<std::mutex> lock(audioStateMutex);
    
    // Ensure volume is within range (0-100)
    volume = std::max(0, std::min(volume, 100));
    
    audioState.setVolume(volume);
    audioService.setVolume(volume);
    
    playerView->flashMessage("Volume: " + std::to_string(volume));
}

void PlayerController::increaseVolume() {
    setVolume(audioState.getVolume() + 5);
}

void PlayerController::decreaseVolume() {
    setVolume(audioState.getVolume() - 5);
}

void PlayerController::showPlayer() {
    // Start View Thread
    SDL_LockMutex(mutex);
    isDisplaying = true;
    SDL_CondSignal(condition);
    SDL_UnlockMutex(mutex);
    
    bool continueRunning = true;
    while (continueRunning) {
        // Get user input for controls
        std::string input = playerView->getInput("");
        
        if (input.empty()) {
            // Update player view again
            updatePlayerView();
            continue;
        }
        
        char key = std::toupper(input[0]);
        
        switch (key) {
            case ' ': // Space - Play/Pause
                togglePlayPause();
                break;
                
            case 'S': // Stop
                stop();
                break;
                
            case 'N': // Next track
                next();
                break;
                
            case 'P': // Previous track
                previous();
                break;
                
            case '+': // Volume up
                increaseVolume();
                break;
                
            case '-': // Volume down
                decreaseVolume();
                break;
                
            case 'Q': // Quit player view
                continueRunning = false;
                SDL_LockMutex(mutex);
                isDisplaying = false;
                SDL_UnlockMutex(mutex);
                break;
                
            default:
                // Ignore other keys
                break;
        }
    }
}

void PlayerController::updatePlayerView() {
    std::lock_guard<std::mutex> lock(audioStateMutex);
    audioState.setCurrentPosition(audioService.getCurrentPosition());
    playerView->displayPlayer(audioState);
}

AudioState& PlayerController::getAudioState() {
    return audioState;
}

bool PlayerController::isPlaying() const {
    return audioState.getPlayerState() == Constants::PlayerState::PLAYING;
}

bool PlayerController::_isDisplaying() const {
    return isDisplaying;
}
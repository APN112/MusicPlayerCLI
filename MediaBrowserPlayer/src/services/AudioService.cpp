#include "../../include/services/AudioService.h"
#include <taglib/fileref.h>
#include <taglib/audioproperties.h>
#include <stdexcept>
#include <iostream>

AudioService::AudioService() : music(nullptr), duration(0.0), startTime(0), pauseTime(0), paused(false) {
}

AudioService::~AudioService() {
    cleanup();
}

bool AudioService::initialize() {
    // Initialize SDL audio system
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        std::cerr << "SDL could not initialize! SDL Error: " << SDL_GetError() << std::endl;
        return false;
    }

    if(Mix_Init(MIX_INIT_MP3) == 0) {
        std::cerr << "SDL_mixer could not initialize! SDL_mixer Error: " << Mix_GetError() << std::endl;
        SDL_Quit();
        return false;
    }
    
    // Initialize SDL_mixer
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 8192) < 0) {
        std::cerr << "SDL_mixer could not initialize! SDL_mixer Error: " << Mix_GetError() << std::endl;
        Mix_Quit();
        SDL_Quit();
        return false;
    }
    
    return true;
}

void AudioService::cleanup() {
    stop();
    
    // Quit SDL_mixer and SDL
    Mix_CloseAudio();
    SDL_Quit();
}

bool AudioService::loadAndPlay(const std::string& filePath) {
    // Stop any currently playing music
    stop();

    music = Mix_LoadMUS(filePath.c_str());
    if (!music) {
        std::cerr << "Failed to load music! SDL_mixer Error: " << Mix_GetError() << std::endl;
        return false;
    }
    
    // Play music
    if (Mix_PlayMusic(music, 0) == -1) {
        std::cerr << "Failed to play music! SDL_mixer Error: " << Mix_GetError() << std::endl;
        Mix_FreeMusic(music);
        music = nullptr;
        return false;
    }

    // Update Duration
    TagLib::FileRef f(filePath.c_str());

    if (!f.isNull() && f.audioProperties()) {
        TagLib::AudioProperties* properties = f.audioProperties();
        duration = properties->length(); // duration in second
    }
    
    // Reset timer
    resetTimer();
    paused = false;
    
    return true;
}

void AudioService::pause() {
    if (music && !paused) {
        Mix_PauseMusic();
        pauseTime = SDL_GetTicks();
        paused = true;
    }
}

void AudioService::resume() {
    if (music && paused) {
        Mix_ResumeMusic();
        startTime += (SDL_GetTicks() - pauseTime);
        paused = false;
    }
}

void AudioService::stop() {
    if (music) {
        Mix_HaltMusic();
        Mix_FreeMusic(music);
        music = nullptr;
        duration = 0.0;
        startTime = 0;
        pauseTime = 0;
        paused = false;
    }
}

void AudioService::setVolume(int volume) {
    Mix_VolumeMusic(convertToSDLVolume(volume));
}

double AudioService::getCurrentPosition() const {
    return calculatePosition();
}

bool AudioService::isPlaying() const {
    return music != nullptr && Mix_PlayingMusic() == 1 && !paused;
}

bool AudioService::isPaused() const {
    return music != nullptr && paused;
}

double AudioService::getDuration() const {
    return duration;
}

// void AudioService::setPosition(double position) {
//     // SDL_mixer doesn't support seeking directly
//     // This is a placeholder for future implementation
//     // You could potentially restart the track and use Mix_SetMusicPosition()
//     // for formats that support it
// }

double AudioService::calculatePosition() const {
    if (!music) {
        return 0.0;
    }
    
    if (paused) {
        return (pauseTime - startTime) / 1000.0;
    } else {
        return (SDL_GetTicks() - startTime) / 1000.0;
    }
}

void AudioService::resetTimer() {
    startTime = SDL_GetTicks();
    pauseTime = 0;
}

int AudioService::convertToSDLVolume(int appVolume) const {
    // Convert from 0-100 to 0-128 (SDL_mixer volume range)
    return (appVolume * MIX_MAX_VOLUME) / 100;
}

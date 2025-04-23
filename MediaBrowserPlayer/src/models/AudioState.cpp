#include "../../include/models/AudioState.h"
#include <stdexcept>

AudioState::AudioState() 
    : currentTrackIndex(-1), currentPosition(0.0), volume(80), 
      playerState(Constants::PlayerState::STOPPED) {
}

int AudioState::getCurrentTrackIndex() const {
    return currentTrackIndex;
}

void AudioState::setCurrentTrackIndex(int index) {
    currentTrackIndex = index;
}

double AudioState::getCurrentPosition() const {
    return currentPosition;
}

void AudioState::setCurrentPosition(double position) {
    currentPosition = position;
}

const Playlist& AudioState::getCurrentPlaylist() const {
    return currentPlaylist;
}

void AudioState::setCurrentPlaylist(const Playlist& playlist) {
    currentPlaylist = playlist;
}

int AudioState::getVolume() const {
    return volume;
}

void AudioState::setVolume(int vol) {
    // Ensure volume is within range
    volume = (vol < 0) ? 0 : (vol > 100) ? 100 : vol;
}

Constants::PlayerState AudioState::getPlayerState() const {
    return playerState;
}

void AudioState::setPlayerState(Constants::PlayerState state) {
    playerState = state;
}

const MediaFile& AudioState::getCurrentTrack() const {
    if (currentTrackIndex >= 0 && currentTrackIndex < static_cast<int>(currentPlaylist.getTrackCount())) {
        // Create a member variable to store the track
        // Use the mutable member variable instead of a static local
        cachedCurrentTrack = currentPlaylist.getTrack(currentTrackIndex);
        return cachedCurrentTrack;
    }
    throw std::out_of_range("No current track selected");
}

bool AudioState::hasValidTrack() const {
    return currentTrackIndex >= 0 && 
           currentTrackIndex < static_cast<int>(currentPlaylist.getTrackCount());
}

bool AudioState::nextTrack() {
    if (currentPlaylist.getTrackCount() == 0) {
        return false;
    }
    
    currentTrackIndex++;
    if (currentTrackIndex >= static_cast<int>(currentPlaylist.getTrackCount())) {
        currentTrackIndex = 0; // Loop to beginning
    }
    
    currentPosition = 0.0;
    return true;
}

bool AudioState::previousTrack() {
    if (currentPlaylist.getTrackCount() == 0) {
        return false;
    }
    
    currentTrackIndex--;
    if (currentTrackIndex < 0) {
        currentTrackIndex = static_cast<int>(currentPlaylist.getTrackCount()) - 1; // Loop to end
    }
    
    currentPosition = 0.0;
    return true;
}

bool AudioState::isAtEnd() const {
    return currentTrackIndex >= static_cast<int>(currentPlaylist.getTrackCount()) - 1;
}

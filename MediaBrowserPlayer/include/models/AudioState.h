#ifndef AUDIOSTATE_H
#define AUDIOSTATE_H

#include <string>
#include "../Constants.h"
#include "Playlist.h"

class AudioState {
public:
    AudioState();
    
    // Get/set current track index
    int getCurrentTrackIndex() const;
    void setCurrentTrackIndex(int index);
    
    // Get/set current position in seconds
    double getCurrentPosition() const;
    void setCurrentPosition(double position);
    
    // Get/set current playlist
    const Playlist& getCurrentPlaylist() const;
    void setCurrentPlaylist(const Playlist& playlist);
    
    // Get/set volume (0-100)
    int getVolume() const;
    void setVolume(int volume);
    
    // Get/set player state
    Constants::PlayerState getPlayerState() const;
    void setPlayerState(Constants::PlayerState state);
    
    // Get currently playing media file
    const MediaFile& getCurrentTrack() const;
    
    // Cached current track for reference return
    mutable MediaFile cachedCurrentTrack;
    
    // Check if we have a valid track to play
    bool hasValidTrack() const;
    
    // Move to next/previous track
    bool nextTrack();
    bool previousTrack();
    
    // Is the playback at the end of the playlist?
    bool isAtEnd() const;
    
private:
    int currentTrackIndex;
    double currentPosition; // in seconds
    Playlist currentPlaylist;
    int volume; // 0-100
    Constants::PlayerState playerState;
};

#endif // AUDIOSTATE_H

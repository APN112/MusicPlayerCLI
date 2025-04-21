#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <string>

namespace Constants {
    // UI constants
    constexpr int ITEMS_PER_PAGE = 25;
    constexpr int MAX_DISPLAY_WIDTH = 80;
    constexpr int SCREEN_WIDTH = 800;
    constexpr int SCREEN_HEIGHT = 600;
    
    // File types
    enum class FileType {
        AUDIO,
        VIDEO,
        UNKNOWN
    };
    
    // Supported extensions
    const std::string AUDIO_EXTENSIONS[] = {".mp3", ".wav", ".ogg", ".flac", ".aac"};
    const std::string VIDEO_EXTENSIONS[] = {".mp4", ".avi", ".mkv", ".mov"};
    
    // S32K144 Board settings
    constexpr int S32K144_BAUDRATE = 9600;
    constexpr char S32K144_PORT[] = "/dev/ttyACM0";
    
    // Playlist settings
    constexpr char PLAYLISTS_DIR[] = "/home/namanh/code/MediaBrowserPlayer/playlists/";
    constexpr char PLAYLIST_EXT[] = ".playlist";
    
    // Metadata keys
    namespace MetadataKeys {
        constexpr char TITLE[] = "title";
        constexpr char ARTIST[] = "artist";
        constexpr char ALBUM[] = "album";
        constexpr char YEAR[] = "year";
        constexpr char GENRE[] = "genre";
        constexpr char DURATION[] = "duration";
        constexpr char BITRATE[] = "bitrate";
        constexpr char CODEC[] = "codec";
        constexpr char PUBLISHER[] = "publisher";
        constexpr char TRACK_NUMBER[] = "track_number";
    }
    
    // Player states
    enum class PlayerState {
        PLAYING,
        PAUSED,
        STOPPED
    };
}

#endif // CONSTANTS_H

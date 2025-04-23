#ifndef MEDIALISTVIEW_H
#define MEDIALISTVIEW_H

#include "IView.h"
#include "../models/MediaFile.h"
#include <vector>

class MediaListView : public IView {
public:
    MediaListView();
    
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
    
    // Display a list of media files with pagination
    void displayMediaFiles(const std::vector<MediaFile>& files, int page);
    
    // Display detailed metadata for a media file
    void displayMediaFileDetails(const MediaFile& file);
    
    // Display media file edit menu
    void displayMediaFileEditMenu(const MediaFile& file);
    
    // Display scanning progress
    void displayScanningProgress(const std::string& directory, int filesFound);
    
    // Display media options menu
    void displayMediaOptionsMenu();
    
private:
    // Format a media file entry for display
    std::string formatMediaFileEntry(const MediaFile& file, int index);
    
    // Helper method for pagination controls
    void displayPaginationControls(int currentPage, int totalPages);
};

#endif // MEDIALISTVIEW_H

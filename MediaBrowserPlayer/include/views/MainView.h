#ifndef MAINVIEW_H
#define MAINVIEW_H

#include "IView.h"
#include <vector>
#include <string>

class MainView : public IView {
public:
    MainView();
    
    // IView implementation
    void displayMessage(const std::string& message) override;
    void displayError(const std::string& message) override;
    std::string getInput(const std::string& prompt) override;
    std::string getInitialDirectory() override;
    int getMenuChoice(int min, int max) override;
    void clearScreen() override;
    void waitForInput() override;
    void displayHeader() override;
    void displayCurrentDirectory(const std::string& directory) override;
    void displayMainMenu() override;
    
private:
    // Helper for drawing menu items
    void displayMenuItem(int number, const std::string& text);
};

#endif // MAINVIEW_H

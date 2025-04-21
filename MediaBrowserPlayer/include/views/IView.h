#ifndef IVIEW_H
#define IVIEW_H

#include <string>

class IView {
public:
    virtual ~IView() = default;
    
    // Display a message to the user
    virtual void displayMessage(const std::string& message) = 0;
    
    // Display an error message to the user
    virtual void displayError(const std::string& message) = 0;
    
    // Get input from the user
    virtual std::string getInput(const std::string& prompt) = 0;

    // // Get initial Directory
    virtual std::string getInitialDirectory(){ return "."; };
    
    // Get a menu choice from the user
    virtual int getMenuChoice(int min, int max) = 0;
    
    // Clear the screen
    virtual void clearScreen() = 0;
    
    // Wait for user input to continue
    virtual void waitForInput() = 0;
    
    // Display header
    virtual void displayHeader() = 0;
    
    // Display current directory
    virtual void displayCurrentDirectory(const std::string& directory) = 0;
    
    // Display main menu
    virtual void displayMainMenu() = 0;
};

#endif // IVIEW_H

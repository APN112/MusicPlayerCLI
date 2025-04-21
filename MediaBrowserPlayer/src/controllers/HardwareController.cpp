#include "../../include/controllers/HardwareController.h"
#include "../../include/controllers/PlayerController.h"
#include <iostream>
#include <chrono>
#include <thread>
#include <cmath>

HardwareController::HardwareController(std::shared_ptr<PlayerController> playerCtrl)
    : boardHandle(-1), playerController(playerCtrl), stopInputThread(false) {
}

HardwareController::~HardwareController() {
    cleanup();
}

bool HardwareController::initialize() {
    // Try to connect to the S32K144 board
    if (!s32k144.connect()) {
        std::cerr << "Failed to connect to S32K144 board" << std::endl;
        return false;
    }
    
    // Initialize board handle
    boardHandle = 1; // Simplified handle
    
    // Start thread to read input from hardware
    stopInputThread = false;
    inputThread = std::thread(&HardwareController::readHardwareInput, this);
    
    return true;
}

void HardwareController::cleanup() {
    // Stop input thread
    if (inputThread.joinable()) {
        stopInputThread = true;
        inputThread.join();
    }
    
    // Disconnect from the board
    s32k144.disconnect();
    boardHandle = -1;
}

bool HardwareController::isConnected() const {
    return boardHandle != -1 && s32k144.isConnected();
}

void HardwareController::readHardwareInput() {
    while (!stopInputThread && isConnected()) {
        // Read data from the board
        std::string data = s32k144.readData();
        
        if (!data.empty() && playerController->_isDisplaying()) {
            if (data == "P") {
                playerController->previous();
            }
            else if (data == "N") {
                playerController->next();
            }
            else if (data == "T") {
                playerController->togglePlayPause();
            }
            else if (data == "S") {
                playerController->stop();
            }
            else if(std::all_of(data.begin(), data.end(), ::isdigit)){
                playerController->setVolume(std::stoi(data));
            }
        }
        
        // // Sleep to reduce CPU usage
        // std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}
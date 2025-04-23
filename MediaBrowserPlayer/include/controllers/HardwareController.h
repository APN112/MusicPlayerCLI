#ifndef HARDWARECONTROLLER_H
#define HARDWARECONTROLLER_H

#include <memory>
#include <thread>
#include <atomic>
#include <functional>
#include "../utils/S32K144Communication.h"

// Forward declaration
class PlayerController;

class HardwareController {
public:
    HardwareController(std::shared_ptr<PlayerController> playerController);
    ~HardwareController();
    
    // Initialize the hardware interface
    bool initialize();
    
    // Close the hardware interface
    void cleanup();
    
    // Check if hardware is connected
    bool isConnected() const;
   
private:
    int boardHandle;
    S32K144Communication s32k144;
    std::shared_ptr<PlayerController> playerController;
    
    std::thread inputThread;
    std::atomic<bool> stopInputThread;
    
    // Thread function to read input from hardware
    void readHardwareInput();
};

#endif // HARDWARECONTROLLER_H

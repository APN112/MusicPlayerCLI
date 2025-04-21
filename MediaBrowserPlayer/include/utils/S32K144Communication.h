#ifndef S32K144COMMUNICATION_H
#define S32K144COMMUNICATION_H

#include <string>
#include <vector>
#include <thread>

class S32K144Communication {
public:
    S32K144Communication();
    ~S32K144Communication();
    
    // Open serial connection to S32K144 board
    bool connect();

    // Initialize UART
    bool initializeUART();

    // Check if S32K144 is connected
    bool isConnected() const;
    
    // Close serial connection
    void disconnect();

    // Read incoming data from S32K144
    std::string readData();
    
private:
    int serialPort;
    bool connected;
    std::string buffer;
};

#endif // S32K144COMMUNICATION_H

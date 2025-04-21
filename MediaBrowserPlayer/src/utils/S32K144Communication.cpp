#include "../../include/utils/S32K144Communication.h"
#include "../../include/Constants.h"
#include <thread>
#include <chrono>
#include <sstream>
#include <algorithm>
#include <cstring>
#include <iostream>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

S32K144Communication::S32K144Communication() : serialPort(-1), connected(false){
}

S32K144Communication::~S32K144Communication() {
    disconnect();
}

bool S32K144Communication::connect() {
    if (connected) {
        return true;
    }
    
    // Open serial port
    serialPort = open("/dev/ttyACM0", O_RDWR | O_NOCTTY | O_NDELAY);
    if (serialPort < 0) {
        std::cerr << "Can't open UART port" << std::endl;
        return 1;
    }

    connected = true;

    // Initialize UART
    initializeUART();
    
    return connected;
}

void S32K144Communication::disconnect() {
    if (!connected) {
        return;
    }
    
    std::cout << "Disconnecting from S32K144..." << std::endl;
    
    // Set flag to stop the read thread
    connected = false;
    
    // In a real application, close the serial port here
    close(serialPort);
}

bool S32K144Communication::initializeUART() {
    termios tty;
    memset(&tty, 0, sizeof(tty));
    if (tcgetattr(serialPort, &tty) != 0) {
        std::cerr << "Error in getting port attributes" << std::endl;
        return false;
    }
    
    cfsetospeed(&tty, B9600); // baudrate
    cfsetispeed(&tty, B9600);
    
    tty.c_cflag |= (CLOCAL | CREAD);    // Enable read
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;                 // 8 bit data
    tty.c_cflag &= ~PARENB;             // No parity
    tty.c_cflag &= ~CSTOPB;             // 1 stop bit
    tty.c_cflag &= ~CRTSCTS;            // No thread Control
    
    tty.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); // Turn off line mode
    tty.c_iflag &= ~(IXON | IXOFF | IXANY);         // Don't use XON/XOFF
    tty.c_oflag &= ~OPOST;                          // Turn off output handler
    
    tcsetattr(serialPort, TCSANOW, &tty);
    return true;
}

bool S32K144Communication::isConnected() const{
    return connected;
}

std::string S32K144Communication::readData() {
    if (!connected) {
        return "";
    }

    char ch;

    while (read(serialPort, &ch, 1) == 1){
        if(ch == '.') {
            std::string result = buffer;
            buffer.clear();
            return result;
        }
        buffer += ch;
    }
    return "";
}
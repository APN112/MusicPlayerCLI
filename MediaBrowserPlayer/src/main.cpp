#include "../include/controllers/ApplicationController.h"
#include <iostream>
#include <filesystem>
#include <cstdlib>
#include <cstring>
#include <exception>
#include <csignal>

// Forward declaration of signal handler
void signalHandler(int signal);

int main() {
    ApplicationController app;
    
    // Run the application
    app.run();
    return 0;
}

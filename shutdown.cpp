#include <iostream>
#include <chrono>
#include <csignal>
#include <fstream>
#include <thread>

void signal_handler(int signum) {
    std::ofstream file("./shutdown.txt", std::ios::binary);  // Use binary mode if writing raw bytes
    std::string message = "hello there, how are you? this is a shutdown program";

    if (file.is_open()) {
        file.write(message.c_str(), message.size());  // Correctly using write
        file.close();
    }
}

int main() {
    signal(SIGTERM, signal_handler);

    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    return 0;
}

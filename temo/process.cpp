#include <iostream>
#include <fstream>
#include <string>
#include <csignal>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <vector>

const std::string PID_FILE = "/tmp/testprocess.pid";

void handle_sigterm(int signum) {
    std::cout << "Received SIGTERM, terminating process." << std::endl;
    std::ofstream file("temp.txt",std::ios::binary);
    file << "this is a file";
    file.close();
    exit(0);
}

void loop() {
    while (1) {
        std::cout << "Running..." << std::endl;
        sleep(4);
    }
}

int check() {
    std::ifstream pid_file(PID_FILE);
    if (pid_file.is_open()) {
        pid_t pid;
        pid_file >> pid;
        pid_file.close();
        // Check if the process is still running
        if (kill(pid, 0) == 0) return 1;
    }
    return 0;
}

void start() {
    signal(SIGTERM, handle_sigterm);

    if (check()) {
        std::cout << "Process is already running." << std::endl;
        exit(0);
    }

    pid_t pid = fork();

    if (pid > 0) {
        // Parent process
        std::ofstream pid_file(PID_FILE);
        pid_file << pid;
        pid_file.close();
        std::cout << "Started process with PID: " << pid << std::endl;
    } else if (pid == 0) {
        // Child process
        loop();
        std::cout << "Child process terminated." << std::endl;
        exit(0);
    } else {
        std::cerr << "Failed to fork process." << std::endl;
        exit(1);
    }
}

void stop() {
    if (check()) {
        std::ifstream pid_file(PID_FILE);
        pid_t pid;
        pid_file >> pid;
        pid_file.close();
        kill(pid, SIGTERM);
        std::remove(PID_FILE.c_str());
        std::cout << "Stopped process with PID: " << pid << std::endl;
    } else {
        std::cout << "No process is running." << std::endl;
    }
}

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " [start|stop]" << std::endl;
        return 1;
    }

    std::string command = argv[1];
    if (command == "start") {
        start();
    } else if (command == "stop") {
        stop();
    } else {
        std::cerr << "Invalid command. Use 'start' or 'stop'." << std::endl;
        return 1;
    }

    return 0;
}

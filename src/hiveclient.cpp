#include "hivesupport.hpp"

int main(int argc, char** argv) {
    std::string mqueue = "hivemqueue";
    std::string PID_FILE = "/tmp/hive";
    int clientIdentifier = 0;
    int daemonIdentifier = 1; 

    std::vector<std::string> args = ConvertArgsToVector(argc, argv);
    char buffer[1024];
    strcpy(buffer, Serialize(args).c_str());
    
    if (WriteArgsMQueue(mqueue, buffer, daemonIdentifier) == 0) {
        std::cerr << "Failed to write to message queue" << std::endl;
        return 1;
    }
    // std::cout << "Writing to queue..." << std::endl;

    // Get daemon response
    std::vector<std::string> data;
    while (1) {
        IpcMessage msg = ReadArgsMQueue(mqueue);
        if (msg.status && msg.identifier == clientIdentifier) {
            data = Deserialize(msg.msg);
            ClearMqueue(msg.msgId); // Only clear after processing the message
            break;
        }else if(msg.status){
            ClearMqueue(msg.msgId);
            WriteArgsMQueue(mqueue,msg.msg,msg.identifier);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(500)); // Allow time for the daemon to respond
    }

    std::cout << "Response from daemon:" << std::endl;
    for (const auto& a : data) {
        std::cout << a << std::endl;
    }

    return 0;  // Return success
}

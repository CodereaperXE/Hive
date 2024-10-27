#pragma once
#include <vector>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string>
#include <cstring>
#include <iostream>
#include <chrono>
#include <thread>

#include <csignal>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

class IpcMessage{
    public:
    IpcMessage(){status=0;}
    long msgType;
    char msg[1024];
    int status;
    int identifier;
    int msgId;
};


IpcMessage ReadArgsMQueue(std::string);

int WriteArgsMQueue(std::string,char[],int);

int ClearMqueue(int);
 
std::string Serialize(std::vector<std::string>);

std::vector<std::string> Deserialize(char[]);

std::vector<std::string> ConvertArgsToVector(int ,char** );

int ReadPidFile(std::string);

int CreatePidFile(std::string);

int CheckPidFile(std::string);

int RemovePidFile(std::string);

std::vector<std::vector<std::string>> GetSavedSchedulerData(const std::string);
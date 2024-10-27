#include "hivesupport.hpp"
#include <fstream>
#include <csignal>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


std::vector<std::string> ConvertArgsToVector(int argc,char** argv){
    std::vector<std::string> output;
    for(int i=0;i<argc;i++){
        output.push_back(argv[i]);
    }
    return output;
}


std::string Serialize(std::vector<std::string> msg){
    std::string output;
    for(auto elem:msg){
        output+=elem+'|';
    }
    return output;  
}

std::vector<std::string> Deserialize(char buffer[]){
    std::vector<std::string> output;
    std::string temp = std::string(buffer);
    std::string element;
    for(auto elem : temp){
        if(elem!='|') element+=elem;
        else{
            output.push_back(element);
            element.clear();
        }
    }
    return output;
}

IpcMessage ReadArgsMQueue(std::string fileName){
    key_t key = ftok(fileName.c_str(),65);
    int msgId = msgget(key,0666);

    if(msgId==-1){
        // std::cerr<<"error in accessing message"<<std::endl;
        return IpcMessage();
    }
    IpcMessage message;

    //recieve message queue
    // std::cout<<"recv message data"<<std::endl;
    if(msgrcv(msgId,&message,sizeof(message) - sizeof(message.msgType),0,0)==-1)
    {
        std::cerr<<"error in recieving message"<<std::endl;
        // ClearMqueue(msgId);
        return IpcMessage();
    }
    // std::cout<<"recived message data"<<std::endl;
    //remove message queue
    // if(msgctl(msgId,IPC_RMID, nullptr)==-1) {
    //     std::cerr<<"error in removing message queue"<<std::endl;
    //     return IpcMessage();
    // }
    message.msgId=msgId;
    message.status=1;
    return message;

}

int WriteArgsMQueue(std::string fileName,char msg[],int identifier){
    key_t key = ftok(fileName.c_str(),65);
    //create message queue
    int msgId = msgget(key,0666 | IPC_CREAT);

    if (msgId == -1) {
        std::cerr << "Error creating message queue: " << strerror(errno) << std::endl;
        return 0;
    }

    IpcMessage message;
    message.msgType=1;
    message.msgId=msgId;
    message.identifier=identifier;
    strcpy(message.msg,msg);

    message.status=1;
    //send data in message queue
    if (msgsnd(msgId, &message, sizeof(message)-sizeof(message.msgType), 0) == -1) {
        std::cerr << "Error sending message: " << strerror(errno) << std::endl;
        return 0;
    }
    return 1;
}

int ClearMqueue(int msgId){
    if(msgctl(msgId,IPC_RMID, nullptr)==-1) {
        std::cerr<<"error in removing message queue"<<std::endl;
        return 1;
    }else return 0;
}

int CreatePidFile(std::string PID_FILE){
    pid_t pid = getpid();
    std::ofstream pidFile(PID_FILE);
    pidFile << pid;
    pidFile.close();
    return 1;
}

int ReadPidFile(std::string PID_FILE){
    std::ifstream pidFile(PID_FILE);
    if(pidFile.is_open()){
        int pid;
        pidFile >> pid;
        pidFile.close();
        return pid;
    }
    return -1;
}

int RemovePidFile(std::string PID_FILE){
    std::ifstream pidFile(PID_FILE);
    if(!pidFile.is_open()){
        std::cout<<"pid file doesnt exists"<<std::endl;
        return 0;
    } 
    pidFile.close();
    std::remove(PID_FILE.c_str());
    return 1;
}

int CheckPidFile(std::string PID_FILE){
    std::ifstream pidFile(PID_FILE);
    if(pidFile.is_open()){
        pidFile.close();
        return 1;
    }
    return 0;
}



std::vector<std::vector<std::string>> GetSavedSchedulerData(const std::string fileName){
    std::ifstream file(fileName,std::ios::binary);
    std::vector<std::vector<std::string>> data;
    std::string line;
    while(std::getline(file,line)){
        std::stringstream ss(line);
        std::string value;
        std::vector<std::string> temp;
        while(std::getline(ss,value,',')){
            temp.push_back(value);
        }
        data.push_back(temp);
    }
    file.close();
    //remove file
    if(fs::exists(fileName)){
        fs::remove(fileName);
    }
    return data;
}


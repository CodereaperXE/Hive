#include <iostream>
#include <cstring>
#include <errno.h>
#include <vector>
#include "hivesupport.hpp"
#include "hivescheduler.hpp"
#include "hivebackup.hpp"
#include "hiveclihandler.hpp"



// void SignalHandler(int signum){
    
// }

// HiveBackup* backupObj;
HiveScheduler* schedulerObj;

int main(){
// signal(SIGUSR1,SignalHandler);
std::string mqueue = "hivemqueue";
int clientIdentifier=0;
int daemonIdentifier=1;
std::string PID_FILE = "/tmp/hive";
CreatePidFile(PID_FILE);

std::vector<std::string> args;
//recieve message (commands from cli)

//creating scheduler object
schedulerObj = new HiveScheduler();

while(1){

    while(1){
        IpcMessage data = ReadArgsMQueue(mqueue);
        // std::cout<<"size" <<data.msg<<std::endl;
        // std::cout<<"mq"<<data.msg<<std::endl;
        if(data.status && data.identifier==daemonIdentifier){
            args=Deserialize(data.msg);
            ClearMqueue(data.msgId);
            // std::cout<<"read queue"<<std::endl;
            break;
        }else if(data.status){
            ClearMqueue(data.msgId);
            WriteArgsMQueue(mqueue,data.msg,data.identifier);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
    
    std::cout<<"running"<<std::endl;
    MatchParamOp data = MatchParams(args);
    std::cout<<data.backupName;
    std::cout<<data.schedulerFlag;
    
    //main code
    HiveBackup backupObj(data.src,data.dst,data.mode);
    if(schedulerObj->SchedulerStatus())
        schedulerObj->StopScheduler();
    if(data.schedulerFlag){
        schedulerObj->AddSchedule(data.backupName,data.days,data.hours,data.minutes,backupObj);
    }

    schedulerObj->StartScheduler();

    char buffer[128];


    std::vector<std::string> v = {std::to_string(data.minutes),data.src,data.dst , std::to_string(data.schedulerFlag)};
    strcpy(buffer,Serialize(v).c_str());
    WriteArgsMQueue(mqueue,buffer,clientIdentifier);
    
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

}

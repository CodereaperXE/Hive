#include <iostream>
#include <cstring>
#include <errno.h>
#include <vector>
#include "hivesupport.hpp"
#include "hivescheduler.hpp"
#include "hivebackup.hpp"
#include "hiveclihandler.hpp"


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
    //for debugging
    // std::cout<<data.backupName;
    // std::cout<<data.schedulerFlag;
    
    //main code
    HiveBackup backupObj(data.src,data.dst,data.mode);

    //if scheduler running stop scheduler temporarily to add task
    if(schedulerObj->SchedulerStatus())
        schedulerObj->StopScheduler();
    if(data.schedulerFlag){
        schedulerObj->AddSchedule(data.backupName,data.days,data.hours,data.minutes,backupObj);
    }

    //start scheduler
    schedulerObj->StartScheduler();

    char buffer[1024];
    std::vector<std::string> response;
    //getting all runnning jobs in scheduler
    if (data.getRunningJobs) response.push_back(schedulerObj->GetRunningJobs());
    
    //write into response buffer to send to client
    strncpy(buffer,Serialize(response).c_str(),sizeof(buffer));
    WriteArgsMQueue(mqueue,buffer,clientIdentifier);
    
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

}

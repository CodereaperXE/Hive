#include <iostream>
#include <cstring>
#include <errno.h>
#include <vector>
#include "hivesupport.hpp"
#include "hivescheduler.hpp"
#include "hivebackup.hpp"
#include "hiveclihandler.hpp"


//hive scheduler object
HiveScheduler* schedulerObj=nullptr;
int terminateFlag=0;
int poweroffStatus=0; //resets on poweroff

void SignalHandler(int signum) {
    std::cout << "Received SIGTERM" << std::endl;

    // Open the file in append mode and binary mode
    std::ofstream file("/home/rishab/projects/hive/build/in.txt", std::ios::app | std::ios::binary);
    
    // Check if the file opened successfully
    if (!file) {
        std::cerr << "Error opening file for appending." << std::endl;
        return; // Exit the function instead of the program
    }


    if (schedulerObj != nullptr) {
        std::vector<std::string> data = schedulerObj->GetSignalHandlerData();
        for (const std::string& elem : data) { 
            std::cout << elem << std::endl;
            file << elem;
            file <<"\n";
        }
    }
    file.close();
    terminateFlag=1;
}

int GetSavedData(){
    if(!poweroffStatus){
        if(schedulerObj !=nullptr){
            auto data = GetSavedSchedulerData("/home/rishab/projects/hive/build/in.txt");
            //stop scheduler if running
            if(schedulerObj->SchedulerStatus()){
                schedulerObj->StopScheduler();
            }
            for(auto element : data){
                auto name = element[0];
                auto sourcePath = fs::path(element[1]);
                auto destinationPath = fs::path(element[2]);
                auto mode = BackupMode(stoi(element[3]));
                auto duration = stoi(element[4]);
                HiveBackup backupObj(sourcePath,destinationPath,mode);
                schedulerObj->AddSchedule(name,0,0,duration,backupObj);
                std::cout<<name<<sourcePath<<destinationPath<<mode<<duration<<std::endl;
            }
        }
    }
    poweroffStatus=1;
    //start scheduler
    if(!schedulerObj->SchedulerStatus()) schedulerObj->StartScheduler();
    return 1;
}

int init(){
    GetSavedData();
    return 1;
}

int main(){
// signal(SIGUSR1,SignalHandler);
signal(SIGTERM,SignalHandler);



std::string mqueue = "hivemqueue";
int clientIdentifier=0;
int daemonIdentifier=1;
std::string PID_FILE = "/tmp/hive";
CreatePidFile(PID_FILE);

std::vector<std::string> args;
//recieve message (commands from cli)

//creating scheduler object
schedulerObj = new HiveScheduler();

//startup procedure (always after scheduler object created)
init();


while(1){
    if(terminateFlag) break;
    //wait for command line params
    while(1){
        if(terminateFlag) break;

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
    //decode cli params
    MatchParamOp data = MatchParams(args);
    
    //main code
    //create backup obj
    HiveBackup backupObj(data.src,data.dst,data.mode);

    //add schedule
    if(data.addScheduleFlag){
        //if scheduler running stop scheduler temporarily to add task
        if(schedulerObj->SchedulerStatus())
            schedulerObj->StopScheduler();
        schedulerObj->AddSchedule(data.backupName,data.days,data.hours,data.minutes,backupObj);
    }

    char buffer[1024];
    std::vector<std::string> response; //response to client
    //getting all runnning jobs in scheduler
    if (data.getRunningJobs) response.push_back(schedulerObj->GetRunningJobs());

    //removing job from scheduler
    if(data.removeRunningJob) {
        if(schedulerObj->SchedulerStatus())
            schedulerObj->StopScheduler();
        schedulerObj->RemoveRunningJob(data.removeRunningJobName);
        response.push_back("Removed job "+ data.removeRunningJobName);
    }

    //start scheduler
    if(!schedulerObj->SchedulerStatus()) schedulerObj->StartScheduler();
    
    //write into response buffer to send to client
    strncpy(buffer,Serialize(response).c_str(),sizeof(buffer));
    WriteArgsMQueue(mqueue,buffer,clientIdentifier);
    
    // std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

}

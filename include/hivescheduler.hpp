#pragma once
#include <iostream>
#include <filesystem>
#include <vector>
#include <chrono>
#include <thread>  
#include <memory>
#include <functional>
#include "hivebackup.hpp"


class BackupJob{
    public:
    std::string jobName;
    std::chrono::minutes targetDuration;
    std::chrono::system_clock::time_point nextSchedule;
    HiveBackup backupObj;
    BackupJob(std::string jobName,int targetDuration,HiveBackup backupObj) : jobName(jobName), targetDuration(targetDuration), backupObj(backupObj){}
};

class HiveScheduler{
    public:
     
    ~HiveScheduler();
    
    int AddSchedule(std::string,int,int,int,HiveBackup);
    void PrintJob(std::string& name);
    
    
    void StartScheduler();
    void StopScheduler();

    int SchedulerStatus();

    std::string GetRunningJobs();
    int RemoveRunningJob(std::string);

    //signal handler data
    std::vector<std::string> GetSignalHandlerData();
    
    private:
   
    void RunScheduler();
    void RunSchedulerThreaded(HiveBackup&);

    std::vector<BackupJob> jobList;
    int schedulerThreadFlag;
    int schedulerStatus;
    std::unique_ptr<std::thread> schedulerThreadPtr;
};
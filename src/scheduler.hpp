#include <iostream>
#include <filesystem>
#include <vector>
#include <chrono>
#include <thread>  
#include <memory>
#include <functional>


class BackupJob{
    public:
    std::string jobName;
    std::chrono::minutes targetDuration;
    std::chrono::system_clock::time_point nextSchedule;
    BackupJob(std::string jobName,int targetDuration) : jobName(jobName), targetDuration(targetDuration){}
};

class HiveScheduler{
    public:
     
    ~HiveScheduler();
    
    int AddSchedule(std::string,int,int,int);
    void PrintJob(std::string& name);
    
    
    void StartScheduler();
    void StopScheduler();
    
    private:
   
    void RunScheduler();

    std::vector<BackupJob> jobList;
    int schedulerThreadFlag;
    std::unique_ptr<std::thread> schedulerThreadPtr;
};
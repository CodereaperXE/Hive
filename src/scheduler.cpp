#include <iostream>
#include <filesystem>
#include <vector>
#include <chrono>


class BackupJob{
    public:
    std::string jobName;
    std::chrono::minutes targetDuration;
    std::chrono::system_clock::time_point nextSchedule;
    BackupJob(std::string jobName,int targetDuration) : jobName(jobName), targetDuration(targetDuration){}
};

std::vector <BackupJob> v;

void AddSchedule(std::string jobName,int minuteDuration,std::vector<BackupJob>& jobList){
    std::chrono::system_clock::time_point curTime = std::chrono::system_clock::now();
    BackupJob job(jobName,minuteDuration);
    job.nextSchedule=curTime + job.targetDuration;
    jobList.push_back(job);
    
}

void PrintJob(std::string& name){
    std::cout<<name<<std::endl;
}

void Run(std::vector<BackupJob>& jobList){
    while(1){
    
        for(auto curJob = jobList.begin();curJob!=jobList.end();curJob++){
            if(std::chrono::system_clock::now() >= curJob->nextSchedule) {
                curJob->nextSchedule = std::chrono::system_clock::now() + curJob->targetDuration;
                PrintJob(curJob->jobName);
            }
        }
    }
}

int main(){
    std::string name = "first";
    std::string name2= "second";
    AddSchedule(name,1,v);
    AddSchedule(name2,2,v);
    Run(v);
}
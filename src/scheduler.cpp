#include "scheduler.hpp"


HiveScheduler::~HiveScheduler(){
    schedulerThreadFlag=0;
}

int HiveScheduler::AddSchedule(std::string jobName,int dayDuration, int hourDuration, int minuteDuration){
    std::chrono::system_clock::time_point curTime = std::chrono::system_clock::now();
    BackupJob job(jobName,dayDuration + hourDuration + minuteDuration);
    job.nextSchedule = curTime + job.targetDuration;
    jobList.push_back(job);
    return 0;
}

void HiveScheduler::PrintJob(std::string& name){
    std::cout<<name<<std::endl;
}

void HiveScheduler::RunScheduler(){
    while(!schedulerThreadFlag){ 
        for(auto curJob = jobList.begin();curJob!=jobList.end();curJob++){
            if(std::chrono::system_clock::now() >= curJob->nextSchedule) {
                curJob->nextSchedule = std::chrono::system_clock::now() + curJob->targetDuration;
                PrintJob(curJob->jobName);
            }
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
}

void HiveScheduler::StartScheduler(){
    schedulerThreadFlag = 0;
    schedulerThreadPtr = std::make_unique<std::thread>(std::bind(&HiveScheduler::RunScheduler,this));
}

void HiveScheduler::StopScheduler(){
    schedulerThreadFlag=1;
    schedulerThreadPtr->join();
}


int main(){
    HiveScheduler h;
    h.AddSchedule("first",0,0,1);
    h.StartScheduler();

    std::this_thread::sleep_for(std::chrono::seconds(70));
    h.StopScheduler();
}
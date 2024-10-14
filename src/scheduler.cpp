#include "scheduler.hpp"


int HiveScheduler::AddSchedule(std::string jobName,int dayDuration, int hourDuration, int minuteDuration, std::vector<BackupJob>& jobList){
    std::chrono::system_clock::time_point curTime = std::chrono::system_clock::now();
    BackupJob job(jobName,dayDuration + hourDuration + minuteDuration);
    job.nextSchedule = curTime + job.targetDuration;
    jobList.push_back(job);
    return 0;
}

void HiveScheduler::PrintJob(std::string& name){
    std::cout<<name<<std::endl;
}

void HiveScheduler::RunScheduler(std::vector<BackupJob>& jobList){
    while(1){
    
        for(auto curJob = jobList.begin();curJob!=jobList.end();curJob++){
            if(std::chrono::system_clock::now() >= curJob->nextSchedule) {
                curJob->nextSchedule = std::chrono::system_clock::now() + curJob->targetDuration;
                PrintJob(curJob->jobName);
            }
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
}


int main(){
    HiveScheduler h;
    std::vector<BackupJob> j;
    h.AddSchedule("first",0,0,1,j);
    h.RunScheduler(j);
}
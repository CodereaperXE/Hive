#include "scheduler.hpp"


HiveScheduler::~HiveScheduler(){
    schedulerThreadFlag=0;
}

int HiveScheduler::AddSchedule(std::string jobName,int dayDuration, int hourDuration, int minuteDuration, HiveBackup backupObj){
    std::chrono::system_clock::time_point curTime = std::chrono::system_clock::now();
    BackupJob job(jobName,dayDuration + hourDuration + minuteDuration, backupObj);
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
                //needs to be threaded as backup takes time
                curJob->backupObj.StartBackup();
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

    
    
    fs::path source{"./something/"};
    fs::path dest{"./destination/"};
    
    HiveBackup b(source,dest,VERSIONED);


    HiveScheduler h;
    
    h.AddSchedule("first",0,0,1,b);
    h.StartScheduler();

    std::this_thread::sleep_for(std::chrono::seconds(70));
    h.StopScheduler();
}
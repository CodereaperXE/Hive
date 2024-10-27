#include "hivescheduler.hpp"

//default constructtor
HiveScheduler::~HiveScheduler(){
    schedulerThreadFlag=0;
}

//add backup schedule for joblist
int HiveScheduler::AddSchedule(std::string jobName,int dayDuration, int hourDuration, int minuteDuration, HiveBackup backupObj){
    std::chrono::system_clock::time_point curTime = std::chrono::system_clock::now();
    BackupJob job(jobName,dayDuration*24*60 + hourDuration*60 + minuteDuration, backupObj);
    job.nextSchedule = curTime + job.targetDuration;
    jobList.push_back(job);
    return 0;
}

//prints backupjob name
void HiveScheduler::PrintJob(std::string& name){
    std::cout<<name<<std::endl;
}

//theaded implementation
void HiveScheduler::RunSchedulerThreaded(HiveBackup& backupObj){
    std::thread backupThread(&HiveBackup::StartBackup,&backupObj);
    backupThread.detach(); //risky approach (not recommended and has to be changed in future)
}

//run the scheduler (private callback from startscheduler function, threaded implementation)
void HiveScheduler::RunScheduler(){
    while(!schedulerThreadFlag){ 
        for(auto curJob = jobList.begin();curJob!=jobList.end();curJob++){
            if(std::chrono::system_clock::now() >= curJob->nextSchedule) {
                curJob->nextSchedule = std::chrono::system_clock::now() + curJob->targetDuration;
                PrintJob(curJob->jobName);
                //needs to be threaded as backup takes time
                // curJob->backupObj.StartBackup();

                //threaded implementation
                // std::cout<<curJob->backupObj.GetBackupStatus();
                if(!curJob->backupObj.GetBackupStatus())
                    HiveScheduler::RunSchedulerThreaded(curJob->backupObj);
                    // curJob->backupObj.StartBackup();
                else std::cout<<"backup busy"<<std::endl;
            }
            
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

//start scheduler thread
void HiveScheduler::StartScheduler(){
    schedulerThreadFlag = 0;
    schedulerStatus=1;
    schedulerThreadPtr = std::make_unique<std::thread>(std::bind(&HiveScheduler::RunScheduler,this));
}


//stop scheduler and wait for thread to stop
void HiveScheduler::StopScheduler(){
    schedulerThreadFlag=1;
    schedulerStatus=0;
    schedulerThreadPtr->join();
}

//scheduler status
int HiveScheduler::SchedulerStatus(){
    return schedulerStatus;
}

//get running jobs
std::string HiveScheduler::GetRunningJobs(){
    std::string jobs;
    for(auto job : jobList){
        jobs+=job.jobName + " " + job.backupObj.GetBackupInfo() + "\n"; 
    }
    return jobs;
}

//remove running jobs
int HiveScheduler::RemoveRunningJob(std::string jobName){
    int index=-1;
    for(int i=0;i<jobList.size();i++){
        if(jobName == jobList[i].jobName) {
            index = i;
            break;
        }
    }
    jobList.erase(jobList.begin()+index);
    return 1;

}


//signal handler data

std::vector<std::string> HiveScheduler::GetSignalHandlerData(){
    std::vector<std::string> data;
    for(auto job : jobList){
        std::string jobName  = job.jobName;
        std::string sourcePath = job.backupObj.GetSourcePath();
        std::string destinationPath = job.backupObj.GetDestinationPath();
        std::string mode = std::to_string(job.backupObj.GetMode());
        std::string duration = std::to_string(job.targetDuration.count());
        std::string op = jobName + "," + sourcePath +","+destinationPath+","+mode+","+duration;
        data.push_back(op);
    }
    return data;
}

// int main(){

    
    
//     fs::path source{"./something/"};
//     fs::path dest{"./destination/"};

//     //fs::path s{"./something/"};
//     // fs::path d{"./dst/"};
    
//     HiveBackup b(source,dest,DIFFERENTIAL);
//     // HiveBackup c(s,d,VERSIONED);

//     HiveScheduler h;
    
//     h.AddSchedule("first",0,0,0,b);
//     // h.AddSchedule("second",0,0,0,c);
//     h.StartScheduler();

//     std::this_thread::sleep_for(std::chrono::seconds(70));
//     h.StopScheduler();
// }
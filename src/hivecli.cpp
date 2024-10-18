#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <filesystem>
#include "scheduler.hpp"
#include "hive.hpp"
#include <map>

namespace fs = std::filesystem;


int FindIndex(std::string target,std::vector<std::string>& argv){
    auto it = std::find(argv.begin(),argv.end(),target);
    return (it!=argv.end()) ? std::distance(argv.begin(),it) : -1;
}

void MatchParams(std::map<std::string,int>& targetParams,std::vector<std::string>& argv){

    if(targetParams["--help"]){
        std::cout<<"Usage: Hive\t [-b <source> <destination> <backup mode (Diffential/Versioned)>],\n\t\t [-b <params(same is -b)> -s <backup name> <days> <hours> <minutes>]\n\t\t Example usage:\n\t\t ./hivecli -b src dst DIFFERENTIAL/VERSIONED \n\t\t ./hivecli -b src dst DIFFERENTIAL/VERSIONED -s name 0 0 0 0" <<std::endl;

    }
    else if(targetParams["-b"] && targetParams["-s"]){
        int indexB = FindIndex("-b",argv);
        fs::path src(argv[indexB+1]);
        fs::path dst(argv[indexB+2]);

        auto mode = [&argv,&indexB]() -> BackupMode{
            if(argv[indexB+3]=="DIFFERENTIAL") return DIFFERENTIAL;
            else if(argv[indexB+3]=="VERSIONED") return VERSIONED;
            else return DIFFERENTIAL;
        };


        int indexS = FindIndex("-s",argv);
        std::string backupName = argv[indexS + 1];
        int days = std::stoi(argv[indexS + 2]);
        int hours = std::stoi(argv[indexS + 3]);
        int minutes = std::stoi(argv[indexS + 4]);

       
        HiveBackup backupObj(src,dst,mode());
        
        HiveScheduler backupScheduler;
        backupScheduler.AddSchedule(backupName,days,hours,minutes,backupObj);
        backupScheduler.StartScheduler();
        // std::cout<<src<<"  "<<dst<<" "<< days <<hours<< minutes<<std::endl;
        while(1){}

    }
    else if(targetParams["-b"]){
        int indexB = FindIndex("-b",argv);
        fs::path src(argv[indexB+1]);
        fs::path dst(argv[indexB+2]);

         auto mode = [&argv,&indexB]() -> BackupMode{
            if(argv[indexB+3]=="DIFFERENTIAL") return DIFFERENTIAL;
            else if(argv[indexB+3]=="VERSIONED") return VERSIONED;
            else return DIFFERENTIAL;
        };
        
        HiveBackup backupObj;
        backupObj.Backup(src,dst,mode());
        
    }
}


int main(int argc,char** argv){
    std::vector<std::string> cliArgs;

    if(argc <= 1) {
        std::cout<<"Try --help to know more"<<std::endl;
        return 0;
    }

    for(int i=0;i<argc;i++){
        cliArgs.push_back(argv[i]);
    }

    std::map<std::string,int> paramMp;
    for(auto elem:cliArgs){
        paramMp[elem]++;
    }

    MatchParams(paramMp,cliArgs);
    
    

}   
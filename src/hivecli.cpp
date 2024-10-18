#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <filesystem>
#include "scheduler.hpp"
#include "hive.hpp"
#include <map>

namespace fs = std::filesystem;


int findIndex(std::string target,std::vector<std::string>& argv){
    auto it = std::find(argv.begin(),argv.end(),target);
    return (it!=argv.end()) ? std::distance(argv.begin(),it) : -1;
}

void match(std::map<std::string,int>& targetParams,std::vector<std::string>& argv){
    if(targetParams["-b"] && targetParams["-s"]){
        int indexB = findIndex("-b",argv);
        fs::path src(argv[indexB+1]);
        fs::path dst(argv[indexB+2]);

        auto mode = [&argv,&indexB]() -> BackupMode{
            if(argv[indexB+3]=="DIFFERENTIAL") return DIFFERENTIAL;
            else if(argv[indexB+3]=="VERSIONED") return VERSIONED;
            else return DIFFERENTIAL;
        };


        int indexS = findIndex("-s",argv);
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
        int indexB = findIndex("-b",argv);
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

std::vector<std::string> cliArgs;
int main(int argc,char** argv){
    for(int i=0;i<argc;i++){
        cliArgs.push_back(argv[i]);
    }

    std::map<std::string,int> paramMp;
    for(auto elem:cliArgs){
        paramMp[elem]++;
    }


    match(paramMp,cliArgs);
    
    

}   
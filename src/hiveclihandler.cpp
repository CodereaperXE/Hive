#include "hiveclihandler.hpp"



int FindIndex(std::string target,std::vector<std::string>& argv){
    auto it = std::find(argv.begin(),argv.end(),target);
    return (it!=argv.end()) ? std::distance(argv.begin(),it) : -1;
}

MatchParamOp MatchParams(std::vector<std::string>& argv){
    std::map<std::string,int> targetParams;
    for(auto param : argv){
        targetParams[param]++;
    }
    MatchParamOp output;
    if(targetParams["--help"]){
        output.helpFlag=1;
        output.helpMsg="Usage: Hive\t [-b <source> <destination> <backup mode (Diffential/Versioned)>],\n\t\t [-b <params(same is -b)> -s <backup name> <days> <hours> <minutes>]\n\t\t Example usage:\n\t\t ./hivecli -b src dst DIFFERENTIAL/VERSIONED \n\t\t ./hivecli -b src dst DIFFERENTIAL/VERSIONED -s name 0 0 0 0";

    }
    else if(targetParams["-b"] && targetParams["-s"]){
        int indexB = FindIndex("-b",argv);
        output.src=argv[indexB+1];
        output.dst=argv[indexB+2];

        auto mode = [&argv,&indexB]() -> BackupMode{
            if(argv[indexB+3]=="DIFFERENTIAL") return DIFFERENTIAL;
            else if(argv[indexB+3]=="VERSIONED") return VERSIONED;
            else return DIFFERENTIAL;
        };


        int indexS = FindIndex("-s",argv);
        output.backupName=argv[indexS + 1];
        output.days = std::stoi(argv[indexS + 2]);
        output.hours = std::stoi(argv[indexS + 3]);
        output.minutes = std::stoi(argv[indexS + 4]);

       
        // HiveBackup backupObj(src,dst,mode());
        
        // HiveScheduler backupScheduler;
        output.schedulerFlag=1;
        // backupScheduler.AddSchedule(backupName,days,hours,minutes,backupObj);
        // backupScheduler.StartScheduler();
        // std::cout<<src<<"  "<<dst<<" "<< days <<hours<< minutes<<std::endl;
        // while(1){}

    }
    else if(targetParams["-b"]){
        int indexB = FindIndex("-b",argv);
        output.src=argv[indexB+1];
        output.dst=argv[indexB+2];

         auto mode = [&argv,&indexB]() -> BackupMode{
            if(argv[indexB+3]=="DIFFERENTIAL") return DIFFERENTIAL;
            else if(argv[indexB+3]=="VERSIONED") return VERSIONED;
            else return DIFFERENTIAL;
        };
        
        // HiveBackup backupObj;
        // backupObj.Backup(src,dst,mode());
        output.mode=mode();
        
    }
    return output;
}


// int HandleCli(int argc,char** argv){
//     std::vector<std::string> cliArgs;

//     if(argc <= 1) {
//         std::cout<<"Try --help to know more"<<std::endl;
//         return 0;
//     }

//     for(int i=0;i<argc;i++){
//         cliArgs.push_back(argv[i]);
//     }

//     std::map<std::string,int> paramMp;
//     for(auto elem:cliArgs){
//         paramMp[elem]++;
//     }

//     MatchParams(paramMp,cliArgs);
//     return 1;
// }   
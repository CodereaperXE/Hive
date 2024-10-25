#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <filesystem>
#include "hivescheduler.hpp"
#include "hivebackup.hpp"
#include <map>

namespace fs = std::filesystem;

class MatchParamOp{
    public:
    MatchParamOp(){
        days=0;
        hours=0;
        minutes=0;
        schedulerFlag=0;
        helpFlag=0;
        mode=DIFFERENTIAL;
    }

    int days;
    int hours;
    int minutes;
    BackupMode mode;
    int schedulerFlag;
    std::string backupName;
    fs::path src;
    fs::path dst;
    int helpFlag;
    std::string helpMsg;
    int getRunningJobs;
};

MatchParamOp MatchParams(std::vector<std::string>& );

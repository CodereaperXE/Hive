#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <filesystem>
#include "./src/scheduler.hpp"
#include "./src/hive.hpp"
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

        int indexS = findIndex("-s",argv);
        int days = std::stoi(argv[indexS + 1]);
        int hours = std::stoi(argv[indexS + 2]);
        int minutes = std::stoi(argv[indexS + 3]);

        
        std::cout<<src<<"  "<<dst<<" "<< days <<hours<< minutes<<std::endl;


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
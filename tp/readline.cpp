#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <fstream>


std::vector<std::vector<std::string>> GetSavedSchedulerData(const std::string fileName){
    std::ifstream file(fileName,std::ios::binary);
    std::vector<std::vector<std::string>> data;
    std::string line;
    while(std::getline(file,line)){
        std::stringstream ss(line);
        std::string value;
        std::vector<std::string> temp;
        while(std::getline(ss,value,',')){
            temp.push_back(value);
        }
        data.push_back(temp);
    }
    file.close();
}
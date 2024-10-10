#include <iostream>
#include <filesystem>
#include <zlib.h>
#include <fstream>
#include <ostream>
#include "hive.hpp"

namespace fs = std::filesystem;

void HiveBackup::CreateVersion(fs::path fPath){
    //check
    if(!fs::exists(fPath) || fs::is_directory(fPath)) {
        std::cout<< "path doesnt exists or path not file";
        return;
    }
    //getting current time
    std::string curDateTime = GetTime();
    //creating time path to create new dir with timestamp suffix
    fs::path dateTimePath = fs::path(fPath.string() + "_" + curDateTime.substr(0,10) +"_"+ curDateTime.substr(11,19));
    //if backup dir doesnt exists create 
    if(!fs::exists(fs::path(fPath.string()+"_hbk"))) fs::create_directory(fPath.string()+"_hbk");
    //creating dir with timestamp suffix inside backup dir
    fs::path finalFpath = fs::path(fPath.string()+"_hbk"+"/"+dateTimePath.filename().string());
    fs::create_directory(finalFpath);
    //moving file to backup dir/timestamp suffix dir
    fs::rename(fPath,finalFpath / fPath.filename());
}

std::string HiveBackup::GetTime(){
    // auto now = std::chrono::system_clock::now();
    std::time_t currentTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::ostringstream oss;
    oss << std::put_time(std::localtime(&currentTime), "%Y-%m-%d %X");
    return oss.str();
 }

void HiveBackup::Backup(fs::path& sourcePath, fs::path& destinationPath){
    //checking if dir/file exists in destination path
    fs::path tempdstPath = fs::path(destinationPath / sourcePath.filename());

    //if does not exists in destination directly copy from source to destination
    if(!fs::exists(tempdstPath)){
        //if dir recursively copy to dst from src
        if(fs::is_directory(sourcePath))
            fs::copy(sourcePath,tempdstPath,fs::copy_options::update_existing | fs::copy_options::recursive);
        //if file directly copy to dst from src
        if(fs::is_regular_file(sourcePath))
            fs::copy(sourcePath,tempdstPath,fs::copy_options::update_existing);
    }
    
    //if exists
    else if(fs::exists(tempdstPath)){
        //if dir then compare
        if(fs::is_directory(tempdstPath)){
            for(const auto& entry : fs::recursive_directory_iterator(sourcePath)){
                fs::path dstPath = fs::path(tempdstPath / fs::relative(entry.path(),sourcePath));
                // std::cout<<"df"<< dstPath.string()<<std::endl;
                if(!fs::exists(fs::path(entry.path())) || !fs::exists(dstPath)){
                    //(bug might exist) check if src file has backup existing
                    
                    std::cout<<"file or dir missing"<<entry.path()<<" "<<dstPath<<std::endl;
                } 
                //bug here (didnt consider backup taken)
                
                if(!CheckIntegrity(fs::path(entry.path()),dstPath)){
                    std::cout<<"changed" << dstPath <<std::endl;
                    //if file changed
                    //create versioned backup
                    CreateVersion(dstPath);
                    //put new changed file
                    fs::copy(entry.path(),dstPath);
                }
            }
            // std::cout<<"existing"<<std::endl;
        }
        //if file then compare
        if(fs::is_regular_file(tempdstPath)){
            std::cout<<"existing" << tempdstPath <<std::endl;
        }
    }

}

int HiveBackup::CheckIntegrity(const fs::path& path1,const fs::path& path2){
    unsigned long crc1=0,crc2=0;
    const size_t bufferSize=4096;
    char buffer[bufferSize];

    std::ifstream file1(path1,std::ios::binary);
    std::ifstream file2(path2,std::ios::binary);

    while(file1.read(buffer,bufferSize) || file1.gcount() > 0){
        crc1=crc32(crc1,reinterpret_cast<const unsigned char*>(buffer),file1.gcount());
    }

    std::fill(std::begin(buffer),std::end(buffer),'\0');

    while(file2.read(buffer,bufferSize) || file2.gcount() > 0){
        crc2=crc32(crc2,reinterpret_cast<const unsigned char*>(buffer),file2.gcount());
    }

    file1.close();
    file2.close();

    if(crc1==crc2) return 1;
    else return 0;
}

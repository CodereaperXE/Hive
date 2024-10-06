//5/10/2024 RB
#include <iostream>
#include <fstream>
#include <filesystem>
#include <zlib.h>

// #include "crc.hpp"

namespace fs=std::filesystem;

int CheckIntegrity(fs::path& path1, fs::path& path2){
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

int DifferentialCopy(fs::path& source, fs::path& destination){
    return CheckIntegrity(source,destination);
}

void OverwriteCopy(fs::path& source, fs::path& destination){
    fs::copy(source,destination,fs::copy_options::recursive | fs::copy_options::overwrite_existing);
}

void CheckFile(fs::path& path){
    if(fs::exists(path) && fs::is_regular_file(path)){
        std::cout<<"file exists"<<std::endl;
    }else if(fs::exists(path) && fs::is_directory(path)){
        std::cout<<"Directory exists"<<std::endl;
    }else{
        std::cout<<"Directory does not exist"<<std::endl;
    }

}

void Backup(fs::path& sourcePath, fs::path& destinationPath){
    CheckFile(sourcePath);
    
    fs::copy(sourcePath,destinationPath,fs::copy_options::update_existing);
    

}

int main(){
    // fs::path dirpath{"./"};

    // if(fs::exists(dirpath)){
    //     for(const auto& entry : fs::directory_iterator(dirpath)){
    //         if(fs::is_regular_file(entry.status()))
    //             std::cout<<"File"<<entry.path().filename() <<std::endl;
    //         if(fs::is_directory(entry.status()))
    //             std::cout<<"Directory"<<entry.path().filename() <<std::endl;

    //     }
    // }
    // fs::path dirpath{"./"};

    // for(const auto& entry : fs::recursive_directory_iterator(dirpath)){
    //     std::cout<<entry.path()<<std::endl;
    // }

    // fs::path dirpath{"new_directory"};

    // fs::create_directory(dirpath);
    
    fs::path s{"./test/s"};
    fs::path d{"./desk/s"};
    std::cout<< DifferentialCopy(s,d);


}

//5/10/2024 RB
#include <iostream>
#include <fstream>
#include <filesystem>

namespace fs=std::filesystem;

void CheckFile(fs::path path){
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
    
    fs::copy(sourcePath,destinationPath,fs::copy_options::overwrite_existing);
    

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


}

//5/10/2024 RB
#include <iostream>
#include <fstream> //for std::ifstream and ofstream
#include <filesystem>
#include <zlib.h> //for crc function use -lz

// #include "crc.hpp"

namespace fs=std::filesystem;

void CreateVersion(fs::path);

int CheckIntegrity(const fs::path& path1,const fs::path& path2){
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



#include <chrono> //for time
// #include <ctime>
#include <sstream> //for ostringstream

#include <string>

std::string GetTime(){
    // auto now = std::chrono::system_clock::now();
    std::time_t currentTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::ostringstream oss;
    oss << std::put_time(std::localtime(&currentTime), "%Y-%m-%d %X");
    return oss.str();
}


//implementation of versioning system

void CreateVersion(fs::path fPath){
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
    
    // fs::path s{"./test/"};
    // fs::path d{"./desk/"};
    // std::cout<< DifferentialCopy(s,d);
    // Backup(s,d);
    // ShowTime();
    // CreateVersion("./dfdfdfd");
    // std::ofstream file("./dfdfdfd_2024-10-07/tt");
    // file.write("hello",4);
    // file.close();
    // fs::rename("dfdfdfd_2024-10-07","./desk/df");
    
    // fs::rename("./test","./o/test");
    
    // CreateVersion(fs::path{"./something"});
    // fs::path s("./source/");
    // fs::path d("./ceurse/");
    // Backup(s,d);
    fs::path s{"./primary/"};
    fs::path d{"./backup/"};

    Backup(s,d);
}


//old implementation
int CheckIntegrity(const fs::path& path1, const fs::path& path2) {
    // size_t bufferSize = 4096; // Define a buffer size for reading files
    // char buffer[bufferSize];

    // std::ifstream file1(path1, std::ios::binary);
    // std::ifstream file2(path2, std::ios::binary);

    // // Check if both files opened successfully
    // if (!file1.is_open() || !file2.is_open()) {
    //     throw std::runtime_error("Failed to open one or both files.");
    // }

    // EVP_MD_CTX* ctx1 = EVP_MD_CTX_new();
    // EVP_MD_CTX* ctx2 = EVP_MD_CTX_new();
    
    // // Initialize the digest contexts for SHA-256
    // if (EVP_DigestInit_ex(ctx1, EVP_sha256(), nullptr) != 1) {
    //     EVP_MD_CTX_free(ctx1);
    //     EVP_MD_CTX_free(ctx2); // Free ctx2 as well on failure
    //     throw std::runtime_error("Failed to initialize digest for file 1");
    // }

    // if (EVP_DigestInit_ex(ctx2, EVP_sha256(), nullptr) != 1) {
    //     EVP_MD_CTX_free(ctx1);
    //     EVP_MD_CTX_free(ctx2);
    //     throw std::runtime_error("Failed to initialize digest for file 2");
    // }

    // // Read file1 and update hash
    // while (file1.read(buffer, bufferSize) || file1.gcount() > 0) {
    //     EVP_DigestUpdate(ctx1, buffer, file1.gcount());
    // }

    // // Read file2 and update hash
    // while (file2.read(buffer, bufferSize) || file2.gcount() > 0) {
    //     EVP_DigestUpdate(ctx2, buffer, file2.gcount());
    // }

    // unsigned char hash1[EVP_MAX_MD_SIZE];
    // unsigned char hash2[EVP_MAX_MD_SIZE];
    // unsigned int hash1Length;
    // unsigned int hash2Length;

    // // Finalize the digest for file 1
    // if (EVP_DigestFinal_ex(ctx1, hash1, &hash1Length) != 1) {
    //     EVP_MD_CTX_free(ctx1);
    //     EVP_MD_CTX_free(ctx2);
    //     throw std::runtime_error("Failed to finalize digest for file 1");
    // }

    // // Finalize the digest for file 2
    // if (EVP_DigestFinal_ex(ctx2, hash2, &hash2Length) != 1) {
    //     EVP_MD_CTX_free(ctx1);
    //     EVP_MD_CTX_free(ctx2);
    //     throw std::runtime_error("Failed to finalize digest for file 2");
    // }

    // // Clean up
    // EVP_MD_CTX_free(ctx1);
    // EVP_MD_CTX_free(ctx2);

    // file1.close();
    // file2.close();

    // // Compare the hashes
    // if (hash1Length != hash2Length || memcmp(hash1, hash2, hash1Length) != 0) {
    //     return 0; // Files are different
    // }

    // return 1; // Files are the same
}
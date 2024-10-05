#include <iostream>
#include <fstream>
#include <zlib.h>
#include <filesystem>
namespace fs = std::filesystem;

void CheckIntegrity(fs::path path1, fs::path path2){
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

    if(crc1==crc2) std::cout<<"true";
    else std::cout<<"false";


}


int main(){
    fs::path p{"./file"};
    fs::path q{"./file1"};
    CheckIntegrity(p,q);
}
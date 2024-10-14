#include "hive.hpp"
#include <iostream>
#include <filesystem>
#include "scheduler.hpp"

namespace fs = std::filesystem;




int main(){
    HiveBackup h;
    
    fs::path source{"/media/rishab/New Volume/lostdata/"};
    fs::path dest{"/media/rishab/New Volume/backup/"};
    
    h.Backup(source,dest,VERSIONED);

}
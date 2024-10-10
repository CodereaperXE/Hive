#include "hive.hpp"
#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;


int main(){
    HiveBackup h;
    
    fs::path source{"./hir/"};
    fs::path dest{"./fb/"};

    h.Backup(source,dest);

}
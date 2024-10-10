#include <iostream>
#include <filesystem>
#include <zlib.h>
#include <fstream>
#include <ostream>

namespace fs = std::filesystem;

#ifndef HIVE_HPP
#define HIVE_HPP

class HiveBackup{
    public:
    
    void Backup(fs::path&,fs::path&);

    private:
    int CheckIntegrity(const fs::path&, const fs::path&);
    void CreateVersion(fs::path);
    std::string GetTime();

    
};

#endif
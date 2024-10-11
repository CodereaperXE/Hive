#include <iostream>
#include <filesystem>
#include <zlib.h>
#include <fstream>
#include <ostream>
#include <tuple>
#include <vector>

namespace fs = std::filesystem;

#ifndef HIVE_HPP
#define HIVE_HPP

enum BackupMode{
    DIFFERENTIAL,
    VERSIONED
};

class HiveBackup{
    public:
    
    int Backup(fs::path&,fs::path&,BackupMode);



    private:
    std::tuple<std::vector<unsigned char>, unsigned int> GenerateHash(std::ifstream& );
    int CheckIntegrity(const fs::path&, const fs::path&);
    void CreateDifferentialBackup(fs::path,fs::path);
    void CreateVersionBackup(fs::path,fs::path);
    std::string GetTime();

    
};

#endif
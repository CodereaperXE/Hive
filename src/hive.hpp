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

class HiveBackup{
    public:
    
    void Backup(fs::path&,fs::path&);

    private:
    std::tuple<std::vector<unsigned char>, unsigned int> GenerateHash(std::ifstream& );
    int CheckIntegrity(const fs::path&, const fs::path&);
    void CreateVersion(fs::path);
    std::string GetTime();

    
};

#endif
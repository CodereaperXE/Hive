#include <iostream>
#include <filesystem>
#include <zlib.h>
#include <fstream>
#include <ostream>
#include <cstring>
#include <tuple>
#include <vector>
#include <openssl/evp.h>

namespace fs = std::filesystem;

#ifndef HIVE_HPP
#define HIVE_HPP

enum BackupMode{
    DIFFERENTIAL,
    VERSIONED
};

class HiveBackup{
    public:
    
    HiveBackup();
    HiveBackup(fs::path,fs::path,BackupMode);
    void StartBackup();
    int Backup(fs::path&,fs::path&,BackupMode);
    int GetBackupStatus();

    private:
    fs::path sourcePath;
    fs::path destinationPath;
    BackupMode mode;
    std::tuple<std::vector<unsigned char>, unsigned int> GenerateHash(std::ifstream& );
    int CheckIntegrity(const fs::path&, const fs::path&);
    void CreateDifferentialBackup(fs::path,fs::path);
    void CreateVersionBackup(fs::path,fs::path);
    std::string GetTime();
    int backupStatus;

    
};

#endif
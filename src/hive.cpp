#include "hive.hpp"


namespace fs = std::filesystem;


HiveBackup::HiveBackup(){ backupStatus=0; }
HiveBackup::HiveBackup(fs::path sourcePath ,fs::path destinationPath ,BackupMode mode) : sourcePath(sourcePath), destinationPath(destinationPath), mode(mode){ backupStatus = 0;}

//scheduler specific function
void HiveBackup::StartBackup(){
    Backup(sourcePath,destinationPath,mode);
}

int HiveBackup::GetBackupStatus(){
    return backupStatus;
}

//version backup function
void HiveBackup::CreateVersionBackup(fs::path versionFPath,fs::path newFPath){
    //check
    if(!fs::exists(versionFPath) || fs::is_directory(versionFPath)) {
        std::cout<< "path doesnt exists or path not file";
        return;
    }
    //getting current time
    std::string curDateTime = GetTime();
    //creating time path to create new dir with timestamp suffix
    fs::path dateTimePath = fs::path(versionFPath.string() + "_" + curDateTime.substr(0,10) +"_"+ curDateTime.substr(11,19));
    //if backup dir doesnt exists create 
    if(!fs::exists(fs::path(versionFPath.string()+"_hbk"))) fs::create_directory(versionFPath.string()+"_hbk");
    //creating dir with timestamp suffix inside backup dir
    fs::path finalFpath = fs::path(versionFPath.string()+"_hbk"+"/"+dateTimePath.filename().string());
    fs::create_directory(finalFpath);
    //moving file to backup dir/timestamp suffix dir
    fs::rename(versionFPath,finalFpath / versionFPath.filename());
    //moving new file to old file path
    fs::copy(newFPath,versionFPath);
    
}

//differential backup function
void HiveBackup::CreateDifferentialBackup(fs::path backupFPath, fs::path newFPath){
    fs::copy(newFPath,backupFPath,fs::copy_options::overwrite_existing);
}

std::string HiveBackup::GetTime(){
    // auto now = std::chrono::system_clock::now();
    std::time_t currentTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::ostringstream oss;
    oss << std::put_time(std::localtime(&currentTime), "%Y-%m-%d %X");
    return oss.str();
 }

int HiveBackup::Backup(fs::path& sourcePath, fs::path& destinationPath,BackupMode mode){
    //set backup status busy
    backupStatus=1;
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
                    
                    std::cout<<"File or directory missing "<<entry.path()<<" "<<dstPath<<std::endl;
                    //adding bug fix by copying file or directory
                    //if directory recursive copy;
                    if(fs::is_directory(entry.path())){
                        std::cout<<"Adding missing directory "<<entry.path()<<" "<<dstPath<<std::endl;
                        fs::copy(entry.path(),dstPath,fs::copy_options::recursive);
                    }
                    //if regular file then copy
                    if(fs::is_regular_file(entry.path())){
                        std::cout<<"Adding missing file "<<entry.path()<<" "<<dstPath<<std::endl;
                        fs::copy(entry.path(),dstPath,fs::copy_options::overwrite_existing);
                    }
                    // return 0;
                } 
                //bug here (didnt consider backup taken)
                
                if(!CheckIntegrity(fs::path(entry.path()),dstPath)){
                    // std::cout<<"changed" << dstPath << ((BackupMode::VERSIONED==mode) ? 1 : 0) <<std::endl;
                    if(mode == BackupMode::VERSIONED){
                        //if file changed
                        //create versioned backup
                        //put new changed file
                        std::cout<<"creating version"<<std::endl;
                        CreateVersionBackup(dstPath,entry.path());
                    }

                    else if(mode == BackupMode::DIFFERENTIAL){
                        //if differential backup mode
                        CreateDifferentialBackup(entry.path(),dstPath);
                    }
                }
            }
            // std::cout<<"existing"<<std::endl;
        }
        //if file then compare
        if(fs::is_regular_file(tempdstPath)){
            std::cout<<"existing" << tempdstPath <<std::endl;
        }
    }
    //reset backup status busy flag
    backupStatus=0;
    return 1;
}



//hash evaluation functions


std::tuple<std::vector<unsigned char>, unsigned int> HiveBackup::GenerateHash(std::ifstream& file) {
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file");
    }

    const size_t bufferSize = 4096;
    char buffer[bufferSize];

    EVP_MD_CTX* ctx = EVP_MD_CTX_new();
    if (EVP_DigestInit_ex(ctx, EVP_sha256(), nullptr) != 1) {
        EVP_MD_CTX_free(ctx);
        throw std::runtime_error("Failed to initialize digest for file");
    }

    while (file.read(buffer, bufferSize) || file.gcount() > 0) {
        EVP_DigestUpdate(ctx, buffer, file.gcount());
    }

    std::vector<unsigned char> hash(EVP_MAX_MD_SIZE);
    unsigned int hashLength;

    if (EVP_DigestFinal_ex(ctx, hash.data(), &hashLength) != 1) {
        EVP_MD_CTX_free(ctx);
        throw std::runtime_error("Failed to finalize digest for file");
    }

    EVP_MD_CTX_free(ctx);
    
    return std::make_tuple(hash, hashLength);
}

int HiveBackup::CheckIntegrity(const fs::path& path1, const fs::path& path2) {
    std::ifstream file1(path1, std::ios::binary);
    std::ifstream file2(path2, std::ios::binary);

    if (!file1.is_open() || !file2.is_open()) {
        std::cerr << "Error opening files.\n";
        return 0;  // Error condition
    }

    auto [hash1, hash1Length] = GenerateHash(file1);
    auto [hash2, hash2Length] = GenerateHash(file2);

    // Check if hashes are equal
    if (hash1Length != hash2Length || memcmp(hash1.data(), hash2.data(), hash1Length) != 0) {
        return 0;  // Files are not identical
    }

    return 1;  // Files are identical
}
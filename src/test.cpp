#include <iostream>
#include <filesystem>
#include <vector>
#include <algorithm>

int main(int argc, char** argv){
    std::vector<std::string> a;
    for(int i=0;i<argc;i++)
        a.push_back(argv[i]);
    
    for(auto b:a) std::cout <<b<<std::endl;
    auto it = std::find(a.begin(),a.end(),"hello");

    std::cout<<std::distance(a.begin(),it);

}
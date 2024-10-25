#include "hivesupport.hpp"

int main(){
    std::string a="hello";
    while(1){
        auto data = ReadArgsMQueue(a);
        if(data.status){
            auto a =Deserialize(data.msg);
            for(auto b : a) std::cout<<b<<std::endl;
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}
//hello
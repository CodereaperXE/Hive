#include <iostream>
#include <fstream>
#include <string>
#include <csignal>

int main(){
std::ifstream file("in.txt",std::ios::binary);
std::string data;
char buffer[100];
while(file.read(buffer,sizeof(buffer)) || file.gcount()>0){

    data.append(buffer,file.gcount());

}
std::cout<<data<<std::endl;
file.close();
}

g++ -std=c++20 -o ./build/hivecli ./src/hivecli.cpp ./src/scheduler.cpp ./src/hive.cpp -I include/ -lssl -lcrypto -lpthread
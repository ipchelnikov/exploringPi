#include <sys/types.h>
#include <sys/stat.h>
#include <cstring>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

#include <iostream>
#include <iomanip>
#include <chrono>
#include <ctime>
#include <thread>

int main()
{
    int fd = open("/dev/indicator", O_RDWR);
    if(fd < 0)
    {
        std::cout << "Cannot open device file" << std::endl;
        return fd;
    }

    char string_to_send[10];

    int count;

    while( true )
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        
        auto time_now = std::chrono::system_clock::now();
        std::time_t time_now_t = std::chrono::system_clock::to_time_t(time_now);

        std::tm now_tm = *std::localtime(&time_now_t);
        std::strftime(string_to_send, sizeof(string_to_send), "%H%M", &now_tm);

        int ret = write(fd, string_to_send, strlen(string_to_send));
    }

    return 0;
}

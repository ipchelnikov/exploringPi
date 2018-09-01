#include <sys/types.h>
#include <sys/stat.h>
#include <cstring>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

#include <iostream>
#include <chrono>
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
        ++count;
        //std::cout << std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now()) << std::endl;

        sprintf(string_to_send,"%d",count);

        int ret = write(fd, string_to_send, strlen(string_to_send));
    }

    return 0;
}

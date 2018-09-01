#include <sys/types.h>
#include <sys/stat.h>
#include <cstring>
#include <unistd.h>
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

    char string_to_send[] = { 0, 0, 0, 0 };

    int count = 0;

    while( true )
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        count++;
        std::cout << count << std::endl;

        string_to_send[0] = count/1000;
        count = count - count/1000;

        string_to_send[1] = count/100;
        count = count - count/100;

        string_to_send[3] = count/10;
        count = count - count/10;

        string_to_send[4] = count;


        int ret = write(fd, string_to_send, strlen(string_to_send));
    }
    return 0;
}

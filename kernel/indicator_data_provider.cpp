#include <sys/types.h>
#include <sys/stat.h>
#include <cstring>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <chrono>
#include <ctime>
#include <thread>

const std::string w1_path = "/sys/bus/w1/devices/28-00000a532090/w1_slave";

const int disply_temp = 0;
const int disply_time = 1;

int disply_current;

int main()
{
    std::ifstream w1_stream(w1_path.c_str());

    int fd = open("/dev/indicator", O_RDWR);
    if(fd < 0)
    {
        std::cout << "Cannot open device file" << std::endl;
        return fd;
    }

    char string_to_send[10];

    while( true )
    {
        // TODO! REplace this with ticks in swparate thread
        //
        static int count = 0;
        if(count == 10)
        {
            disply_current = !disply_current;
            count = 0;
        }
        else
        {
            ++count;
        }

        if (disply_current == disply_time)
        {
            string_to_send[0] = 't';

            auto time_now = std::chrono::system_clock::now();
            std::time_t time_now_t = std::chrono::system_clock::to_time_t(time_now);

            std::tm now_tm = *std::localtime(&time_now_t);
            std::strftime(string_to_send+1, sizeof(string_to_send)-1, "%H%M", &now_tm);

        }
        else if(disply_current == disply_temp)
        {
            string_to_send[0] = 'c';
            string_to_send[4] = '\0';
            
            // Magic num 69 is the temerature position in the driver file
            w1_stream.seekg(69, std::ios::beg);
            w1_stream.read(string_to_send+1,3);
        }
        int ret = write(fd, string_to_send, strlen(string_to_send));
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return 0;
}

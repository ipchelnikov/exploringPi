#include <sys/types.h>
#include <sys/stat.h>
#include <cstring>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>


#include <iostream>

int main()
{
    int fd = open("/dev/indicator", O_RDWR);
    if(fd < 0)
    {
        std::cout << "Cannot open device file" << std::endl;
        return fd;
    }

    const char string_to_send[] = { "String to device driver" };

    int ret = write(fd, string_to_send, strlen(string_to_send));

    return 0;
}

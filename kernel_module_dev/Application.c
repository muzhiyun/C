#include <stdio.h>
#include <stdlib.h>
#include <error.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h> 

#define ENCODE 1
#define DECODE 0

int main(void)
{
    int fd = open("/dev/coder",O_RDWR);
    char buf[512] = {0};
    if(fd == -1)
    {
        perror("Wrong");
        return -1;
    }
    printf("Open Succeed!\n");

    read(fd,buf,512);
    printf("read Succeed! %s\n",buf);

    int ret = ioctl(fd,ENCODE);
    write(fd,"555",4);
    printf("ENCODE Write Succeed!\n");
    read(fd,buf,512);
    printf("ENCODE read Succeed! %s\n",buf);


    ret = ioctl(fd,DECODE);
    write(fd,"888",4);
    printf("DECODE Write Succeed!\n");
    read(fd,buf,512);
    printf("DECODE read Succeed! %s\n",buf);

    ioctl(fd,3);
    printf("DECODE Write 3!\n");
    ioctl(fd,5);
    printf("DECODE Write get!\n");
    ioctl(fd,4);
    printf("DECODE Write set!\n");
    ioctl(fd,5);
    printf("DECODE Write get!\n");
    ioctl(fd,6);
    printf("DECODE Write 6!\n");
    ioctl(fd,5);
    printf("DECODE Write get!\n");
    ioctl(fd,7);
    printf("DECODE Write 7!\n");
    ioctl(fd,8);
    printf("DECODE Write 8!\n");

    
}
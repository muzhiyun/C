#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include<string.h>

int main(int argc,const char *argv[])
{
	int fbtoa = 0;
	//int fp = 0;
	int ret = 0;
	char tmpbuff[4096] = {0};
/*	
	if(argc!=2)
        {
                printf("Error!Usage send_file [filename]\n");
                exit(1);
        }

*/
	ret = mkfifo("/home/muzhi/myfifo",0664);
	if(-1 == ret &&EEXIST != errno)
	{
		perror("fail to mkfifo\n");
		exit(1);
	}
	

	fbtoa = open("/home/muzhi/myfifo",O_RDONLY);

	if(-1 == fbtoa)
	{
		perror("fail to open");
		return -1;
	}


	ret = read(fbtoa,tmpbuff,sizeof(tmpbuff));
	
	puts(tmpbuff);

	int fd = open(tmpbuff,O_WRONLY|O_CREAT,0666);
	if( -1 == fd)
	{
		printf("open file error\n");
		return 1;
	}

	while(1)
	{
		ret = read(fbtoa,tmpbuff,sizeof(tmpbuff));
		if(ret <= 0)
		{
			break;
		}
		//printf("%s\n",tmpbuff);
		write(fd,tmpbuff,ret);
		}

	close(fd);
	close(fbtoa);


	return 0;
}

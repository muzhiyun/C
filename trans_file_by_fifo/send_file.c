#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include<string.h>

typedef struct file
{
	int type;
	char data[4096];
	int len;
}fileMSG;

int main(int argc,const char *argv[])
{
	int fp = 0;
	int ret = 0;
	char tmpbuff[4096] = {0};
	int fbtoa = 0;
	fileMSG sendmsg;

	if(argc!=2)
	{
		printf("Error!Usage send_file [filename]\n");
		exit(1);
	}	
	
	fp = open(argv[1],O_RDONLY);
	if(-1 == fp)
	{
		printf("Error!file to open file!\n");
		exit(1);
	}
	

	ret = mkfifo("/home/muzhi/myfifo",0664);
	if(-1 == ret &&EEXIST != errno)
	{
		perror("fail to mkfifo\n");
		exit(1);
	}
	



	
	fbtoa = open("/home/muzhi/myfifo",O_WRONLY);

	if(-1 == fbtoa)
	{	
		perror("fail to open fifo");
		return -1;
	}

	//sendmsg.type = 1;
	//strcpy(sendmsg.data,argv[1]);
	//sendmsg.len = strlen(argv[1]);
	//puts(argv[1]);
	//printf("%d\n",strlen(argv[1]));	
	write(fbtoa,argv[1],strlen(argv[1]));
	sleep(2);

	//exit(0);

	while(1)
	{
		ret = read(fp,tmpbuff,sizeof(tmpbuff));
		if(ret <= 0)
		{
			break;
		}
		write(fbtoa,tmpbuff,ret);
	}
	
	close(fp);
	close(fbtoa);

	return 0;
}

/*
 * =====================================================================================
 *
 *       Filename:  client.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  07/22/2019 05:06:34 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  MuZhi (PuTe), my616422@126.com
 *   Organization:  MuZhi
 *
 * =====================================================================================
 */
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <signal.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>

typedef struct NET_MSG
{
	int type;
	char name[512];
	char data[4095];
	int len;
}MSG;

  
void upload(void* arg,const char* name)
{
	MSG recvMSG;
	MSG sendMSG;
	int filenum = *(int*)arg;
	int ret = -1;
	sendMSG.type = 0;
	strcpy(sendMSG.name,name);
	
	int fd  = open(name,O_RDONLY);	//打开本地文件
	if(-1 == fd)
	{
	//sendMSG.type = -1;
	//strcpy(sendMSG.data,"file ERROR");
	printf(" Failed to open file\n");
	//send(filenum,&sendMSG,sizeof(sendMSG),0);
	close(filenum);
	exit(1);
	}
	
	
	send(filenum,&sendMSG,sizeof(sendMSG),0);		//发送欲上传的文件名
	

	int recv_ret = recv(filenum,&recvMSG,sizeof(recvMSG),0);	//等待服务器返回对方文件创建是否正常
	if(recvMSG.type == -1)		//服务器返回不正常 欲断开连接
	{
		puts(recvMSG.data);	//输出服务端返回的错误信息
		close(filenum);	
		exit(1);
	}

	else if (recvMSG.type == 2)	//服务器返回正常 准备传输文件
	{
		
		while(1)		//循环发送文件
		{
			
			sendMSG.type = 0;
			ret = read(fd,sendMSG.data,4096);
			if(0 == ret)			
				break;	
			sendMSG.len = ret; 
			send(filenum,&sendMSG,sizeof(sendMSG),0);
		}
		sendMSG.type = -1;
        strcpy(sendMSG.data,"finish");
        send(filenum,&sendMSG,sizeof(sendMSG),0);
		close(filenum);
		printf("finish\n");
	}			 
}

void download(void* arg,const char* name)
{
	MSG recvMSG;
	MSG sendMSG;
	int filenum = *(int*)arg;
	sendMSG.type = 1;
	strcpy(sendMSG.name,name);
	
	send(filenum,&sendMSG,sizeof(sendMSG),0);		//发送欲下载的文件名
	

	int recv_ret = recv(filenum,&recvMSG,sizeof(recvMSG),0);	//等待服务器返回对方文件存在是否正常
	if(recvMSG.type == -1)		//服务器返回不正常 欲断开连接
	{
		puts(recvMSG.data);	//输出服务端返回的错误信息
		close(filenum);	
		exit(1);
	}

	else if (recvMSG.type == 2)	//服务器返回正常 准备传输文件
	{
		int fd  = open(name,O_WRONLY|O_CREAT,0666);	//创建文件
		while(1)		//循环接收文件
		{
			recv_ret = recv(filenum,&recvMSG,sizeof(recvMSG),0);
			if(recvMSG.type == -1)		//服务端欲断开 
			{	
				puts(recvMSG.data);	//输出服务端返回的断开信息
				break;
			}
			printf("data:%d |",recvMSG.len);
			write(fd,recvMSG.data,recvMSG.len);
		}
		close(fd);
		close(filenum);	
	}			 
	
}


int main(int argc,const char *argv[])
{
	char ipAddr[32];

	if (argc!=3)
	{
		puts("Usage  ");
		puts("Upload:    ./client u upload.txt");
		puts("Download:  ./client d download.txt");
		puts("");
		exit(1);
	}
	
	//if(argc == 1)
		strcpy(ipAddr,"127.0.0.1");
	//else
	//	strcpy(ipAddr,argv[1]);

	
		
	struct sockaddr_in serAddr,cliAddr;

	serAddr.sin_family = AF_INET;
	serAddr.sin_port = htons(5000);
	serAddr.sin_addr.s_addr = inet_addr(ipAddr);

	int send_ret = socket(AF_INET,SOCK_STREAM,0);
	if (-1 == send_ret)
	{
		perror("socket");
		exit(1);
	}

	int ret = connect(send_ret,(struct sockaddr *)&serAddr,sizeof(serAddr));
	if (-1 == ret)
	{
		perror("connect");
		exit(1);
	}
	
	if(strcmp(argv[1],"d")==0)
		download(&send_ret,argv[2]);
	else if(strcmp(argv[1],"u")==0)
		upload(&send_ret,argv[2]);
	
	
	close(send_ret);
	return 0;
}


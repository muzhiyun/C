/*
 * =====================================================================================
 *
 *       Filename:  server.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  07/22/2019 05:05:56 PM
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


void upload(char* name,void* num)
{
	 MSG sendMSG;
	 MSG recvMSG;
	 char data[4096]={0};
	 int ret = 0;
	 int recv_ret = -1;
	 
     int filenum = *(int *)num;
	 printf("id %d give %s|",filenum,name);
	 
	 int fd = open(name,O_WRONLY|O_CREAT,0666);	//创建文件
	 if(-1 == fd)
	 {
		sendMSG.type = -1;
	 	strcpy(sendMSG.data,"ERROR Failed to create file");
		printf(" Failed to create file\n");
		send(filenum,&sendMSG,sizeof(sendMSG),0);
		close(filenum);
		pthread_exit(NULL);
	 }
	 sendMSG.type = 2;
	 strcpy(sendMSG.data,"create success");
	 send(filenum,&sendMSG,sizeof(sendMSG),0);

	 while(1)
	 {
	  	recv_ret = recv(filenum,&recvMSG,sizeof(recvMSG),0);
		if(recvMSG.type == -1)		//客户端欲断开 
		{	
			puts(recvMSG.data);	//输出客户端返回的断开信息
			break;
		}
		printf("data:%d |",recvMSG.len);
		write(fd,recvMSG.data,recvMSG.len);
	 }
	 close(fd);
	 
	 close(filenum);
     pthread_exit(NULL);
}

void download(char* name,void* num)
{
	 MSG sendMSG;
	 MSG recvMSG;
	 char data[4096]={0};
	 int ret = 0;

         int filenum = *(int *)num;
	 printf("id %d ask %s|",filenum,name);
	 
	 int fd = open(name,O_RDONLY);
	 if(-1 == fd)
	 {
		sendMSG.type = -1;
	 	strcpy(sendMSG.data,"ERROR,Open file fail");
		printf(" But not exist\n");
		send(filenum,&sendMSG,sizeof(sendMSG),0);
		close(filenum);
		pthread_exit(NULL);
	 }
	 sendMSG.type = 2;
	 strcpy(sendMSG.data,"file exist");
	 send(filenum,&sendMSG,sizeof(sendMSG),0);

	 while(1)
	 {
	  	 sendMSG.type = 1;
		 ret = read(fd,sendMSG.data,4096);
		 if(0 == ret)			
	 		break;	
	 	 printf(" length :%d |",ret);	
		 sendMSG.len = ret; 
		 send(filenum,&sendMSG,sizeof(sendMSG),0);
	 }
	 close(fd);
	 sendMSG.type = -1;
         strcpy(sendMSG.data,"finish");
         send(filenum,&sendMSG,sizeof(sendMSG),0);
	 close(filenum);
	 printf("finish\n");
         pthread_exit(NULL);
}

void* handle(void* arg)
{
	//char data[512] = {0};
	MSG recvMSG;
	MSG sendMSG;
	int connect_num = *(int *)arg;
	pthread_detach(pthread_self());
	int recv_ret = recv(connect_num,&recvMSG,sizeof(recvMSG),0);
	if(0 == recv_ret)
	{
		printf("client connect fail");
		close(connect_num);
		pthread_exit(NULL);
	}
	if (recvMSG.type == 0)
		upload(recvMSG.name,&connect_num);
	else if (recvMSG.type == 1)
		download(recvMSG.name,&connect_num);
	else 
	{
		sendMSG.type = -1;
		strcpy(sendMSG.data,"Bad message type");
		send(connect_num,&sendMSG,sizeof(sendMSG),0);
		close(connect_num);
		pthread_exit(NULL);
	}

}

//MSG recvMSG;
//MSG sendMSG;

int main(int argc,const char *argv[])
{
	char ipAddr[32];

	if(argc == 1)
		strcpy(ipAddr,"127.0.0.1");
	else
		strcpy(ipAddr,argv[1]);


	int connum[100]={0};
	int i = 0;
	int len = 0;
	struct sockaddr_in serAddr,cliAddr;

	serAddr.sin_family = AF_INET;
	serAddr.sin_port = htons(5000);
	serAddr.sin_addr.s_addr = inet_addr(ipAddr);

	int sockfd = socket(AF_INET,SOCK_STREAM,0);
	if (-1 == sockfd)
	{
		perror("socket");
		exit(1);
	}

	int ret = bind(sockfd,(struct sockaddr *)&serAddr,sizeof(serAddr));
	if (-1 == ret)
	{
		perror("bind");
		exit(1);
	}
	
	listen(sockfd,3);
	pthread_t tid[100] = {0};
	len =sizeof(cliAddr);

	while(1)
	{
		connum[i] = accept(sockfd,(struct sockaddr *)&cliAddr,&len);
		pthread_create(&tid[i],NULL,handle,&connum[i]);
		i++;
		if(i>100)
			break;
	}
	close(sockfd);
	return 0;
}

/*
 * =====================================================================================
 *
 *       Filename:  taskManager.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/25/2019 07:20:14 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  MuZhi (PuTe), my616422@126.com
 *   Organization:  MuZhi
 *
 * =====================================================================================
 */

#include<stdio.h>
#include<dirent.h>
#include<errno.h>
#include<time.h>
#include<string.h>
#include <unistd.h>
#include <ncurses.h>

typedef struct dirent* (Dirinfo);

int isDigitArray(char *tmpstr)		//判断字符串是否全为数字函数
{
	//puts(str);
	while(*tmpstr)
	{
	if('0'>*tmpstr||'9'<*tmpstr)
		return 0;
	tmpstr++;
	}
	return 1;
}

char *taskCounter(char *str)		//计数函数
{
	DIR* dir=opendir("/proc");
	if(NULL==dir)
	{
		perror("taskCounter->opendir:");
		return NULL;
	}
	int count=0;
	Dirinfo info=NULL;
	while(1)
	{
		info=readdir(dir);
		if(NULL==info)
			break;
		if(isDigitArray(info->d_name))
			count++;
	}
	char tmpstr[20];
	sprintf(tmpstr," 进程数：%d",count);
	strcat(str,tmpstr);
	return str;

}

char* loadAvg(char *str)		//获取系统负载函数
{
	FILE* fp=fopen("/proc/loadavg","r");
	 if(NULL==fp)
	 {
		 perror("loadAvg->fopen:");
		 return NULL;
	 }
	 char tmpstr[20];
	 char arg[20];
	 fgets(arg,20,fp);
	 sprintf(tmpstr," 系统负载：%s",arg);
	 strcat(str,tmpstr);
	 return str;

}

char *getTime(char *str)		//获取当前时间函数
{
	struct tm *nowTime;
	time_t unixTime;

	time(&unixTime);
	nowTime=localtime(&unixTime);

	sprintf(str," %04d-%02d-%02d %02d:%02d:%02d",nowTime->tm_year+1900,nowTime->tm_mon+1,nowTime->tm_mday,nowTime->tm_hour,nowTime->tm_min,nowTime->tm_sec);
	return str;

}

int writeLog(char *str)			//写日志函数
{

	FILE* fp=fopen("./taskManager.log","a");
	if(NULL==fp)
	{
		perror("writeLog->fopen:");
		return -1;
	}
	fputs(str,fp);
	fputc('\n',fp);
	fclose(fp);
	return 1;
}


int top_handle()
{
	char str[60]={""};

   // 使用conio.h頭文件中的kbhit函數可以實現非阻塞式鍵盤監聽。

   // 我們每按下一個鍵，都會將對應的字符放到鍵盤緩沖區，kbhit 函數會檢測緩沖區中是否有字符，
    //如果有字符返回非0值，沒有返回0。但是kbhit不會讀取字符，字符仍然留在緩沖區。
    while(1)
	{
		getTime(str);
		taskCounter(str);
		loadAvg(str);
        //int ret=writeLog(str);
        //if(1==ret)
			puts(str);
        //else
            //puts("Error!");
		sleep(1);
        if(getchar()=='q')
                return 0;
	}
	return 0;
}

/*
 * =====================================================================================
 *
 *       Filename:  02_ls.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/24/2019 10:55:36 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  MuZhi (PuTe), my616422@126.com
 *   Organization:  MuZhi
 *
 * =====================================================================================
 */

#include"ls.h"

typedef struct dirent* (Dirinfo);


void ls(int argc,const char *argv[])
{

	DIR* dir;
	if(argc==2)
		dir = opendir(argv[1]);
	else if(argc==1)
		dir = opendir("./");
	else
	{
        perror("ls.c:too many");
        return ;
	}

	Dirinfo info = NULL;
	if(NULL==dir)
	{
        perror("ls.c->opendir()");
        return ;
	}
	while(1)
	{
		info = readdir(dir);
		if(NULL==info)
			break;
		if('.'==info->d_name[0])
			continue;
		if(DT_REG==info->d_type)						//普通文件
			printf("\033[1;32;40m%s\033[0m    ",info->d_name);
		if(DT_DIR==info->d_type)						//目录文件
			printf("\033[1;34;42m%s\033[0m    ",info->d_name);
		if(DT_CHR==info->d_type)						//字符设备
			printf("\033[1;33;40m%s\033[0m    ",info->d_name);
		if(DT_BLK==info->d_type)						//块设备
			printf("\033[1;34;40m%s\033[0m    ",info->d_name);
		if(DT_LNK==info->d_type)						//软链接
			printf("\033[1;36;40m%s\033[0m    ",info->d_name);
                if(DT_FIFO==info->d_type)						//管道文件
			printf("\033[1;34;40m%s\033[0m    ",info->d_name);
		if(DT_SOCK==info->d_type)						//socket文件
                        printf("\033[1;32;40m%s\033[0m    ",info->d_name);
                if(DT_UNKNOWN==info->d_type)						//未知文件
   			printf("\033[1;34;40m%s\033[0m    ",info->d_name);

	}
	puts("");
	closedir(dir);
}

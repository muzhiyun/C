#include"ll.h"

int judgeFileOrDir(const char *filenametmp)			//判断文件类型函数
{
	struct stat buftmp;
	#if DEBUG
	printf("ll.c->judgeFileOrDir:传参:%s\n",filenametmp);
	#endif
	int rettmp = lstat(filenametmp,&buftmp);
	if(-1 == rettmp)
	{
        perror("ll->judgeFileOrDir:");
        return -1;
	}
	mode_t tmp = buftmp.st_mode;
	if(S_ISREG(tmp))        //普通
	{
		fputc('-',stdout);
        return 1;
	}
	else if(S_ISDIR(tmp))	//目录
	{
		fputc('d',stdout);
        return 2;
	}
	else if(S_ISCHR(tmp))	//字符
		fputc('c',stdout);
	else if(S_ISBLK(tmp))	//块
    	fputc('b',stdout);
	else if(S_ISFIFO(tmp))	//管道
		fputc('p',stdout);
	else if(S_ISLNK(tmp))	//软连接
    	fputc('l',stdout);
	else if(S_ISSOCK(tmp))	//socket
		fputc('s',stdout);
	else 					//未知
    	fputc('u',stdout);
	return 0;
	

}

void judgePermiss(mode_t tmp,int flag)		//判断权限函数
{
	int S1,S2,S3;
	switch(flag)
	{
		case 1:				//用户
			S1=S_IRUSR;
			S2=S_IWUSR;
			S3=S_IXUSR;
			break;
		case 2:				//组
			S1=S_IRGRP;
			S2=S_IWGRP;
			S3=S_IXGRP;
			break;
		case 3:				//OTHER
			S1=S_IROTH;
			S2=S_IWOTH;
			S3=S_IXOTH;
			break;
	}
	
	(tmp & S1)?fputc('r',stdout):fputc('-',stdout);		//读权限
	(tmp & S2)?fputc('w',stdout):fputc('-',stdout);		//写权限
	(tmp & S3)?fputc('x',stdout):fputc('-',stdout);		//运行权限
}

void fileHandle(const char *filename)						//文件处理函数
{
	struct stat buf;
	struct passwd *pw;		//用户信息结构体
	struct group *group;	//组信息结构体
	struct tm *p;			//时间结构体
	
	//char filename[]="/etc/passwd";
	#if DEBUG
	printf("ll.c->fileHandle入参检测 文件名:%s\n",filename);
	#endif
	int ret = lstat(filename,&buf);
	if(-1 == ret)
	{
		fprintf(stderr,"ll.c->fileHandle:stat file errorr\n");
		return ;
	}
	judgeFileOrDir(filename);	//判断文件类型
	
	judgePermiss(buf.st_mode,1);	//依次判断文件权限 
	judgePermiss(buf.st_mode,2);	//也可写为循环三次 就可以不用传flag参数 只需调用一次 传buf.st_mode即可
	judgePermiss(buf.st_mode,3);
	
	pw = getpwuid(buf.st_uid);
	group = getgrgid (buf.st_gid);	//用户id转用户名	组id转组名
	
	//Unix时间戳转标准时间  gmtime方式
	// p=gmtime(&buf.st_mtime);  		
    // char time[100];  
    // strftime(time, sizeof(time), "%Y-%m-%d %H:%M:%S", p);  
	
	//Unix时间戳转标准时间  ctime方式
	char time[100]={"\0"};  
	strncat(time,ctime(&buf.st_mtime),strlen(ctime(&buf.st_mtime))-1);
	//puts(time);
	//puts(ctime(&buf.st_mtime));
	//printf("buf:%lu t:%lu\n",buf.st_mtime,t);
	//printf("%s",ctime(&buf.st_mtime));
	
	//Unix时间戳转标准时间  localtime方式
	//p= localtime(&buf.st_mtime);
	//printf(" %d-%d-%d %d:%d:%d ",p->tm_year+1900,p->tm_mon+1,p->tm_mday
	//		,p->tm_hour,p->tm_min,p->tm_sec);
	
	//ctime示例代码
	// time_t t;
	// time(&t);
	//printf("%s",ctime(&t));
	
	
	
	
	//printf(" %d %d %d %ld %lu %s\n",buf.st_nlink,buf.st_uid,buf.st_gid,buf.st_size,buf.st_mtime,filename);
	
	printf(" %lu %s %s %6ld %s ",buf.st_nlink,pw->pw_name,group->gr_name,buf.st_size,time);
	
	//printf(" %lu %s %s %ld ",buf.st_nlink,pw->pw_name,group->gr_name,buf.st_size);
			
    if(judgeFileOrDir(filename)==1)						//普通文件 绿色
        printf("\b\033[1;32;40m%s\033[0m\n",filename);
	
    else if(judgeFileOrDir(filename)==2)					//目录文件 蓝色打印
	{
        printf("\b\b\033[1;34;40m%s/\033[0m \n",filename);		//'\b' 用于消除判断目录时产生的输出  \n前空格用于消除诡异的d
	}		
	else
		printf("\b\b%s\n",filename);								//其他文件 正常打印
}


typedef struct dirent* (Dirinfo);

//2 1
//2 0
int ll(int argc, const char *argv[])
{
	
	
	if(argc>1)					//判断有参数传递
	{
        int num=argc-1;
		//printf("ll入参检测:有参 num初值:%d ",num);
		//printf("ll传参检测1%s\n",argv[num]);
        while(num)							//循环处理所有传递的参数
		{
			//printf("ll传参检测:%s\n",argv[num]);
			//printf("ll.c->judgeFileOrDir:返回值:%d\n",judgeFileOrDir(argv[num]));
            if(2==judgeFileOrDir(argv[num]))			//有参数传递 且传递的参数为目录
			{
				printf("\b");										//用来删除判断目录产生的打印输出 
				DIR* dir;
				dir = opendir(argv[num]);							//打开目录
				
				if(NULL==dir)
				{
					puts("OPENDIR ERROR!");
					return -1;
				}
				
				char pathBack[512];
				getcwd(pathBack,512);							//备份当前目录路径
					
				chdir(argv[num]);								//跳到目标目录 以便后续调用fileHandle函数时 可以只给一个文件名 
																//否则就要传路径或出现not found问题									
				Dirinfo info = NULL;
					
				while(1)										//获取目录下文件名 循环处理目录下文件
				{
					info = readdir(dir);
					if(NULL==info)
						break;
					fileHandle(info->d_name);					//依次将文件名传入文件处理函数
				}
				chdir(pathBack);								//跳回原目录
			}
            else if (-1!=judgeFileOrDir(argv[num]))								//有参数传递 且传递的参数为文件
				{
					fileHandle(argv[num]);
				}
            else {
                {
                    perror("ll->judgeFileOrDir return -1: ");
                }
            }
			num--;
			puts("------------------------------------------------");
		}
	}
	else if(argc==1)		//未传递参数 则打印当前目录下所有文件
	{
		DIR* dir;
		dir = opendir("./");					//打开当前目录
		if(NULL==dir)
		{
			puts("OPENDIR ERROR!");
			return -1;
		}
		Dirinfo info = NULL;
		while(1)										//获取当前目录下文件名 循环处理目录下文件
		{
			info = readdir(dir);
			if(NULL==info)
				break;
			fileHandle(info->d_name);					//依次将文件名传入文件处理函数
		}
	}
	

}

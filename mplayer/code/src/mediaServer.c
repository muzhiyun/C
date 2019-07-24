#include"mediaServer.h"
#include "public.h"

//char fileName[30][100];

int loadMediaFile(char *dirName)
{
	int i=0;
	DIR *dp=NULL;
	//struct stat buf;
	struct dirent* dirp;
	int ret =0;
	//puts(dirName);
	dp=opendir(dirName);
	while((dirp=readdir(dp))!=NULL)
	{
		if(dirp->d_name[0]!='.')
			strcpy(fileName[i++],dirp->d_name);
	}
	closedir(dp);
	mediaFileListLen=sizeof(fileName)/sizeof(fileName[0]);
	return 0;	
	
}

void changeMediaFilePath()	//允许用户自定义媒体文件路径
{
	printf("\n当前默认读取路径为%s下的媒体文件\n",mediaFilePath);
	printf("如需修改请在下方输入完整的绝对路径并回车保存\n");
	printf("\n\n软件启动时默认读取当前用户名下Videos文件夹，路径错误会导致程序异常退出，请检查路径！\n");
	int i;
	for(i=0;i<mediaFileListLen;i++)		//清空之前的文件列表
	{
		strcpy(fileName[i],"");
	}
	scanf("%s",mediaFilePath);
}

	
/* Mplayer进程Pid */
pid_t pid;

/* 播放媒体文件 */
int PlayMedia(char *FilePath)
{
	char FifoPath[256] = {0};

	pid = fork();
	if (-1 == pid)
	{
		//LogWrite(fLog, ERROR, "fail to play music!");
		return -1;
	}
	if (0 == pid)
	{
		close(1);
		close(2);
		int ret=0;
		ret=mkfifo(MPLAYER_SLAVE_FIFO_PATH, 0664);
		if (-1 == ret && EEXIST != errno)
		{		
			perror("fail to mkfifo");		
			return -1;	
		}
		sprintf(FifoPath, "file=%s", MPLAYER_SLAVE_FIFO_PATH);
		puts(FifoPath);
		puts(FilePath);
		execlp("mplayer", "mplayer", "-slave", "-input", FifoPath, FilePath, "-quiet", NULL);
		exit(0);
	}
	gMplayerStat = 1;

	return 0;
}

	


#include"public.h"
#include"menu.h"
#include"mediaServer.h"

char fileName[50][100];
int mediaFileListLen;
int gMplayerStat=0;
char mediaFilePath[30];

void getMediaFilePath()			//根据用户名 拼接出默认媒体路径
{
	char filePath[30]={"/home/"};
	char userName[10]={""};
	//strcpy(userName,system("whoami"));  //废了
	struct passwd *pwd;
	pwd = getpwuid(getuid());		//获取当前用户名
	strcpy(userName,pwd->pw_name);
	strcat(filePath,userName);
	strcat(filePath,"/Videos");
	strcat(mediaFilePath,filePath);
}

int main(int argv,const char *argc[])
{
	getMediaFilePath();
	loadMediaFile(mediaFilePath);
	//printfMediaFileList();
	//printfFuncList();
	startMplayer();
return 0;
}

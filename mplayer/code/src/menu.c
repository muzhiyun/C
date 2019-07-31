#include"menu.h"
#include"mediaServer.h"
#include "public.h"
#include "doub_list_for_fileList.h"

char funcList[10][30]={
		"查看媒体文件",
		"开始/暂停",
		"修改默认读取路径",
		"停止",
		"上一首",
		"下一首",
		"快进",
		"定位",
		"播放方式",
		"退出"
		};
void reflash();
int checkKey();
void exitMplayer();

int funcListLen=sizeof(funcList)/sizeof(funcList[0]);

struct termios old,new;

int funcListFocus=0;
int mediaFileListFocus=0;

int menuLevel=1;

int fileNameStart=0;
int fileNameEnd=2;

int funcListStart=0;
int funcListEnd=10;

void printfFuncList()
{
	system("clear");
	printf("+---------------------------------+\n");
	printf("|          Mplayer播放            |\n");
	printf("+---------------------------------+\n");
	int temp;
	for(temp=funcListStart;temp<funcListEnd;temp++)
	{
		if(funcListFocus==temp)
			printf("|\033[30;43m%-2d.%s\033[0m\n",temp+1,funcList[temp]);
		else
			printf("|%2d.%s\n",temp+1,funcList[temp]);
	}
	 printf("+---------------------------------+\n");
	 printf("\n  上/下键选择 回车键进入 q键退出 \n");
}


void printfMediaFileList()
{
	system("clear");
	printf("+---------------------------------+\n");
	printf("|          文件列表               |\n");
	printf("+---------------------------------+\n");
	 int i;                                             
        for(i=fileNameStart;i<fileNameEnd;i++) 
        {       
		if(mediaFileListFocus==i)
			printf("|\033[30;43m%2d.%s\033[0m\n",i+1,fileName[i]);
		else		
                	printf("|%2d.%s\n",i+1,(fileName[i]));                          
        }
	printf("+---------------------------------+\n");
	printf("\n  上/下键选择 回车键进入 q键退出 \n");
}

void printfMediaFileListList(LinkList *list)
{
	system("clear");
	printf("+---------------------------------+\n");
	printf("|          文件列表               |\n");
	printf("+---------------------------------+\n");
	 int i=0;                            
	 LinkNode *temp = list->head;   

	 while(temp->data.id==fileNameStart-1)			//使list的起始位置和Start保持同步
	 {
		 	temp=temp->next;
	 }

        for(i=fileNameStart;i<fileNameEnd;i++) 
        {       
			if(mediaFileListFocus==i)
				{
					printf("|\033[30;43m%2d.%s\033[0m\n",i+1,temp->data.path);		//判断预打印这个是不是焦点
					temp =temp->next; 
				}
			else
				{
					printf("|%2d.%s\n",i+1,temp->data.path);
					temp =temp->next; 
				}
			  
		
		}
	printf("+---------------------------------+\n");
	// temp = list->head;   
	// printf("%d:%s\n",temp->data.id,temp->data.path);
	// temp =temp->next; 
	// printf("%d:%s\n",temp->data.id,temp->data.path);
	// temp =temp->next; 
	// printf("%d:%s\n",temp->data.id,temp->data.path);
	// temp =temp->next; 
	// printf("%d:%s\n",temp->data.id,temp->data.path);
	//showList(list,0);
	printf("\n  上/下键选择 回车键进入 q键退出 \n");
}

/* 屏蔽回显*/ 
void term()
{	
	tcgetattr(0,&old);	
	tcgetattr(0,&new);	
	new.c_lflag &= ~(ECHO|ICANON);	
	new.c_cc[VTIME] = 0 ;	
	new.c_cc[VMIN]= 1;	
	tcsetattr(0,TCSANOW,&new);
}



/* 
 * ===  FUNCTION  ======================================================================
 *  Name:  startMplayer
 *  Description:  启动初始化
 * =====================================================================================
 */
void startMplayer (LinkList *list)
{
	fileNameEnd=mediaFileListLen-1;
	term();				//关闭回显
	printfFuncList();			//打印一级菜单
	//checkKey();		//检测按键
	while(1)
	{
	reflash(checkKey());
	if(menuLevel==1)
		printfFuncList();
	if(menuLevel==2)
		//printfMediaFileList();  //普通数组方式
		printfMediaFileListList(list);  //链表方式
	}
}		
/* -----  end of function startMplayer  ----- */

void reflash(int num)
{
	if (num==2)//下键
	{
		if (funcListFocus<funcListEnd-1&&menuLevel==1)			//未超默认显示范围 一级
			funcListFocus+=1;
		else if(mediaFileListFocus<fileNameEnd-1&&menuLevel==2)		//未超默认显示范围 二级
			mediaFileListFocus+=1;
		else if(funcListFocus==funcListEnd-1&&funcListEnd<funcListLen&&menuLevel==1)	//超默认显示范围 但未超出全部范围 一级
		{
			funcListStart+=1;
			funcListEnd+=1;
			funcListFocus+=1;
		}
		else if (mediaFileListFocus==fileNameEnd-1&&fileNameEnd<mediaFileListLen&&menuLevel==2)
		{
			fileNameEnd+=1;
			fileNameStart+=1;
			mediaFileListFocus+=1;
		}

	}
	 if (num==3)//上键
	 {
		 if (funcListFocus>funcListStart&&menuLevel==1) 
 		 	funcListFocus-=1;
		 else if(mediaFileListFocus>fileNameStart&&menuLevel==2)
			mediaFileListFocus-=1;
		 else if(funcListFocus==funcListStart&&funcListStart>0&&menuLevel==1) //超默认显示范围 但未超出全部范围 一级 
		 {

		 	funcListStart-=1;           
		 	funcListEnd-=1;													                         
		 	funcListFocus-=1;
																                 
		 }
		 else if (mediaFileListFocus==fileNameStart&&fileNameStart>0&&menuLevel==2)
                 {	                   
			 fileNameEnd-=1;
			 fileNameStart-=1;	                         
			mediaFileListFocus-=1;
          	}					 
	 }

	 if (num==1)	//回车键
	 {
		 if(menuLevel==1)			//一级界面按下回车键
		 {
		 	switch (funcListFocus){
				case 0:					//进入播放列表功能
					menuLevel=2;
					return ;
					break;
				case 2:					//修改默认路径功能
					tcsetattr(0,TCSANOW,&old);     		//打开回显 方便用户输入 并减少程序崩溃带来影响
					changeMediaFilePath();			//修改媒体路径
					loadMediaFile(mediaFilePath);		//重新读取文件
					puts("修改成功");
					getchar();				//回收scanf自带回车
					term();					//重新关闭回显
					break;

				case 9:					//退出功能
					printf("GoodBay!!!\n");
					exitMplayer();
					break;
			}
		 }
	 	if (menuLevel==2)		//二级界面按下回车键
		{
			char fullPath[50];		//拼接完整路径 准备播放
			strcpy(fullPath,mediaFilePath);
			strcat(fullPath,"/");
			strcat(fullPath,fileName[mediaFileListFocus]);
			puts("在二级界面按下回车");
			PlayMedia(fullPath);
		}
	 }

	 if (num==0)	//q键
	 {
	 	if(menuLevel==1)	//如果位于1级菜单 就退出
		{
			 printf("GoodBay!!!\n");
			 exitMplayer();
		}
		else			//位于二级菜单 就返回一级菜单
		{
			menuLevel=1;
		}
	 }
		 

}

/*退出函数*/
void exitMplayer()
{
	 tcsetattr(0,TCSANOW,&old);	//打开回显 restore_key;
        if (0!= gMplayerStat)	
	{
		kill(pid, SIGKILL);	
	}
	 exit(0);
}


/* 检测按键按下*/ 
int checkKey()
{	
	while(1)	
	{		
		char c = getchar();		
		switch(c)		
		{			
			case '\n':				
				//printf("按了回车\n");	
				return 1;			
				break;			
			case 66:				
				//printf("按了下键\n");		
				return 2;		
				break;			
			//case 27:				
			//	printf("按了ESC键\n");				
			//	break;			
			case 65:				
				//printf("按了上键\n");
				return 3;				
				break;			
			case 'q':				
				//printf("GoodBay!!!\n");				
				//exitMplayer();	
				return 0;			
				break;		
		}	
	}
}

/* \n 1		66 2 	65 3	27'q' 0 */

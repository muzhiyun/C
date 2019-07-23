#include<stdio.h>
#include<stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <time.h>

#define N 4

void handle();
int addNum();
void exitGame();
void printfLogo();
int checkFull();

int array[4][4]={0};  //chai fen
struct termios old,new;  //chai fen

/* 随机产生返回2/4 */
int forRand(void)
{
	if(rand()%2)
		return 2;
	else
		return 4;
}


/*  打印2048游戏界面  */
void printf2048() //show_box
{
	system("clear");
	printfLogo();
	int i =0, j=0 ;
	for(j = 0 ;j< 4;++j)
	{
		printf("                        \033[34;44m ||----------------||\033[0m\n                        ");
		for(i=0;i<4;++i)
		{
			printf("\033[34;44m \033[0m");
			if(0 == array[j][i])
			{
				printf("    ");
			}
			else
			{
				printf("%4d",array[j][i]);
			}
		}
		printf("\033[34;44m \033[0m\n");
	}
	printf("                        \033[34;44m ||----------------||\033[0m\n");
}

/*屏蔽回显*/
void term()
{
	tcgetattr(0,&old);
	tcgetattr(0,&new);
	new.c_lflag &= ~(ECHO|ICANON);
	new.c_cc[VTIME] = 0 ;
	new.c_cc[VMIN]= 1;

	tcsetattr(0,TCSANOW,&new);

}

/*检测按键按下*/
void checkKey()
{
	while(1)
	{
		char c = getchar();
		switch(c)
		{
			case 'a':
				handle(1);
				break;
			case 's':
				handle(4);
				break;
			case 'd':
				handle(2);
				break;
			case 'w':
				handle(3);
				break;
			case 'q':
				printf("GoodBay!!!\n");
				exitGame();
				break;
		}


	}
}


/*       head->end       flag    方向    */
/*  ++   0       3        1      左      */ 
/*  --   3       0        2      右      */
/*  ++   0       3        3      上      */
/*  --   3       0        4      下      */


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  wightmoveArray
 *  Description:  水平移动合并函数
 * =====================================================================================
 */
void wightmoveArray(int head,int end,int flag)		/*	head->end 	flag	方向	*/
{							/* ++ 	0	3	 1	左	*/
	int i=head,j=head,k;				/* --	3	0	 2	右	*/
	while(-1<j&&j<4)	//三重循环 移动部分	/* ++	0	3	 3      上	*/		
	{						/* --	3	0	 4	下	 */
		i=head;
		while(-1<i&&i<4)	
		{
			(flag%2==0)?(k=i-1):(k=i+1);
			//printf("\n\n\n\n\n\nkkkkk:%d\n",k);
			while(-1<k&&k<4)			//移到顶端
			{
				if(0==array[j][i]&&array[j][k]!=0)
				{
					array[j][i]=array[j][k];
					array[j][k]=0;
				}
				(flag%2)?k++:k--;
			}
			(flag%2)?i++:i--;
		}
		(flag%2)?j++:j--;
	}
	//printf("head:%d end:%d i:%d j:%d k:%d\n",head,end,i,j,k);
	i=head;
	j=head;
	 while(-1<j&&j<4)	//合并部分
	 { 
		 i=head;	
		 while(-1<i&&i<4)
		 {
			  (flag%2==0)?(k=i-1):(k=i+1);
			if(array[j][i] != 0 && array[j][i] == array[j][k])						    
			 {
				 array[j][i]*=2;
				 array[j][k]=0;
			 }
		 	(flag%2)?i++:i--;
		 }
		 (flag%2)?j++:j--;
	 }
	/*	
	int i,j;
	for (j = 0; j < 4; j++)				//移动
	{
		for (i = 0; i < 4-1; i++)
		{			
			if (0 == array[j][i] && array[j][i+1] != 0)			
			{				
				array[j][i] = array[j][i+1];		
				array[j][i+1] = 0;
			}		
		}	
	}
	for (j=0;j < 4;j++)
	{	
		for (i = 0; i < 4-1; i++)					//合并
		{		
			if (array[j][i] != 0 && array[j][i] == array[j][i+1])		
			{			
				array[j][i] *= 2;			
				array[j][i+1] = 0;			
			}	
		}
	}	
	*/
	/*
	for (j = 0; j < N-1; j++)	
	{		
		for (i = 0; i < 4; i++)		
		{
			if (0 == temp[i] && temp[i+1] != 0)			
			{				
				temp[i] = temp[i+1];				
				temp[i+1] = 0;			
			}		
		}	
	}
	*/	
}		
/* -----  end of function wightmoveArray ----- */


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  hightmoveArray
 *  Description:  垂直移动合并函数
 * =====================================================================================
 */
void hightmoveArray(int head,int end,int flag)		/*	head->end 	flag	方向	*/
{							/* ++ 	0	3	 1	左	*/
	int i=head,j=head,k;				/* --	3	0	 2	右	*/
	while(-1<j&&j<4)	//三重循环 移动部分	/* ++	0	3	 3      上	*/		
	{						/* --	3	0	 4	下	 */
		i=head;
		while(-1<i&&i<4)	
		{
			(flag%2==0)?(k=i-1):(k=i+1);
			//printf("\n\n\n\n\n\nkkkkk:%d\n",k);
			while(-1<k&&k<4)			//移到顶端
			{
				if(0==array[i][j]&&array[k][j]!=0)
				{
					array[i][j]=array[k][j];
					array[k][j]=0;
				}
				(flag%2)?k++:k--;
			}
			(flag%2)?i++:i--;
		}
		(flag%2)?j++:j--;
	}
	//printf("head:%d end:%d i:%d j:%d k:%d\n",head,end,i,j,k);
	i=head;
	j=head;
	 while(-1<j&&j<4)	//合并部分
	 { 
		 i=head;	
		 while(-1<i&&i<4)
		 {
			  (flag%2==0)?(k=i-1):(k=i+1);
			if(array[i][j] != 0 && array[i][j] == array[k][j])						    
			 {
				 array[i][j]*=2;
				 array[k][j]=0;
			 }
		 	(flag%2)?i++:i--;
		 }
		 (flag%2)?j++:j--;
	 }
}		
/* -----  end of function hightmoveArray ----- */

/* 
 * ===  FUNCTION  ======================================================================
 *  Name:  left
 *  Description:  按键调用反馈函数
 * =====================================================================================
 */
void handle(int num)
{
	switch(num)
	{
		case 1:
			wightmoveArray(0,3,1);
			break;
		case 2:
			wightmoveArray(3,0,2);
			break;
		case 3:
			hightmoveArray(0,3,3);
			break;
		case 4:
			hightmoveArray(3,0,4);
			break;
	}
	if((addNum(forRand())==0)&&(checkFull()==1))//增加数字失败 且 验满为真
	{
		 printf("You Are Fail!\n");
		 exitGame();
	}
	checkFull();
	printf2048();

}		
/* -----  end of function left  ----- */

/*        head->end       flag    方向    */
/*   ++   0       3        1      左      */
/*   --   3       0        2      右      */
/*   ++   0       3        3      上      */
/*   --   3       0        4      下      */ 


/* 
 * ===  FUNCTION  ======================================================================
 *  Name:  checkFull
 *  Description:  检查是否填满或有2048
 * =====================================================================================
 */
int checkFull()			//检测到2048 直接祝贺退出
{				//不满 返回0   否则满返1
	int i,j;
	for (i=0;i<4;i++)
	{
		for(j=0;j<4;j++)
		{	
			if(array[i][j]==1024)

			{
				printf2048();
				printf("Congratulations You Won!\n");
				exitGame();
			}
			if(array[i][j]==0)
			{
				return 0;
			}

		}
	}
	//printf("验满函数\n");
	return 1;
}		
/* -----  end of function checkFull  ----- */


/* 将传入的数插入数组随机位置 */
int addNum(int num)
{
	int x,y,ret=0;//4 *4 ;
	
	int rand_count = 0 ;
	while(rand_count<50)
	{
		x = rand()%4;
		y = rand()% 4;
	//	printf("%d %d %d \n",x,y,array[y][x]);
		if(0 == array[y][x])
		{
			break;
		}
		rand_count++;
		//int ret = check_win_lose();
	//	if(50 ==  rand_count)
	//	{
	//		break;
	//	}

	}
	//printf("%d %d %d \n",x,y,array[y][x]);
	if(array[y][x]!=0)
	{
		//printf("插入成功\n");
		return 0;		//尝试50 仍然插入失败 怀疑可能为满
	}
	else
	{
		array[y][x]=num;	
		return 1;		//插入成功 返回1
	}
}

/* 
 * ===  FUNCTION  ======================================================================
 *  Name:  startGame
 *  Description:  启动初始化
 * =====================================================================================
 */
void startGame ()
{
	term();				//关闭回显
	addNum(forRand());		//产生随机2/4并放入随机位置
	printf2048();			//打印界面
}		
/* -----  end of function startGame  ----- */


/*退出函数*/
void exitGame()
{
	 tcsetattr(0,TCSANOW,&old);//restore_key;
         exit(0);
}



int main()
{
	startGame();
	while(1)
	{
	checkKey();
	}
	return 0;
}












void printfLogo()
{
	printf("             ;;;;;;          ;;;;;;               ;;;;       ;;;;;;;                   \n");
	printf("           ;;;;;;;;;       ;;;;;;;;;;            ;;;;;      ;;;;;;;;;;                 \n");
	printf("          ;;;;;;;;;;;      ;;;;;;;;;;           ;;;;;;     ;;;;;;;;;;;;                \n");
	printf("          ;;;;   ;;;;     ;;;;;  ;;;;;          ;;;;;;    ;;;;     ;;;;                \n");
	printf("           ;;     ;;;;    ;;;;    ;;;;         ;;;;;;;    ;;;;      ;;;                \n");
	printf("                  ;;;;   ;;;;      ;;;;       ;;;;;;;;    ;;;;     ;;;;                \n");
	printf("                  ;;;;   ;;;;      ;;;;      ;;;; ;;;;     ;;;;   ;;;;;                \n");
	printf("                 ;;;;    ;;;;      ;;;;      ;;;; ;;;;      ;;;;;;;;;;                 \n");
	printf("                 ;;;;    ;;;;      ;;;;     ;;;;  ;;;;       ;;;;;;;                   \n");
	printf("                ;;;;     ;;;;      ;;;;    ;;;;   ;;;;      ;;;;;;;;;;                 \n");
	printf("               ;;;;      ;;;;      ;;;;    ;;;    ;;;;     ;;;;;  ;;;;;                \n");
	printf("              ;;;;       ;;;;      ;;;;   ;;;;    ;;;;    ;;;;     ;;;;                \n");
	printf("             ;;;;        ;;;;      ;;;;  ;;;;;;;;;;;;;;;  ;;;;      ;;;;               \n");
	printf("           ;;;;;         ;;;;      ;;;;  ;;;;;;;;;;;;;;;  ;;;;      ;;;;               \n");
	printf("          ;;;;;           ;;;;    ;;;;   ;;;;;;;;;;;;;;;  ;;;;      ;;;;               \n");
	printf("         ;;;;;;;;;;;;;    ;;;;;;;;;;;;   ;;;;;;;;;;;;;;;  ;;;;;   ;;;;;                \n");
	printf("         ;;;;;;;;;;;;;     ;;;;;;;;;;             ;;;;     ;;;;;;;;;;;;                \n");
	printf("         ;;;;;;;;;;;;;      ;;;;;;;;              ;;;;      ;;;;;;;;;;                 \n");
	printf("                              ;;;;                             ;;;;                    \n");
}

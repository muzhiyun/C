#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <stdlib.h>
#define NUM 4
int array[NUM][NUM];
struct termios old,new;
void show_box()
{
	system("clear");
	printf("这个是2048小游戏\n");
	printf("w:上 s:下 a:左 d:右 q:退出\n");
	int i =0, j=0 ;
	for(j = 0 ;j< NUM; ++j)
	{
		printf("\033[34;44m ||--------------------------||\033[0m\n");
		for(i=0; i<NUM; ++i)
		{
			printf("\033[34;44m ||\033[0m");
			if(0 == array[j][i])
			{
				printf("    ");
			}
			else
			{
				printf("%4d",array[j][i]);
			}
		}
		printf("\033[34;44m ||\033[0m\n");
	}
	printf("\033[34;44m ||--------------------------||\033[0m\n");

}

void init_game()
{
	tcgetattr(0,&old);
	tcgetattr(0,&new);
	new.c_lflag &= ~(ECHO|ICANON);
	new.c_cc[VTIME] = 0 ;
	new.c_cc[VMIN]= 1;
	tcsetattr(0,TCSANOW,&new);
	srand(time(NULL));

}
void quit_game()
{


	//回复 回显 进入规范输入模式
	tcsetattr(0,TCSANOW,&old);
	exit(0);
}
int check_win_lose()//0 win 1 lose 2 continue;
{
	int i,j;
	int is_full_flag = 1;
	for(j =0 ;j<NUM ;++j)
	{
		for(i = 0 ;i<NUM; ++i)
		{
			if(2048 == array[j][i])
			{
			
				printf("游戏胜利");
				quit_game();
				return 0;
			}
			if( 0 ==  array[j][i])
			{
			
				is_full_flag = 0 ;
			}

		}
	
	
	}
	if(0 == is_full_flag)
	{
	
		return 2;
	}
	else
	{
	
	printf("遗憾，游戏失败了\n");
	quit_game();
	return 1;
	}

}
void gen_num()
{
	int x,y,num;//4 *4 ;
	
	int rand_count = 0 ;
	while(1)
	{
		x = rand() %4;
		y = rand()% 4;
		if(0 == array[y][x])
		{
			break;
		}
	//	rand_count++;
		int ret = check_win_lose();
		if(2 ==  ret)
		{
			continue;
		}

	}

	
	num =  rand()% 2 ;
	if(0 == num)
	{
		num = 2;
	}
	else
	{
		num = 4;
	}

	array[y][x] =num;
}

void key_left()
{

}

void key_down()
{

}
void key_right()
{

}
void key_up()
{

}
void get_key()
{
	char c =  getchar();
	switch(c)
	{
	case 'a':
		key_left();
		break;
	case 's':
		key_down();
		break;
	case 'd':
		key_right();
		break;
	case 'w':
		key_up();
		break;
	case 'q':
		quit_game();	
		break;
	default:
		break;

	
	}

}
int main(int argc, const char *argv[])
{
	init_game();
	while(1)
	{
		gen_num();
		show_box();
		get_key();
	}
	return 0;
}

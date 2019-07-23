
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, const char *argv[])
{
	struct termios old,new;
	tcgetattr(0,&old);
	tcgetattr(0,&new);
	new.c_lflag &= ~(ECHO|ICANON);
	new.c_cc[VTIME] = 0 ;
	new.c_cc[VMIN]= 1;

	tcsetattr(0,TCSANOW,&new);
	
	while(1)
	{
		char c = getchar();
		switch(c)
		{
			case 'a':
				printf("left key press\n");
				break;
			case 's':
				printf("down key press\n");
				break;
			case 'd':
				printf("right key press\n");
				break;
			case 'w':
				printf("up key press\n");
				break;
			case 'q':
				printf("quit key press\n");
				tcsetattr(0,TCSANOW,&old);//restore_key;
				exit(0);
				break;
		}


	}






	return 0;
}

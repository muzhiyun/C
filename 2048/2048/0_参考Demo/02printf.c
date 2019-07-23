#include <stdio.h>

int array[4][4];
int main(int argc, const char *argv[])
{
	int i =0, j=0 ;
	for(j = 0 ;j< 4;++j)
	{
		printf("\033[34;44m ||----------------||\033[0m\n");
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
	printf("\033[34;44m ||----------------||\033[0m\n");
	return 0;
}

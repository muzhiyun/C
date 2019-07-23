#include <stdio.h>
#include <unistd.h>
int move_handle(int argc, const char *argv[])
{
    if(argc<3||argc>3)
    {
        printf("Too few variables,ln need at least two variables\n");
        return -1;
    }
    int ret = rename(argv[1],argv[2]);
	if( -1 == ret)
	{
		printf("rename errorr\n");
		return 1;
	}
	
	return 0;
}

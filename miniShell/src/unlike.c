#include <stdio.h>
#include <unistd.h>
int unlink_handle(int argc, const char *argv[])
{
    int ret = unlink(argv[1]);
	if( -1 == ret)
	{
		printf("unlink errorr\n");
		return 1;
	}
	
	return 0;
}

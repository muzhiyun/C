#include"rm.h"

void rm_handle(int argc, const char *argv[])
{
    while(argc-1)
    {
        int ret = remove(argv[argc-1]);
        if( -1 == ret)
        {
            printf("rm %s ->",argv[argc-1]);
            fflush(stdout);
            perror("remove:");
            return ;
        }
        argc--;

    }
    return ;
}

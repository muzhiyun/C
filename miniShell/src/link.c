#include "ln.h"


int ln_handle(int argc, const char *argv[])
{
    if(argc<3)
    {
        printf("Too few variables,ln need at least two variables\n");
        return -1;
    }
    if(strcmp(argv[1],"-s")==0)             //ln -s
    {
        if(argc>4||argc<4)
        {
            printf("Too many or too few variables,ln -s need two variables\n");
            return -1;
        }
        int ret = symlink(argv[2],argv[3]);
        if( -1 == ret)
        {
            printf("link errorr\n");
            return 1;
        }

        return 0;


    }
    if(argc>3)                               //ln
    {
        printf("Too many variables,ln need only two variables\n");
        return -1;
    }
    int ret = link(argv[1],argv[2]);
    if( -1 == ret)
    {
        printf("symlink errorr\n");
        return 1;
    }

    return 0;
}


#include"touch.h"
int touch_handle(int argc, const char *argv[])
{
    FILE *fp = NULL;

    if (argc < 2)
    {
        printf("Too few variables,ln need at least two variables\n");
        return -1;
    }

    fp = fopen(argv[1], "a");

    fclose(fp);

    return 0;
}

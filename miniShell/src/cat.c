#include"cat.h"

int cat_handle(int argc, const char *argv[])
{
	// ./a.out 1.c
	if(argc<2)
	{
        printf("useage: cat filename\n");
		return 0;
	}
	FILE* fp = fopen(argv[1],"r");
	if(NULL == fp)
	{
        perror("cat->fopen:");
		return 1;
	}
	while(1)
	{
		char data[30]={0};
		char * ret = fgets(data,30,fp);
		if( NULL == ret) 
			break;
        printf("%s",data);
	}
	fclose(fp);

	return 0;
}

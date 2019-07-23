/*
 * =====================================================================================
 *
 *       Filename:  01_fread.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/22/2019 02:34:21 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  MuZhi (PuTe), my616422@126.com
 *   Organization:  MuZhi
 *
 * =====================================================================================
 */

#include<stdio.h>
#include<stdlib.h>

void cp_handle(int argc,const char *argv[])
{
    if(argc<3)
	{
		puts("ERROR!Please Input 3 Variable");
        return ;
	}
    //puts(argv[1]);
	FILE* fp=fopen(argv[1],"r");
	if(NULL==fp)
	{
        perror("cp->fopen:");
        return ;
	}
	fseek(fp,0,SEEK_END);
    long len=ftell(fp);
	fseek(fp,0,SEEK_SET);
	FILE* fp2=fopen(argv[2],"w");
	char *p = malloc(len);
	fread(p,len,1,fp);
	fwrite(p,len,1,fp2);
	fclose(fp);
	fclose(fp2);
	free(p);
	//while(1)
	//{
		
	//}
    return ;
}

/*
 * =====================================================================================
 *
 *       Filename:  04_mkdir.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/24/2019 11:26:38 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  MuZhi (PuTe), my616422@126.com
 *   Organization:  MuZhi
 *
 * =====================================================================================
 */
#include"mkdir.h"

void mkdir_handle(int argc,const char *argv[])
{
    int ret=mkdir(argv[1],0777);
    if(-1==ret)
    {
        perror("mkdir.c->mkdir:");
    }

    //printf("succuse");

}


void rmdir_handle(int argc,const char *argv[])
{
    int ret =rmdir(argv[1]);
    if(-1==ret)
    {
        perror("mkdir.c->rmdir:");
    }
}

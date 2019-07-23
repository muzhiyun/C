#ifndef MKDIR_H
#define MKDIR_H

#include<stdio.h>
#include<sys/types.h>
#include<dirent.h>
#include <fcntl.h>           /* Definition of AT_* constants */
#include <sys/stat.h>
#include <unistd.h>

void mkdir_handle(int argc,const char *argv[]);
void rmdir_handle(int argc,const char *argv[]);

#endif // MKDIR_H

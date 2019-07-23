#ifndef CD_H
#define CD_H

#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <pwd.h>		//获得用户id转用户名支持

extern void cd(int argc, const char *argv[]);
extern char *pwd(char *path);

#endif // CD_H

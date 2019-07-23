#ifndef _LL_H_
#define _LL_H_

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <pwd.h>		//获得用户id转用户名支持
#include <grp.h>		//获得组id转组名支持
#include <time.h>  		//获得Unix时间戳转标准时间支持


extern int ll(int argc, const char *argv[]);

#endif
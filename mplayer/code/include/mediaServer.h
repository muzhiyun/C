#ifndef _MEDIASERVER_H_
#define _MEDIASERVER_H_
#include <sys/types.h>
#include<dirent.h>
#include "doub_list_for_fileList.h"
//extern char fileName[30][100];
//
extern pid_t pid;

extern int loadMediaFile(char *dirName);
extern int loadMediaFiletoList(char *dirName,LinkList *list);
extern void changeMediaFilePath();
extern int PlayMedia(char *FilePath);
#endif

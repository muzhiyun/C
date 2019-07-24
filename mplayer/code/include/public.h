#ifndef _PUBLIC_H_
#define _PUBLIC_H_
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pwd.h>
#include <signal.h>

#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>
#include <errno.h>

#define MPLAYER_SLAVE_FIFO_PATH "/tmp/mplayer_fifo"
extern int gMplayerStat;
extern char fileName[50][100];
extern int mediaFileListLen;
extern char mediaFilePath[30];
#endif

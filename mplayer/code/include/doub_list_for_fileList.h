#ifndef _DOUB_LIST_FOR_FILELIST_H_
#define _DOUB_LIST_FOR_FILELIST_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct fileList
{
    int id;
    char name[64];
    char path[512];

}DATATYPE;

typedef struct node {
    DATATYPE data;
    struct node *next;
    struct node *prev;
}LinkNode;

typedef struct list {
    LinkNode *head;
    int clen;
    LinkNode* curr;
}LinkList;


extern LinkList *createList();
int insertTailNode(LinkList *list,DATATYPE *data);
int showList(LinkList *list,int type);

#endif
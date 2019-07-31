#include "doub_list_for_fileList.h"

LinkList *createList()
{
    LinkList* temp = malloc(sizeof(LinkList));
    if(NULL == temp)
    {
        perror("createList");
        exit(1);
    }
    temp->clen = 0;
    temp->curr = NULL;
    temp->head = NULL;

    return temp;
}

int insertHeadNode(LinkList *list,DATATYPE *data)
{
    LinkNode *new_node = malloc(sizeof(LinkNode));
    if(NULL == new_node)
    {
        perror("insertHeadNode malloc");
        exit(1);
    }
    memcpy(&new_node->data,data,sizeof(DATATYPE));  //需写为DATATYPE data只是指针 sizeof(data)只有4个字节
    new_node->next = NULL;
    new_node->prev = NULL;
    if(list->head == NULL)
    {
        list->head=new_node;
    }
    else
    {
        new_node->next = list->head;
        list->head->prev = new_node;    //这是在干嘛？
        list->head = new_node;
    }
    list->clen++;
    return 0;
    
}

//打印函数 type为0时打印文件完整路径 type为1时仅打印文件名
int showList(LinkList *list,int type)
{
    LinkNode* temp = list->head;
   
        while(temp)
        {
             if(0 == type)
            {
                printf("%d:%s\n",temp->data.id,temp->data.path);
            }
            else
            {
                printf("%d:%s\n",temp->data.id,temp->data.name);
            }
            temp = temp->next;
            
        }
    return 0;

}

//事实上，这个项目只需要一种插法即可，头插尾插都行，头插更简单，尾插更符合习惯
int insertTailNode(LinkList *list,DATATYPE *data)
{
    LinkNode *new_node = malloc(sizeof(LinkNode));
    if(NULL == new_node)
    {
        perror("insertTailNode malloc");
        exit(1);
    }
    memcpy(&new_node->data,data,sizeof(DATATYPE));
    new_node->next = NULL;
    new_node->prev = NULL;
    if(list->head == NULL)
    {
        list->head=new_node;
    }
    else
    {
        LinkNode *temp = list->head;
        while(temp->next)
        {
           temp = temp->next;
        }
        new_node->prev = temp;
        temp->next = new_node;
    }
    list->clen++;
    return 0;
    
}

//事实上 这个项目不需要链表查找 为了熟练使用 就也写出来了
LinkNode *findList(LinkList *list,char *name)
{
    LinkNode *temp = list->head;
    while(temp->next)
    {
        if(0 == strcmp(temp->data.name,name))
        {
            return temp;
        }
        temp = temp->next;    
    }
    return NULL;
}

//摧毁函数
void destroyList(LinkList *list)
{
    LinkNode *temp = list->head;
    while(temp->next)
    {
        temp = temp->next;
    }                               //移到结尾了 然后呢？free怎么写才对？
}
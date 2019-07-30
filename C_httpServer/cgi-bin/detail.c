#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
#include <unistd.h>
#include "urldeconde.h"

char id[20]="";



int errMsg(const char *str)
{
    
    printf("服务器内部错误\n出错信息："); 
    puts(str);
    char buff[1024];
    getcwd(buff,sizeof(buff));
    puts("出错路径");
    puts(buff);
    exit(1);
}


int callback(void* arg,int col,char** result ,char** title)
{

  if (col == 0)
  {
     return 0;  
  }
  
  //goods_id,goods_name,shop_price,goods_number,keywords,original_img,goods_desc

    printf("id是%s </br>商品名 %s</br> 价格：%s</br> 库存：%s</br> 关键词：%s</br> 原图：<img src=\"../%s\"></br> 描述：%s</br>",result[0],result[1],result[2],result[3],result[4],result[5],result[6]);
    *(int*)arg = 1;
    return 0;
}
                                         
int  main()                                                   
{ 
    char buf[1024]="";	
    char MSG[1024] = {0};

    strcpy(buf,getenv("QUERY_STRING"));
    sscanf(buf, "id=%s",id);

    printf("HTTP/1.1 200 OK\r\n"); 
    printf("Content-type: text/html;charset=utf-8\r\n");
    printf("Content-Length:null\r\nTransfer-Encoding:null\r\nConnection:close\r\n\r\n");
    printf("<html>\n<head>\n<title>CGI Output title</title>\n</head>\n<body> <h4>CGI-OUTPUT</h4><hr></br>");        

 

    

    sqlite3* db;
    int ret = sqlite3_open("./db/123.db",&db);
    if(ret != SQLITE_OK)
    {
         //fprintf(stderr,"open db failure...%s\n",sqlite3_errmsg(db));
        errMsg(sqlite3_errmsg(db));
    }
    char *err;
    int exist_flag =0;
    char sql_cmd[512]={0};
    sprintf(sql_cmd,"select goods_id,goods_name,shop_price,goods_number,keywords,original_img,goods_desc from goods where goods_id = \"%s\";",id);
    //puts(sql_cmd);
    ret = sqlite3_exec(db,sql_cmd,callback,&exist_flag,&err);
    if(SQLITE_OK != ret)
    {
        //fprintf(stderr,"exec sql:%s err:%s\n",sql_cmd,err);
        strcpy(MSG,err);
        errMsg(MSG);
        sqlite3_free(err);
        sqlite3_close(db);
    }
 
    
                                     
    sqlite3_close(db);
    return 0;                                                    
}

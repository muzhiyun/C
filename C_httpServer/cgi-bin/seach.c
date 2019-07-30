#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
#include <unistd.h>
#include "urldeconde.h"

char key[20]="";



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
  
    printf("<a href=\"/cgi-bin/detail?id=%s\"><img src=\"../%s\"></a>商品名 %s ",result[0],result[2],result[1]);
    *(int*)arg = 1;
    return 0;
}
                                         
int  main()                                                   
{ 
    char buf[1024]="";	
    char keytmp[20]={0};
    char MSG[1024] = {0};

    strcpy(buf,getenv("QUERY_STRING"));
    sscanf(buf, "key=%s",keytmp);

    printf("HTTP/1.1 200 OK\r\n"); 
    printf("Content-type: text/html;charset=utf-8\r\n");
    printf("Content-Length:null\r\nTransfer-Encoding:null\r\nConnection:close\r\n\r\n");
    printf("<html>\n<head>\n<title>CGI Output title</title>\n</head>\n<body> <h4>CGI-OUTPUT</h4><hr></br>");        

 

    
    urlDecode(key,keytmp);
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
    sprintf(sql_cmd,"select goods_id,goods_name,goods_thumb from goods where goods_name like \"%%%s%%\";",key);
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
    if(exist_flag == 0)
    {
        printf("<h4>未找到！</h4>");
    }
    
                                     
    sqlite3_close(db);
    return 0;                                                    
}

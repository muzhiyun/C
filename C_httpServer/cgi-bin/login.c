#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
#include <unistd.h>

char user[20]="";
char passwd[20]="";


int errMsg(const char *str)
{
    printf("HTTP/1.0 500 Internal Server Error\r\n"); 
    printf("Content-type: text/html;charset=utf-8\r\n");
    printf("Content-Length:null\r\nTransfer-Encoding:null\r\nConnection:close\r\n\r\n");
    printf("<html>\n<head>\n<title>Error</title>\n</head>\n<body>"); 
    printf("500<hr>服务器内部错误"); 
    puts(str);
    char buff[1024];
    getcwd(buff,sizeof(buff));
    puts(buff);
    exit(1);
}


int callback(void* arg,int col,char** result ,char** title)
{

  if (col == 0)
  {
     return 0;  
  }
  
 if(strcmp(passwd,result[0])==0)
        *(int*)arg = 1;
    return 0;
}
                                         
int  main()                                                   
{ 
    char buf[1024]="";	

    char MSG[1024] = {0};

    strcpy(buf,getenv("QUERY_STRING"));
    sscanf(buf, "Username=%[^&]&Password=%s", user,passwd);

   
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
    sprintf(sql_cmd,"select Password from user_account where Username=\"%s\";",user);
    ret = sqlite3_exec(db,sql_cmd,callback,&exist_flag,&err);
    if(SQLITE_OK != ret)
    {
        //fprintf(stderr,"exec sql:%s err:%s\n",sql_cmd,err);
        strcpy(MSG,err);
        errMsg(MSG);
        sqlite3_free(err);
        sqlite3_close(db);
    }

     printf("HTTP/1.1 200 OK\r\n"); 
    printf("Content-type: text/html;charset=utf-8\r\n");
    printf("Content-Length:null\r\nTransfer-Encoding:null\r\nConnection:close\r\n\r\n");
    printf("<html>\n<head>\n<title>CGI Output title</title>\n</head>\n<body> <h4>CGI-OUTPUT</h4><hr></br>");        

                                     

    if(0 == exist_flag)
    {
        printf("登录失败，请检查账号或密码......\n");
        printf("<script type=\"text/javascript\"> setTimeout(\"window.location.href='../login.html'\",2000);</script>");
    }
    else
    {
        printf("登录成功，请稍等，正在为您跳转......\n");
        printf("<script type=\"text/javascript\"> setTimeout(\"window.location.href='../search.html'\",2000);</script>");

    }
    sqlite3_close(db);
    return 0;                                                    
}

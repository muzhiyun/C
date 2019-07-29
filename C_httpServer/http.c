#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<error.h>
#include<pthread.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<netinet/ip.h>
#include<arpa/inet.h>
#include<fcntl.h>
#include <sys/wait.h>

#include"urldeconde.h"

#define bool int
#define true 1
#define false 0

#define TCPMAXLEN 1024

struct DATA
{
    char name[20];
    char value[128];
};

struct HTTP
{
    char method[20];    //请求方法 GET/POST
    char url[1024];     //请求url
    char type[20];      //请求的文件类型
    char parameter[1024];   //请求参数
    struct DATA data;       //请求参数解析后的结果 name-value方式保存
    bool IsCGI;             //本次请求是否是CGI
};

struct MSG
{
    char statinfo[64];  //返回状态信息
    char ContentTypeinfo[64];   //返回文件类型信息
    unsigned long  Length;    //返回长度
    char Lengthinfo[64];   //返回长度信息
    char msg[512];             //返回信息 出错时用
};




int socket_init()
{

    int sockfd = socket(AF_INET,SOCK_STREAM,0);
    if(-1 == sockfd)
    {
        perror("socket");
        exit(1);
    }

    struct sockaddr_in ser;
    ser.sin_family = AF_INET;
    ser.sin_port = htons(80);
    ser.sin_addr.s_addr = INADDR_ANY;

    int ret = bind(sockfd,(struct sockaddr *)&ser,sizeof(ser));
    if(-1 == ret)
    {
        perror("bind");
        exit(1);
    }
    listen(sockfd,100);
    return sockfd;

}


//出错发送出错信息
int send_msg(int sockfd,struct MSG *msg)
{
    printf("stat  %s",(*msg).statinfo);
    printf("type  %s",(*msg).ContentTypeinfo);
    printf("len   %s",(*msg).Lengthinfo);
    send(sockfd,(*msg).statinfo,strlen((*msg).statinfo),0);
    send(sockfd,(*msg).ContentTypeinfo,strlen((*msg).ContentTypeinfo),0); 
    send(sockfd,(*msg).Lengthinfo,strlen((*msg).Lengthinfo),0);
    send(sockfd,(*msg).msg,strlen((*msg).msg),0);
    pthread_exit(NULL);
}

//将请求的method分割出来
int getmethod(char* buf,struct HTTP *head)
{
    char buftmp[1024]={0};
    strcpy(buftmp,buf);
    char *p = NULL;
    p = buftmp;
   while(*p != ' ')
   {
       p++;
   }
   *p = '\0';
   strcpy((*(head)).method,buftmp);

}

//将请求的url和data分割出来
int geturl_getdata(int sockfd,char* buf,struct HTTP *head,struct MSG *msg)
{
    char buftmp[TCPMAXLEN]={0};
    strcpy(buftmp,buf);
    if(0 == strcmp((*(head)).method,"GET"))
    {
        char *p = NULL;
        p = strstr(buftmp," ");
        strcpy(buftmp,p+1);

        p = buftmp;
        
        while(*p != ' ')
        {
            p++;
        }
        *p = '\0';

        if(strstr(buftmp,"?"))      //GET方式 有data
        {
            p = buftmp;
            while(*p != '?')
            {
                p++;
            }
            *p = '\0';
            strcpy((*(head)).parameter,p+1);
            if(0 == strcmp(buftmp,"/"))
                strcpy(buftmp,"/index.html");
            strcpy((*(head)).url,buftmp);
        }
        else                        //GET方式 无data
        {
           if(0 == strcmp(buftmp,"/"))
                strcpy(buftmp,"/index.html");
           strcpy((*(head)).url,buftmp);  
        }

    }
   /*  else if(0 == strcmp((*(head)).method,"POST"))
    {
        char *p = NULL;
        p = strstr(buftmp," ");
        strcpy(buftmp,p+1);

        p = buftmp;
        
        while(*p != ' ')
        {
            p++;
        }
        *p = '\0';
         if(0 == strcmp(buftmp,"/"))
                strcpy(buftmp,"/index.html");
        strcpy((*(head)).url,buftmp+1);

        p = strstr(buf,"\r\n\r\n");
    }
    */

    else
    {
        char temp[512]={0};
        strcpy((*msg).msg,"<p>错误：请求方式暂未实现</p>");
        strcpy((*msg).statinfo,"HTTP/1.0 500 Internal Server Error\r\n");
        strcpy((*msg).ContentTypeinfo,"Content-type: text/html\r\n");
        (*msg).Length = strlen((*msg).msg);
        sprintf((*msg).Lengthinfo,"Content-Length: %lu\r\n\r\n",(*msg).Length);
        puts("错误：请求方式暂未实现");
        send_msg(sockfd,msg);
        
    }
        
}

int cgiError(int sockfd,struct MSG *msg)
{
        char temp[512]={0};
        strcpy((*msg).msg,"<p>错误：CGI执行出错</p>");
        strcpy((*msg).statinfo,"HTTP/1.0 500 Internal Server Error\r\n");
        strcpy((*msg).ContentTypeinfo,"Content-type: text/html\r\n");
        (*msg).Length = strlen((*msg).msg);
        sprintf((*msg).Lengthinfo,"Content-Length: %lu\r\n\r\n",(*msg).Length);
        puts("错误：CGI执行出错");
        send_msg(sockfd,msg);
}

//检测文件是否存在函数
int Isfileexist(struct HTTP *head)
{
    char fullPath[512]=".";
    strcat(fullPath,(*head).url);
    return access(fullPath,0)+1;  //access文件存在返回0 不存在返回-1   改为存在返回1  不存在返回0
}

//计算文件长度赋值msg结构体函数
int fileLength(struct HTTP *head,struct MSG *msg)
{
    struct stat st;
    char fullPath[512]=".";
    strcat(fullPath,(*head).url);
    int ret = stat(fullPath,&st);
    if(-1 ==ret)
    {
        perror("stat");
        exit(1);
    }
    (*msg).Length = st.st_size;
    sprintf((*msg).Lengthinfo,"Content-Length: %lu\r\n\r\n",st.st_size);
}

//获取文件格式并赋值msg结构体
int fileType(struct HTTP *head,struct MSG *msg)
{
    char temp[64] = {0};
    strcpy(temp,(*head).url);
    char *p = NULL;
    p = temp+strlen(temp);
    while(*p != '.')
    {
        p--;
    }
    strcpy(temp,p+1);
    if (strcmp(temp,"html")==0)
    {
        strcpy((*msg).ContentTypeinfo,"Content-Type: text/html;charset=utf-8\r\n");
    }
    else if(strcmp(temp,"jpg")==0)
    {
        strcpy((*msg).ContentTypeinfo,"Content-Type: image/jpeg;charset=utf-8\r\n");
    }
     else if(strcmp(temp,"png")==0)
    {
        strcpy((*msg).ContentTypeinfo,"Content-Type: image/png;charset=utf-8\r\n");
    }
     else if(strcmp(temp,"js")==0)
    {
        strcpy((*msg).ContentTypeinfo,"application/javascript;charset=utf-8\r\n");
    }
     else if(strcmp(temp,"css")==0)
    {
        strcpy((*msg).ContentTypeinfo,"Content-Type: text/css;charset=utf-8\r\n");
    }
    else 
    strcpy((*msg).ContentTypeinfo,"Content-Type: file;charset=utf-8\r\n");
    
}

//发送response头部分
int send_head(int sockfd,struct MSG *msg,struct HTTP *head)
{
    char *cmd[6]={NULL};
    cmd[0]=(*msg).statinfo;
    cmd[1]="Connection: Keep-Alive\r\n";
    cmd[2]=(*msg).ContentTypeinfo;
    cmd[3]="Date: Sat, 27 Jul 2019 07:55:07 GMT\r\n";
    cmd[4]="Server: Muzhi webserver/1.1\r\n";
    cmd[5]=(*msg).Lengthinfo;

    int i = 0 ;
    for(i = 0 ;i<6;i++)
    {
        send(sockfd,cmd[i],strlen(cmd[i]),0);
    }

    return 0;
}





//发送response主体部分
int send_data(int sockfd,struct MSG *msg,struct HTTP *head)
{
    char fullPath[512]=".";
    strcat(fullPath,(*head).url);
    int fd = open(fullPath,O_RDONLY);
    if(-1 == fd)
    {
        perror("open");
        exit(1);
    }
    char buf[1024]={0};
    while(1)
    {
        int ret  = read(fd,buf,1024);
        if(0 == ret)
            break;
        send(sockfd,buf,ret,0);

    }
    return 0;
}

int cgihandle2(int sockfd,struct MSG *msg,struct HTTP *head)
{
    char fullPath[512]=".";
    strcat(fullPath,(*head).url);
    char *buf[] = {"ps", "ajx", NULL}; 
    execv(fullPath,buf);
}

int get_line(int sock, char *buf, int size)
{
 int i = 0;
 char c = '\0';
 int n;

 while ((i < size - 1) && (c != '\n'))
 {
  //recv()包含于<sys/socket.h>,参读《TLPI》P1259, 
  //读一个字节的数据存放在 c 中
  n = recv(sock, &c, 1, 0);
  /* DEBUG printf("%02X\n", c); */
  if (n > 0)
  {
   if (c == '\r')
   {
    //
    n = recv(sock, &c, 1, MSG_PEEK);
    /* DEBUG printf("%02X\n", c); */
    if ((n > 0) && (c == '\n'))
     recv(sock, &c, 1, 0);
    else
     c = '\n';
   }
   buf[i] = c;
   i++;
  }
  else
   c = '\n';
 }
 buf[i] = '\0';

 return(i);
}


void cgihandle(int client,struct MSG *msg,struct HTTP *head)
{

 char method[64];
 strcpy(method,((*head).method));

 char path[64];
 char fullPath[512]=".";
 strcat(fullPath,(*head).url);

 strcpy(path,fullPath);

 char query_string[64];
 strcpy(query_string,((*head).parameter));

 char buf[1024];
 int cgi_output[2];
 int cgi_input[2];
 pid_t pid;
 int status;
 int i;
 char c;
 int numchars = 1;
 int content_length = -1;
 
 //往 buf 中填东西以保证能进入下面的 while
 buf[0] = 'A'; buf[1] = '\0';
 //如果是 http 请求是 GET 方法的话读取并忽略请求剩下的内容
 if (strcasecmp(method, "GET") == 0)
  while ((numchars > 0) && strcmp("\n", buf))  /* read & discard headers */
   numchars = get_line(client, buf, sizeof(buf));
 else    /* POST */
 {
  //只有 POST 方法才继续读内容
  numchars = get_line(client, buf, sizeof(buf));
  //这个循环的目的是读出指示 body 长度大小的参数，并记录 body 的长度大小。其余的 header 里面的参数一律忽略
  //注意这里只读完 header 的内容，body 的内容没有读
  while ((numchars > 0) && strcmp("\n", buf))
  {
   buf[15] = '\0';
   if (strcasecmp(buf, "Content-Length:") == 0)
    content_length = atoi(&(buf[16])); //记录 body 的长度大小
   numchars = get_line(client, buf, sizeof(buf));
  }
  
  //如果 http 请求的 header 没有指示 body 长度大小的参数，则报错返回
  if (content_length == -1) {
  char temp[512]={0};
    strcpy((*msg).msg,"<p>错误：请求方式暂未实现</p>");
    strcpy((*msg).statinfo,"HTTP/1.0 500 Internal Server Error\r\n");
    strcpy((*msg).ContentTypeinfo,"Content-type: text/html\r\n");
    (*msg).Length = strlen((*msg).msg);
    sprintf((*msg).Lengthinfo,"Content-Length: %lu\r\n\r\n",(*msg).Length);
    puts("错误：请求方式暂未实现");
    send_msg(client,msg);
   return;
  }
 }

 sprintf(buf, "HTTP/1.0 200 OK\r\n");
 send(client, buf, strlen(buf), 0);

 //下面这里创建两个管道，用于两个进程间通信
 if (pipe(cgi_output) < 0) {
  cgiError(client,msg);
  return;
 }
 if (pipe(cgi_input) < 0) {
  cgiError(client,msg);
  return;
 }

 //创建一个子进程
 if ( (pid = fork()) < 0 ) {
 cgiError(client,msg);
  return;
 }
 
 //子进程用来执行 cgi 脚本
 if (pid == 0)  /* child: CGI script */
 {
  char meth_env[255];
  char query_env[255];
  char length_env[255];

  //dup2()包含<unistd.h>中，参读《TLPI》P97
  //将子进程的输出由标准输出重定向到 cgi_ouput 的管道写端上
  dup2(cgi_output[1], 1);
  //将子进程的输出由标准输入重定向到 cgi_ouput 的管道读端上
  dup2(cgi_input[0], 0);
  //关闭 cgi_ouput 管道的读端与cgi_input 管道的写端
  close(cgi_output[0]);
  close(cgi_input[1]);
  
  //构造一个环境变量
  sprintf(meth_env, "REQUEST_METHOD=%s", method);
  //putenv()包含于<stdlib.h>中，参读《TLPI》P128
  //将这个环境变量加进子进程的运行环境中
  putenv(meth_env);
  
  //根据http 请求的不同方法，构造并存储不同的环境变量
  if (strcasecmp(method, "GET") == 0) {
      puts("构建环境变量");
   sprintf(query_env, "QUERY_STRING=%s", query_string);
   putenv(query_env);
  }
  else {   /* POST */
   sprintf(length_env, "CONTENT_LENGTH=%d", content_length);
   putenv(length_env);
  }
  
  //execl()包含于<unistd.h>中，参读《TLPI》P567
  //最后将子进程替换成另一个进程并执行 cgi 脚本
  execl("/mnt/e/嵌入式开发/普特/20190728/5_源码/cgi-bin/login", "login", NULL);
  exit(0);
  
 } else {    /* parent */
  //父进程则关闭了 cgi_output管道的写端和 cgi_input 管道的读端
  close(cgi_output[1]);
  close(cgi_input[0]);
  /* 
  //如果是 POST 方法的话就继续读 body 的内容，并写到 cgi_input 管道里让子进程去读
  if (strcasecmp(method, "POST") == 0)
   for (i = 0; i < content_length; i++) {
    recv(client, &c, 1, 0);
    write(cgi_input[1], &c, 1);
   }
   */
  //然后从 cgi_output 管道中读子进程的输出，并发送到客户端去
  while (read(cgi_output[0], &c, 1) > 0)
   send(client, &c, 1, 0);

  //关闭管道
  close(cgi_output[0]);
  close(cgi_input[1]);
  //等待子进程的退出
  waitpid(pid, &status, 0);
 }
}


void *handle(void *arg)
{
    pthread_detach(pthread_self()); //设置分离属性
    
    while(1)
    {
        struct HTTP head;
        struct MSG msg;


        memset(&head,0,sizeof(head));
        memset(&msg,0,sizeof(msg));
        //struct DATA data;
        char buf[TCPMAXLEN]={0};
        int sockfd = *(int *)arg;
        int ret = recv(sockfd,buf,1024,0);
        if(ret <= 0)
        {
            printf("客户端id %d 已断开连接\n",sockfd);
            close(sockfd);
            break;
            //pthread_exit(NULL);
        }
        else
        {
           
            getmethod(buf,(struct HTTP *)&head);
            geturl_getdata(sockfd,buf,(struct HTTP *)&head,(struct MSG *)&msg);
            //puts(buf);
            printf("method : %s\n",head.method);
            printf("url : %s\n",head.url);
            printf("data : %s\n",head.parameter);
            puts("----------------------------------------------");

            if(strstr(head.url,".")==NULL)
            {
                puts("请求了CGI");

                // char temp[512]={0};
                // strcpy(msg.msg,"<p>错误：请求方式暂未实现</p>");
                // strcpy(msg.statinfo,"HTTP/1.0 500 Internal Server Error\r\n");
                // strcpy(msg.ContentTypeinfo,"Content-type: text/html;charset=utf-8\r\n");
                // msg.Length = strlen(msg.msg);
                // sprintf(msg.Lengthinfo,"Content-Length: %lu\r\n\r\n",msg.Length);
                // puts("错误：请求方式暂未实现");
                // send_msg(sockfd,(struct MSG *)&msg);
                cgihandle(sockfd,(struct MSG *)&msg,(struct HTTP *)&head);
                puts("CGI完成");

            }
            else                                //非CGI 常规静态文件
            {
                if(Isfileexist((struct HTTP *)&head) == true)   // 判断文件是否存在 
                {
                    puts("存在");
                    strcpy(msg.statinfo,"HTTP/1.1 200 OK\r\n"); //填充状态码
                    

                }
                else            //不存在返回404页面
                {
                    puts("不存在");  
                    strcpy(msg.statinfo,"HTTP/1.1 404 Not Found\r\n"); 
                    strcpy(head.url,"/404.html");   // 改为发送404.html
                    
                }
                fileLength((struct HTTP *)&head,(struct MSG *)&msg);    //填充长度
                fileType((struct HTTP *)&head,(struct MSG *)&msg);         //填充文件类型

                puts("----------------------------------------------");
                printf("stat  %s",msg.statinfo);
                printf("type  %s",msg.ContentTypeinfo);
                printf("len   %s",msg.Lengthinfo);

                send_head(sockfd,(struct MSG *)&msg,(struct HTTP *)&head);      //发送头部
                send_data(sockfd,(struct MSG *)&msg,(struct HTTP *)&head);      //发送主体


                
                //printf("%s",buf);
            }
        }

    }
    pthread_exit(NULL);
}

int main()
{
    pthread_t tid[100];
    int sockfd = socket_init();
    int conn[100]={0};
    int i = 0;
    while(1)
    {
        conn[i] = accept(sockfd,NULL,NULL);
        pthread_create(&tid[i++],NULL,handle,&conn[i]);
        if(i>100)
            break;

    }
    close(sockfd);
    return 0;
}
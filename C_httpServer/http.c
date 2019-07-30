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
            p = buftmp;
            if(*(p+strlen(buftmp)-1) == '/')
                strcat(buftmp,"index.html");
            strcpy((*(head)).url,buftmp);
        }
        else                        //GET方式 无data
        {
            p = buftmp;
           if(*(p+strlen(buftmp)-1) == '/')
                strcat(buftmp,"index.html");
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
    strcat(fullPath,(*head).url);   //2019-01-30更新 增加文件夹 文件判断
                                    //access文件存在返回0 不存在返回-1  
                                    //但无法判断是文件还是文件夹 当存在文件夹folder
                                    //访问http://url/folder 会误导性以为访问名为folder的文件 

    if(access(fullPath,0) == 0)     //该项目存在 再判断是文件还是文件夹
    {
        puts("判断 存在");
        struct stat buftmp;
        int rettmp = lstat(fullPath,&buftmp);
        mode_t tmp = buftmp.st_mode;
        if(!S_ISDIR(tmp))	        //非目录 则返回1
        {
            puts("不是文件夹");
            return 1;
        }
    }
    return 0;  //不存在 或存在但为文件夹 则返回0
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
        strcpy((*msg).ContentTypeinfo,"Content-Type: application/javascript;charset=utf-8\r\n");
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





void cgihandle(int client,struct MSG *msg,struct HTTP *head)
{

 char method[64];
 strcpy(method,((*head).method));

 char path[64];
 char fullPath[512]={0};
 getcwd(fullPath,sizeof(fullPath));
  //strcat(fullPath,"/cgi-bin");
 strcat(fullPath,(*head).url);
//puts(fullPath);
 strcpy(path,fullPath);
 puts(path);
 puts(((*head).url)+1);



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
 
 

 //sprintf(buf, "HTTP/1.0 200 OK\r\n");
 //send(client, buf, strlen(buf), 0);

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
  //dup2(cgi_input[0], 0);
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
     
   sprintf(query_env, "QUERY_STRING=%s", query_string);
  // puts(query_env);
   putenv(query_env);
  }
  else {   /* POST */
   sprintf(length_env, "CONTENT_LENGTH=%d", content_length);
   putenv(length_env);
  }
  
  //execl()包含于<unistd.h>中，参读《TLPI》P567
  //最后将子进程替换成另一个进程并执行 cgi 脚本
  
  execl(path, ((*head).url)+1, NULL);
  exit(0);
  
 } else {    /* parent */
  //父进程则关闭了 cgi_output管道的写端和 cgi_input 管道的读端
  close(cgi_output[1]);
  close(cgi_input[0]);
  
  //如果是 POST 方法的话就继续读 body 的内容，并写到 cgi_input 管道里让子进程去读
  if (strcasecmp(method, "POST") == 0)
   for (i = 0; i < content_length; i++) {
    recv(client, &c, 1, 0);
    write(cgi_input[1], &c, 1);
   }
   
  //然后从 cgi_output 管道中读子进程的输出，并发送到客户端去
  while (read(cgi_output[0], &c, 1) > 0)
   send(client, &c, 1, 0);

  //关闭管道
  close(cgi_output[0]);
  close(cgi_input[1]);
  //等待子进程的退出
  waitpid(pid, &status, 0);
  close(client);
 }
}


void handle(int sockfd)
{
    //pthread_detach(pthread_self()); //设置分离属性
    
    while(1)
    {
        struct HTTP head;
        struct MSG msg;


        memset(&head,0,sizeof(head));
        memset(&msg,0,sizeof(msg));
        //struct DATA data;
        char buf[TCPMAXLEN]={0};
        //int sockfd = *(int *)arg;
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

            if(strstr(head.url,"/cgi-bin/")!=NULL)
            {
                puts("请求了CGI");
                if(Isfileexist((struct HTTP *)&head) == true)   // 判断文件是否存在 
                {
                    puts("存在");
                     cgihandle(sockfd,(struct MSG *)&msg,(struct HTTP *)&head);
                   
                    

                }
                else            //不存在返回404页面
                {
                    puts("不存在");  
                    strcpy(msg.statinfo,"HTTP/1.1 404 Not Found\r\n"); 
                    strcpy(head.url,"/404.html");   // 改为发送404.html
                     fileLength((struct HTTP *)&head,(struct MSG *)&msg);    //填充长度
                    fileType((struct HTTP *)&head,(struct MSG *)&msg);         //填充文件类型

                    puts("----------------------------------------------");
                    printf("stat  %s",msg.statinfo);
                    printf("type  %s",msg.ContentTypeinfo);
                    printf("len   %s",msg.Lengthinfo);

                    send_head(sockfd,(struct MSG *)&msg,(struct HTTP *)&head);      //发送头部
                    send_data(sockfd,(struct MSG *)&msg,(struct HTTP *)&head);      //发送主体
                    
                }
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
    int conn = 0;
    int i = 0;
    while(1)
    {
        conn = accept(sockfd,NULL,NULL);
        pid_t pid = fork();
        //pthread_create(&tid[i++],NULL,handle,&conn[i]);
        if(pid>0)
		{
			close(conn);
		}
        if(0 == pid)
		{
			close(sockfd);
			while(1)
			{
                handle(conn);
			}

		}
        if(pid<0)
		{
            perror("fork");
            exit(1);
		}
        if(i>100)
            break;

    }
    close(sockfd);
    return 0;
}
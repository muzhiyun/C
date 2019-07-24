#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>         
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <termios.h>
#include "cJSON.h"



#define sign "c217ef7ab3b64a60b50f97b56f2efbca"
#define appkey "44079"
#define MAXLEN 20480
#define TCPMAXLEN 4096

struct termios old,new;

char Gcity[20]={0};
char Gdata[20]={0};
char Gfunction[20]={0};
char Grecv_buf[MAXLEN]={0};
char Gjson[MAXLEN]={0};
char Gjsonchild[MAXLEN]={0};
char Gvalue[MAXLEN]={0};

void offEcho()
{
    tcgetattr(0,&old);	
	tcgetattr(0,&new);	
	new.c_lflag &= ~(ECHO|ICANON);	
	new.c_cc[VTIME] = 0 ;	
	new.c_cc[VMIN]= 1;	
	tcsetattr(0,TCSANOW,&new);
}

int menuPrintf()
{
    system("clear");
	printf("+---------------------+\n");
	printf("|1.配置城市信息       |\n");
	printf("|2.查看当前环境信息   |\n");
	printf("|3.查看历史环境信息   |\n");
	printf("|4.查看未来环境信息   |\n");
	printf("|5.退出               |\n");
	printf("+---------------------+\n");

	return 0;
}

void chinPrintf(char* str,int sumlen)
{
    int len = strlen(str)/3;
    int i;
    printf("%s",str);
    for(i=0;i<sumlen-len;i++)
    {
        printf("%s","  ");
    }

}

int socket_init()
{
    int sockfd = socket(AF_INET,SOCK_STREAM,0);
    if(-1 == sockfd)
	{
		perror("socket");
		exit(1);
	}

    struct sockaddr_in serAddr;
	serAddr.sin_family = AF_INET;
	serAddr.sin_port = htons(80);
	serAddr.sin_addr.s_addr = inet_addr("139.199.7.215");

    int ret = connect(sockfd,(struct sockaddr *)&serAddr,sizeof(serAddr));
    if (-1 == ret)
    {
        perror("Network failed!");
        exit(1);
    }
    return sockfd;
}




int sendGet()
{

    int sockfd = socket_init();

    char head[128];

    sprintf(head,"GET /?app=weather.%s&weaid=%s&date=%s&appkey=%s&sign=%s&format=json HTTP/1.1\r\n",Gfunction,Gcity,Gdata,appkey,sign);

    int i = 0;
 
    char *send_cmd[9];
	
	send_cmd[0]="Host: api.k780.com\r\n";
	send_cmd[1]="Connection: keep-alive\r\n";
    send_cmd[2]="User-Agent: Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/58.0.3029.110 Safari/537.36 SE 2.X MetaSr 1.0\r\n";
	send_cmd[3]="Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8\r\n";
	send_cmd[4]="Accept-Encoding: gzip, deflate, sdch\r\n";
	send_cmd[5]="Accept-Language: zh-CN,zh;q=0.8\r\n\r\n";

    send(sockfd,head,strlen(head),0);
    printf("请求：%s",head);

    for(i=0;i<6;i++)
    {
        send(sockfd,send_cmd[i],strlen(send_cmd[i]),0);
    }
    return sockfd;
}

char* recvPrintf(char* jsonstring,char* key)
{
    cJSON* cjson = cJSON_Parse(jsonstring); //解析json
    if(cjson == NULL)
        return "0";
    else               //打包成功调用cJSON_Print打印输出
        {
            cJSON_Print(cjson);
            char* success_string = cJSON_GetObjectItem(cjson,key)->valuestring;
            strcpy(Gvalue,success_string);
            return success_string;
        }
    cJSON_Delete(cjson);
}




int recvGet(int sockfd)     //作用 ：接受返回内容 并切割提取json部分 
{

    int ret_num = 0;
    char* strp;
    char recvtemp[MAXLEN];
    char temp[MAXLEN];

    memset(Grecv_buf, 0, sizeof(Grecv_buf));                        //清空数组

    int fileio = open("data.txt",O_WRONLY|O_TRUNC|O_CREAT,0666);       //打开文件 只写 清空 创建 
    while(1)
    {
         ret_num = recv(sockfd,recvtemp,TCPMAXLEN,0);          //接受返回内容 所有返回内容全部存在Grecv中
         printf("数据：%d\n",ret_num);
         recvtemp[ret_num]= '\0';   
         write(fileio,recvtemp,strlen(recvtemp));            //Grecv 写入file
         

        // if(ret_num < TCPMAXLEN)      //如果数组未全满 则数据应该结束
        //    break;
        char *p = strstr(recvtemp,"\r\n0\r\n");
        if(p!=NULL)
            {
                *(p+6)='\0';
                break;
            }
         
    }
    puts("文件保存成功");
    close(fileio);
    fileio = open("data.txt",O_RDONLY);
    int ret = read(fileio,Grecv_buf,MAXLEN);  
    
    close(fileio);
    strcpy(temp,Grecv_buf);                 //复制一份返回内容用于提取总json

    char *offset = strstr(temp,"{\"success\":\""); //丢弃总json起始位置之前的内容

    int i = strlen(offset);

    

    while(i)                                //丢弃总json结束位置之后的内容
    {
        #if LOOP
        printf("%d\n",i);
        #endif

        if(*(offset+i)=='}')
        {
            *(offset+i+1)='\0';
            break;
        }
        i--;
    }
    if(i==0)
        return -1;

    strp = strstr(offset,"\r\n");
    if(strp != NULL)
    {
        puts("存在\\r\\n");
        puts(offset);
        strcpy(recvtemp,offset);
        strp = strtok(offset,"\r\n");
        strcpy(Gjson,strp);
        strp = strstr(recvtemp,"\r\n");
        //printf("strp:%d offset:%d\n",strlen(strp),strlen(offset));
        strp = strstr(strp+2,"\r\n");
        strcpy(temp,strp+2);
        //puts(Gjson);
        //puts("--------------------------------------");
        strcat(Gjson,temp);
        char *p = strstr(Gjson,"\r\n0\r\n");
        if(p!=NULL)
            {
                *(p+1)='\0';
            }
    //
    //printf("strp:%d temp:%d recvtemp:%d \n",strlen(strp+2),strlen(temp2),strlen(recvtemp));
    //strcpy(recvtemp,temp2);

    }
   
    
    else
    
        strcpy(Gjson,offset);           //结果保存到总json

    //puts(Gjson);

    fileio = open("Gjson.json",O_WRONLY|O_TRUNC|O_CREAT,0666);       //打开文件 只写 清空 创建 
    write(fileio,Gjson,strlen(Gjson));            //Grecv 写入file


    #if DEBUG
    puts("标志位4");
    #endif


    strcpy(temp,Gjson);            //复制一份总json用于提取子json
    #if DEBUG
    printf("提取总json：成功\n");
    #endif

    recvPrintf(Gjson,"success");    //验证接口的调用是否正常
    if(strcmp(Gvalue,"1")==0)
        printf("结果：成功\n\n");
    else
        {
            printf("结果：失败\n\n");
            printf("详细错误信息：\n%s\n",Gjson);
            return -1;
        }
    

/************************************************************************************************************************************************************************/
    if(strcmp("today",Gfunction)==0)        //提取实时天气的子json
    {
        offset = strstr(temp,"{\"weaid\":\""); //丢弃子json起始位置之前的内容
        i = strlen(offset);
        while(i)
        {
            if(*(offset+i)=='}')            //丢弃子json结束位置之后的内容
            {
                *(offset+i)='\0';
                break;
            }
            i--;
        }
        strcpy(Gjsonchild,offset);       //结果保存到子json
    }
 /**********************************************************************************************************************************************************************/   
  /**********************************************************************************************************************************************************************/
    else if(strcmp("history",Gfunction)==0)      //提取历史天气的子json
    {
        
       puts("准备提取子json");
        puts(Gjson);
        cJSON* cjson = cJSON_Parse(Gjson);

        //获取数组对象
        cJSON* test_arr = cJSON_GetObjectItem(cjson,"result");
        if(!test_arr){
            printf("no list!\n");
            return -1;
         }
        //获取数组对象个数便于循环
        int array_size = cJSON_GetArraySize(test_arr);//return arr_size 2
        #if DEBUG
        printf("数组对象个数：%d\n",array_size);
        #endif
        int i = 0;
        
        char *p  = NULL;
        printf ("\n         时间          城市名              天气              风向            风力                温度               湿度\n");
        puts("-------------------------------------------------------------------------------------------------------------------------");
        for(i=0; i< array_size; i++) 
        {
            cJSON *item,*it, *city_uptime,*city_name, *city_weather,*city_wind,*city_winp,*city_temp,*city_humidity;
            item = cJSON_GetArrayItem(test_arr, i);
            p = cJSON_PrintUnformatted(item);
            it = cJSON_Parse(p);
            city_uptime = cJSON_GetObjectItem(it, "uptime");
            city_name = cJSON_GetObjectItem(it, "citynm");
            city_weather = cJSON_GetObjectItem(it, "weather");
            city_wind = cJSON_GetObjectItem(it, "wind");
            city_winp = cJSON_GetObjectItem(it, "winp");
            city_temp = cJSON_GetObjectItem(it, "temperature");
            city_humidity = cJSON_GetObjectItem(it, "humidity");
           

            chinPrintf(city_uptime->valuestring,9);
            chinPrintf(city_name->valuestring,9);
            chinPrintf(city_weather->valuestring,9);
            chinPrintf(city_wind->valuestring,9);
            chinPrintf(city_winp->valuestring,9);
            chinPrintf(city_temp->valuestring,9);
            chinPrintf(city_humidity->valuestring,9);
            puts("");
 
        }
        cJSON_Delete(cjson);



    }
/********************************************************************************************************************************************************************/
/**********************************************************************************************************************************************************************/
/**********************************************************************************************************************************************************************/
    else if(strcmp("future",Gfunction)==0)      //提取未来天气的子json
    {
        
        cJSON* cjson = cJSON_Parse(Gjson);

        //获取数组对象
        cJSON* test_arr = cJSON_GetObjectItem(cjson,"result");
        if(!test_arr){
            printf("no list!\n");
            return -1;
         }
        //获取数组对象个数便于循环
        int array_size = cJSON_GetArraySize(test_arr);//return arr_size 2
        #if DEBUG
        printf("数组对象个数：%d\n",array_size);
        #endif
        int i = 0;
        
        char *p  = NULL;
        printf ("\n    日期            城市名              天气              风向              风力              最高温度          最低温度\n");
        printf ("---------------------------------------------------------------------------------------------------------------------\n");
        for(i=0; i< array_size; i++) 
        {
            cJSON *item,*it, *city_days,*city_name, *city_weather,*city_wind,*city_winp,*city_temp_high,*city_temp_low;
            item = cJSON_GetArrayItem(test_arr, i);
            p = cJSON_PrintUnformatted(item);
            it = cJSON_Parse(p);
            city_days = cJSON_GetObjectItem(it, "days");
            city_name = cJSON_GetObjectItem(it, "citynm");
            city_weather = cJSON_GetObjectItem(it, "weather");
            city_wind = cJSON_GetObjectItem(it, "wind");
            city_winp = cJSON_GetObjectItem(it, "winp");
            city_temp_high = cJSON_GetObjectItem(it, "temp_high");
            city_temp_low = cJSON_GetObjectItem(it, "temp_low");

            
            chinPrintf(city_days->valuestring,9);
            chinPrintf(city_name->valuestring,9);
            chinPrintf(city_weather->valuestring,9);
            chinPrintf(city_wind->valuestring,9);
            chinPrintf(city_winp->valuestring,9);
            chinPrintf(city_temp_high->valuestring,9);
            chinPrintf(city_temp_low->valuestring,9);
            //printf(" 长度:%d ",strlen(city_weather->valuestring));
            puts("");
 
        }
        cJSON_Delete(cjson);


    }

}
/**********************************************************************************************************************************************************************/
/**********************************************************************************************************************************************************************/
/**********************************************************************************************************************************************************************/
/**********************************************************************************************************************************************************************/
int askNow()
{
   
    strcpy(Gfunction,"today");
    int sockfd = sendGet();
    recvGet(sockfd);
    //puts(Gjsonchild);
    //puts("");
    printf("今日：%s %s\n%s的天气 %s \n当前温度 %s   今日最高温度 %s 今日最低温度 %s \n风向 %s 风力 %s",recvPrintf(Gjsonchild,"days"),recvPrintf(Gjsonchild,"week"),recvPrintf(Gjsonchild,"citynm"),recvPrintf(Gjsonchild,"weather"),recvPrintf(Gjsonchild,"temperature_curr"),recvPrintf(Gjsonchild,"temp_high"),recvPrintf(Gjsonchild,"temp_low"),recvPrintf(Gjsonchild,"wind"),recvPrintf(Gjsonchild,"winp"));
    close(sockfd);
}

int askHistroy()
{
    printf("请输欲查询的日期并回车\n");
    puts("eg.欲查询2019年1月1日的天气，请输入“2019-01-01”并回车\n");
    tcsetattr(0,TCSANOW,&old);	//打开回显 restore_key;
    scanf("%s",Gdata);
    getchar();
    offEcho();
    strcpy(Gfunction,"history");

    int sockfd = sendGet();
    if(recvGet(sockfd)!= -1)
        recvPrintf(Gjson,"success");
    close(sockfd);
}

int askFuture()
{
    strcpy(Gfunction,"future");
    int sockfd = sendGet();
    recvGet(sockfd);
    recvPrintf(Gjson,"success");
    close(sockfd);
}


int updateCity()
{
    strcpy(Gfunction,"0");
    while(1)
    {
        printf("当前城市为 %s ,请输入新的城市名并回车\n",Gcity);
        puts("eg.修改为北京，请输入“beijing”\n");
        tcsetattr(0,TCSANOW,&old);	//打开回显 restore_key;
        scanf("%s",Gcity);
        getchar();
        offEcho();
        strcpy(Gfunction,"today");
        int sockfd = sendGet();
        
        

        if(recvGet(sockfd) == -1)
        {
            system("clear");
            puts("城市不存在，请重新输入并回车！");
            continue;
        }
         break;

    }
    system("clear");
    puts("修改成功！");
    return 0;

}

int beforeExit()
{
    tcsetattr(0,TCSANOW,&old);	//打开回显 restore_key;
    exit(0);

}


int checkKey()
{
        while(1)
        {
            char c = getchar();
            switch(c)
            {
            case '1':
                    updateCity();				
                    return 1;					
                case '2':				
                    askNow();		
                    return 2;						
                case '3':	
                    askHistroy();			
                    return 3;
                case '4':
                    askFuture();
                    return 4;							
                case '5':				
                    beforeExit();		
                    return 5;	

            }
        }
} 






int main(int argc, char const *argv[])
{
    
    strcpy(Gcity,"xian");
    offEcho();
    while(1)
    {
        menuPrintf();
        checkKey();
        puts("\n\n\n按任意键返回主界面");
        getchar();
    }
    return 0;
}

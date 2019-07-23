/*
 * =====================================================================================
 *
 *       Filename:  main.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/26/2019 02:11:08 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  MuZhi (PuTe), my616422@126.com
 *   Organization:  MuZhi
 *
 * =====================================================================================
 */

#include"public.h"
#include"ll.h"
#include"ls.h"
#include"cd.h"
#include"cp.h"
#include"cat.h"
#include"top.h"
#include "ln.h"
#include"mkdir.h"
#include"rm.h"
#include"touch.h"
#include"mv.h"

long long int str2num(char *str)
{
	#if DEBUG
	printf("准备分割->str2num子函数：待解码命令为 %s\n",str);
	#endif
    long long int tmpnum=0;
	while(*str)
	{
	tmpnum=tmpnum*1000+(*str);
	str++;
	}
	return tmpnum;
}

int main()
{
    char input[512];
    struct passwd *pw;				//用户信息结构体
    pw = getpwuid(getuid());		//获取用户名
    char pathbak[512];
    pwd(pathbak);
	while(1)
	{
    char tmpstr[512];
    pwd(tmpstr);
    printf("%s:%s$",pw->pw_name,tmpstr);		//打印用户名
    fgets(input,512,stdin);			//接受输入

	if(input[0]=='\n')				//处理"无命令输入 单纯按回车"的情况
		continue;
	
	input[strlen(input)-1]='\0';	//清除fgets带来的回车
	
    char inputbak[512];				//备份输入
	strcpy(inputbak,input);
	
    char list[20][256];				//切割获取主命令
	char* tmp=NULL;
    tmp=strtok(input," ");
    stpcpy(list[0],tmp);

    int count=1;                    //切割循环获取参数
	while(1)
	{
	tmp=NULL;
    tmp=(strtok(NULL," "));
	if(NULL==tmp)
		break;
    //puts(tmp);
	stpcpy(list[count],tmp);
	count++;
	}
	
	
    int i=0;						//转存参数为指针
    char *pa[count];
    for( i=0;i<count;i++ )
    {
        pa[i]=list[i];
    }

    long long int ret = str2num(list[0]);


    #if 0
	puts("\n-----------------------debuging-------------------------");
	printf("传参数:%d\n",count);
	printf("输入检测->总命令:%s\n",inputbak);

    printf("%lld",ret);

     printf("分割结束->命令:%s 对应内部id:%lld ",list[0],ret);
    for(i=1;i<count;i++)
    {
    printf("参数%d：",i);
    puts(list[i]);
    }
    printf("准备执行命令:%s ",list[0]);
    printf("送参检测:%s\n\n",pa[1]);
	#endif



	switch(ret)
	{
        case 99100:
            cd(count,(const char **)pa);
            break;
		case 99112:
            cp_handle(count,(const char **)pa);
			break;
		case 108115:
            ls(count,(const char **)pa);
			break;
		case 108108:
            ll(count,(const char **)pa);
			break;
        case 114109:
            rm_handle(count,(const char **)pa);
            break;
        case 109107100105114:
            mkdir_handle(count,(const char **)pa);
            break;
        case 114109100105114:
            rmdir_handle(count,(const char **)pa);
            break;
        case 99097116:
            cat_handle(count,(const char **)pa);
            break;
        case 108110:
            ln_handle(count,(const char **)pa);
            break;
        case 104101108112:
            sprintf(tmpstr,"%s/%s",pathbak,"README");
            stpcpy(list[1],tmpstr);
             pa[1]=list[1];
            cat_handle(count+1,(const char **)pa);
            break;
        case 116111117099104:
            touch_handle(count,(const char **)pa);
            break;
        case 109118:
            move_handle(count,(const char **)pa);
            break;
        case 116111112:
            top_handle();
            break;
        case 101120105116:
             puts("ByeBye!");
             exit(0);
       default:
            printf("%s: command not found\n",list[0]);
            printf("You can try to enter \"help\"\n");
    }


	}
}

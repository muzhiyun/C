

#include"cd.h"

char *pwd(char *path)
{
    getcwd(path,512);
    return path;
}

void cd(int argc, const char *argv[])
{
    if(argc==1)
        return ;
    else if (argc==2)
    {
        if(strcmp("~",argv[1])==0)
        {
            char tmpstr[20];
            struct passwd *pw;				//用户信息结构体
            pw = getpwuid(getuid());		//获取用户名
            sprintf(tmpstr,"%s/%s","/home",pw->pw_name);
            chdir(tmpstr);
        }
        chdir(argv[1]);
        return ;
    }
    else
    {
        puts("too many");
        return ;
    }

}

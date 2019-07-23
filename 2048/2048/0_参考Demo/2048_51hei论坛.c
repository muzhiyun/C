#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <termios.h>

#define N 4

int a[N][N];
int tmp[N];

int change = 1;

struct termios old;
struct termios new;

void ShowMenu(void)
{
        int i = 0;
        int j = 0;

        system("clear");
        printf("\n\t**********************************\n");
        printf("\t\t欢迎小姐姐进入2048！\n");
        printf("\ta:left  d:right w:up s:down q:quit\n");
        printf("\t**********************************\n\n");
        printf("\t       ┌────┬────┬────┬────┐\n");
        
        for (j = 0; j < N; j++)
        {
                printf("\t       │");        

                for(i = 0; i < N; i++)
                {
                        if(0 == a[j][i])
                        {
                                printf("    │");
                        }
                        else
                        {
                                printf("%4d│", a[j][i]);
                        }

                }

                if (j < 3)
                {
                printf("\n\t       ├────┼────┼────┼────┤\n");
        }

        }
        printf("\n\t       └────┴────┴────┴────┘\n");

        printf("\n\t---------------------------------\n");
        return;
}

void GameBegin(void)
{
        int x = 0;
        int y = 0;

        while(1)
        {
                x = rand() % N;
                y = rand() % N;

                if (a[x][y] == 0)
                {
                        a[x][y] = rand() % 5 ? 2 : 4;
                        break;
                }        
        }

        ShowMenu();

        return;
}

void AddNum(void)
{
        int i = 0;
        int j = 0;

        for (j = 0; j < N-1; j++)
        {
                for (i = 0; i < N-1; i++)
                {
                        if (0 == tmp[i] && tmp[i+1] != 0)
                        {
                                tmp[i] = tmp[i+1];
                                tmp[i+1] = 0;
                                change = 1;
                        }
                }
        }

        for (i = 0; i < N-1; i++)
        {
                if (tmp[i] == tmp[i+1])
                {
                        tmp[i] *= 2;
                        tmp[i+1] = 0;
                        change = 1;
                }
        }
        
        for (j = 0; j < N-1; j++)
        {
                for (i = 0; i < N-1; i++)
                {
                        if (0 == tmp[i] && tmp[i+1] != 0)
                        {
                                tmp[i] = tmp[i+1];
                                tmp[i+1] = 0;
                        }
                }
        }
        
        return;
}

void LeftKey(void)
{
        int j = 0;
        int i = 0;
        
        for (j = 0; j < N; j++)
        {
                for (i = 0; i < N; i++)
                {
                        tmp[i] = a[j][i];
                }
                
                AddNum();

                for (i = 0; i < N; i++)
                {
                        a[j][i] = tmp[i];
                }
        }

        return;
}

void RightKey(void)
{
        int j = 0;
        int i = 0;
        
        for (j = 0; j < N; j++)
        {
                for (i = 0; i < N; i++)
                {
                        tmp[i] = a[j][N-1-i];
                }

                AddNum();

                for (i = 0; i < N; i++)
                {
                        a[j][N-1-i] = tmp[i];
                }
        }

        return;
}

void DownKey(void)
{
        int i = 0;
        int j = 0;

        for (j = 0; j < N; j++)
        {
                for (i = 0; i < N; i++)
                {
                        tmp[i] = a[N-1-i][j];
                }

                AddNum();

                for (i = 0; i < N; i++)
                {
                        a[N-1-i][j] = tmp[i];
                }
        }

        return;
}

void UpKey(void)
{
        int i = 0;
        int j = 0;

        for (j = 0; j < N; j++)
        {
                for (i = 0; i < N; i++)
                {
                        tmp[i] = a[i][j];
                }

                AddNum();

                for (i = 0; i < N; i++)
                {
                        a[i][j] = tmp[i];
                }
        }

        return;
}

void GetKey(void)
{
        char ch = 0;
        
        while (1)
        {
                ch = getchar();

                if('a' == ch || 'A' == ch || 'w' == ch || 'W' == ch ||
                                's' == ch || 'S' == ch || 'd' == ch || 'D' == ch
                                || 'q' == ch || 'Q' == ch)
                {
                        break;
                }
        }

        switch (ch)
        {
                case 'a':
                case 'A': LeftKey();break;
                case 'D': 
                case 'd': RightKey();break;
                case 's': 
                case 'S': DownKey();break;
                case 'w': 
                case 'W': UpKey();break;
                case 'q': 
                case 'Q': exit(0);break;
                default:
                                  break;
        }

        return;
}

void Succeed(void)
{
        int i = 0;
        int j = 0;

        for (j = 0; j < N; j++)
        {
                for (i = 0; i < N; i++)
                {
                        if (256 == a[j][i])
                        {
                                printf("\t\tCongratulation!\n");
                        }
                }
        }

        return;
}

void GameOver(void)
{
        int i = 0;
        int j = 0;
        int cnt = 0;
        
        for (j = 0; j < N; j++)
        {
                for (i = 0; i < N; i++)
                {
                        if (0 == a[j][i])
                        {
                                cnt++;
                        }
                }
        }

        if (cnt != 0)
        {
                return;
        }
        
        for (j = 0; j < N-1; j++)
        {
                for (i = 0; i < N-1; i++)
                {
                        if (a[j][i] == a[j][i+1] || a[j][i] == a[j+1][i])
                        {
                                return;
                        }
                }
        }

        printf("\t\t GAMEOVER!\n");

        return;

}

void ChangTerminalState(void)
{
        tcgetattr(0, &old);
        tcgetattr(0, &new);

        new.c_lflag = new.c_lflag & ~(ICANON | ECHO);

        new.c_cc[VTIME] = 0;

        new.c_cc[VMIN] = 1;

        tcsetattr(0, TCSANOW, &new);

        return;

}

int main(int argc, const char *argv[])
{
        ChangTerminalState();

        srand(time(NULL));

        while(1)
        {
                if (change)
                {
                        GameBegin();
                }
        
                Succeed();        
                
                GameOver();
                change = 0;
                GetKey();
        }

        return 0;
}
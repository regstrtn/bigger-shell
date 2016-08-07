#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h> 
#include<sys/stat.h>
#include<signal.h>
#include <termios.h>
#include <sys/select.h>
#include <sys/ioctl.h>
//#include <curses.h>
void split(char *wordP[16384],char command[16384]);
int getch();
void main()
{ 
        char command[16384],ch,*p,path[16384];
        char *wordP[16384];
        char word,arrow;
        int com_len;
        int i=0,j=0,l=0,k=0,b=0,up=1,down=1,nav=0;
        int NoOfWords=1;
        int status;
        char *env[] = { 0 };
        char *myCommand[]={"mypwd","mymkdir","mycd","myrm","mymv","myps","myls","mytail","mygrep","pipe"};
        int com_no=0;
        char pwd[16384],fd[16384],temp[16384];
        char commands[64][16384];
        int countOfCommand=0;
        struct stat sb;
        pid_t pid;
        getcwd(pwd,16384);
        getcwd(fd,16384);
        memset(commands,'\0',64*16384);
        JUMP1 : while(1)
        {                
                getcwd(path,16384);
                memset(command,'\0',sizeof(command));
                i=0;
                NoOfWords=1;
                j=0;
                l=-1;
                nav=0;
                up=down=1;
                // Reading the command.
                printf("\nCMD#>");
                JUMP2 :while((ch=getch())!='\n')
                {
                    if(ch==127)
                    {
                        printf("\b \b");
                        if(nav)
                        {
                           command[strlen(command)-1]='\0';
                        }
                        else
                        {
                        commands[k][--i]='\0';
                        }
                        fflush(stdout);
                        continue;
                    }
                    if(ch=='\033')
                    {
                        getch();
                        arrow=getch();
                        //if(i!=0) for(j=0;j<i;j++)printf("\b");
                        i=0;
                        if(k==0)goto JUMP2;
                        if(l==-1)l=k-1;
                        switch(arrow)
                        {
                            case 'A':if(up)
                                     {
                                        fputs("\033[2K\rCMD#>",stdout);
                                        printf("%s",commands[l]);
                                        nav=1;
                                        strcpy(command,commands[l]);
                                        if(l!=0)l--;
                                        down=0;
                                     }
                                     else
                                     {
                                        if(l==k) l--;
                                        else if(l!=0)l-=2;
                                        fputs("\033[2K\rCMD#>",stdout);
                                        printf("%s",commands[l]);
                                        nav=1;
                                        strcpy(command,commands[l]);
                                        up=1;
                                        down=0;
                                        if(l!=0)l--;
                                     }
                                     break;
                            case 'B':if(down)
                                     {
                                        if(l>k-1)
                                        {
                                           fputs("\033[2K\rCMD#>",stdout);
                                           nav=0;
                                           down=1;
                                           up=0;
                                           goto JUMP2;
                                        } 
                                        fputs("\033[2K\rCMD#>",stdout);
                                        printf("%s",commands[l]);
                                        nav=1;
                                        strcpy(command,commands[l]);
                                        down=1;
                                        up=0;
                                        if(l<k)l++;
                                        
                                        up=0;
                                     }
                                     else
                                     {
                                        if(l==0) l++;
                                        else if(l<k)l+=2;
                                        if(l>k-1)
                                        {
                                           fputs("\033[2K\rCMD#>",stdout);
                                           nav=0;
                                           down=1;
                                           up=0;
                                           goto JUMP2;
                                        }
                                        fputs("\033[2K\rCMD#>",stdout);
                                        printf("%s",commands[l]);
                                        nav=1;
                                        strcpy(command,commands[l]);
                                        if(l<k)l++;
                                        down=1;
                                        up=0;
                                     }
                                     break;
                            case 'C':goto JUMP2;
                            case 'D':goto JUMP2;
                        }
                    }
                    else
                    {
                        printf("%c",ch);
                        commands[k][i++]=ch;
                    }
                }
                if(ch=='\n')printf("\n");
                if(i!=0)
                {
                  commands[k][i]='\0';
                  k++;
                }
                else if(command[0]=='\0')
                {
                  continue;
                }
                if(nav)
                {
                    split(wordP,command);
                }
                else    
                {
                    strcpy(command,commands[k-1]);
                    split(wordP,command); 
                }
                //selecting the give command program and executing it.
                i=0;
                while(strcmp(wordP[0],myCommand[i++]))if(i==10){ i++;break ;}  
                com_no=i;
                //Pipe
                if(strstr(command,"|")) com_no=10;
                sprintf(path,"%s/%s",path,myCommand[com_no-1]);
                fflush(stdout);
                pid=fork();
                if(pid==0)
                {
                    *wordP=pwd;
                    switch(com_no)
                    {
                        case 1  :    execve(path,wordP,env);
                                     break;
                        case 2  :    execve(path,wordP,env);
                                     break;
                        case 3  :    if(wordP[1][0]=='.' && wordP[1][1]=='.')
                        	     {
                        	     	l=strlen(pwd);
                        	     	while(pwd[l-1]!='/')
                        	     	{
                        	     		pwd[--l]='\0';
                        	     	}
                        	     	pwd[l-1]='\0';
                        	     	l=0;
                        	     	break;
                        	     }
                        	     if(wordP[1][0]=='/')
                                     {
                                        memset(temp,'\0',16384);
                                        strcpy(temp,wordP[1]);
                                        //printf("%s",pwd);
                                     }
                                     else
                                     {
                                        memset(temp,'\0',16384);
                                        sprintf(temp,"%s/%s",pwd,wordP[1]);
                                        //printf("%s",pwd);   
                                     }
                                     if(!(stat(temp, &sb) == 0 && S_ISDIR(sb.st_mode)))
                                     {
                                        printf("cd: %s: No such file or directory",wordP[1]);
                                        
                                     }
                                     else
                                     {
                                     	memset(pwd,'\0',16384);
                                        strcpy(pwd,temp);
                                     }   
                                     break; 
                        case 4  :    execve(path,wordP,env);
                                     break;
                        case 5  :    execve(path,wordP,env);
                                     break;
                        case 6  :    execve(path,wordP,env);
                                     break;             
                        case 7  :    execve(path,wordP,env);
                                     break; 
                        case 8  :    execve(path,wordP,env);
                                     break;
                        case 9  :    execve(path,wordP,env);
                                     break;  
                        case 10 :    wordP[1]=command;
                                     wordP[2]="\0";
                                     execve(path,wordP,env);
                                     break;                                   
                        default :    printf("No command found :P");                                                
                                  
                    }
                }
                else
                {
                    wait(&status);
                }
                
        }
}


void split(char *wordP[16384],char command[16384])
{
    int i=0,j=0,k=0;
    wordP[j++]=command;
    while(command[i]!='\0')
    {
        if(command[i]==' ')
        {
            command[i]='\0';
            i++;
            wordP[j++]=&command[i];
        }
        i++;
    }
    wordP[j]="\0";
}

int getch()
{
    struct termios oldattr, newattr;
    int ch;
    tcgetattr( STDIN_FILENO, &oldattr );
    newattr = oldattr;
    newattr.c_lflag &= ~( ICANON | ECHO );
    tcsetattr( STDIN_FILENO, TCSANOW, &newattr );
    ch = getchar();
    tcsetattr( STDIN_FILENO, TCSANOW, &oldattr );
    return ch;
}

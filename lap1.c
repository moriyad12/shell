#include <stdio.h>
#include<unistd.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/resource.h>
//input of user
char str[100];
//command of terminal
char commands[100][100];
//directory
char directory[100];
char* com[100];
//number of commands
int z=0;
//pid of process
int pid;
//parent will wait child or not
bool waiting=true;
//convert input to commands
int parseSpace()
{ int i=0,k=0,l=0;
waiting=true;
    for(int f=0;f<100;f++)
    {
        for(int p=0;p<100;p++)
            commands[f][p]='\0';
    }
    str[strlen(str)-1]='\0';
    while(str[i]!='\0')
    {
        if(str[i]=='$') {
            int r=0;
            char key[100];
            i++;
            for (int j = r; j <100 ; ++j) {
                key[j]='\0';
            }
            while(str[i]!='\0' && str[i]!='\n'&&str[i]!='"'&&str[i]!=' ')
            {
                key[r]=str[i];
                r++;
                i++;
            }
            char* value= getenv(key);

            for(int n=0;n<strlen(value);n++)
            {
                if(value[n]=='\n'||value[n]==' ') {
                    commands[k][l]='\0';
                    k++;
                    l=0;}
                else {
                    commands[k][l++]=value[n];
                }
            }
            if(str[i]==' ') {
                commands[k][l]='\0';
                k++;
                l=0;}

        }
        else if(str[i]==' ') { if(k!=0&&!strcmp(commands[0], "export")) commands[k][l++]=str[i];
            else {
                commands[k][l]='\0';
                k++;
                l=0;}}
        else {
            if(str[i]!='"')
                commands[k][l++]=str[i];
        }
        i++;
    }
    if(commands[1][0]=='&')
    {waiting=false;
    k--;}
    return k+1;

}
//print directory
void printDirectory(){
    printf("%s\n", getcwd(directory, 100));
}
//determine directory
void setup_environment() {
    char dir[100];
    getcwd(dir, sizeof(dir));
    chdir(dir);
    printDirectory();
}
//echo function

void echo(){
    for(int i=1;i<z;i++)
        printf("%s ",commands[i]);
    printf("\n");
}
//cd function
void cd(){
    if(z<2||commands[1][0]=='~')
        chdir(getenv("HOME"));
    else
        chdir(commands[1]);
    printDirectory();
}
//export function
void Export(){
    char key[100],value[100];
    int j=0;
    for (int j = 0; j <100 ; ++j) {
        key[j]='\0';
    }
    while(commands[1][j]!='=')
    {
        key[j]=commands[1][j];
        j++;
    }
    key[j]='\0';
    j++;
    int l=0;
    while(commands[1][j]!='\0')
    {   if(commands[1][j]=='"') j++;
        else value[l++]=commands[1][j++];
    }
    value[l]='\0';
    setenv( key, value , 1);

}
void  execute_shell_bultin()
{
    if(!strcmp(commands[0],"echo"))
        echo();
    else if(!strcmp(commands[0],"cd"))
        cd();
    else if(!strcmp(commands[0],"export"))
        Export();}


void execute_command(){
    pid =fork();

    if(pid<0)
        printf("ERROR\n");
        //child
    else if(pid==0)
    {    for(int i=0;i<100;i++)
            com[i]=NULL;
        for(int i=0;i<z;i++) {
            com[i] = commands[i];

        }
        int c=  execvp(com[0] , com);
        if (c < 0) {
           printf("ERROR \n");
            exit(0);
        }
    }
    //parent
    else {
        if(waiting)
        waitpid(pid, NULL, 0);
    }
}
//taking input from user
void shell()
{
    while(1)
    {fgets(str, sizeof str, stdin);
        z=parseSpace();
        if(!strcmp(commands[0], "cd")|| !strcmp(commands[0], "echo")|| !strcmp(commands[0], "export"))
            execute_shell_bultin();
        else if(!strcmp(commands[0], "exit"))
            exit(0);
        else
            execute_command();
    }
}
//reap zombie process
void reap_child_zombie(){
    while (waitpid((pid_t)(-1), 0, WNOHANG) > 0) {}
}
//log to text file
void l() {
    FILE *f;
    f = fopen("ryad.txt", "a");
    fputs("termination of child process\n", f);
    fclose(f);
}
void on_child_exit() {
    reap_child_zombie();
    l();
}
int main() {
    signal(SIGCHLD, on_child_exit);
    setup_environment();
    shell();
}

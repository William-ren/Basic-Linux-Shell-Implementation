
#include <sys/dir.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <termios.h>
#include <ctype.h>
#include <fcntl.h>
#include <sys/types.h>

static char perms_buff[30];
char pathname[256];
char *token;
char *pathtoken;
char *token1;
char *argv[10]={NULL};
char *argv2[20]={NULL};
void die(char *msg){
  perror(msg);
  exit(0);
}
static int one (const struct dirent *unused){
    return 1;
}
const char *get_perms(mode_t mode){
    char temp[30];
    char *ftype = "?";
    if (S_ISREG(mode)) ftype = "-";
    if (S_ISLNK(mode)) ftype = "l";
    if (S_ISDIR(mode)) ftype = "d";
    if (S_ISBLK(mode)) ftype = "b";
    if (S_ISCHR(mode)) ftype = "c";
    if (S_ISFIFO(mode)) ftype = "|";
    strcat(temp,ftype);
    if (mode & S_IRUSR) strcat(temp,"r");
    else strcat(temp,"-");
    if (mode & S_IWUSR) strcat(temp,"w");
    else strcat(temp,"-");
    if (mode & S_IXUSR) strcat(temp,"x");
    else strcat(temp,"-");
    if (mode & S_IRGRP) strcat(temp,"r");
    else strcat(temp,"-");
    if (mode & S_IWGRP) strcat(temp,"w");
    else strcat(temp,"-");
    if (mode & S_IXGRP) strcat(temp,"x");
    else strcat(temp,"-");
    if (mode & S_IROTH) strcat(temp,"r");
    else strcat(temp,"-");
    if (mode & S_IWOTH) strcat(temp,"w");
    else strcat(temp,"-");
    if (mode & S_IXOTH) strcat(temp,"x");
    else strcat(temp,"-");
    strcat(temp," ");

    strcpy(perms_buff,temp);
    return (const char *)perms_buff;
}
void displayatbegin(char *path){
    if(strlen(path)<=16)
    {
        write(1,path,strlen(path));
        write(1,">",1);
    }
    else
    {
         int p=0;
         char  temppath[256];

         strcpy(temppath,path);
         pathtoken =strtok(temppath,"/");
         memset(argv2,0,100);
         while(pathtoken!=NULL){
             argv2[p]=pathtoken;
             pathtoken=strtok(NULL,"/");
             p++;
         }
        write(1,"/.../",5);
        write(1,argv2[p-1],strlen(argv2[p-1]));
        write(1,">",1);
    }
}
void SetNonCanonicalMode(int fd, struct termios *savedattributes){
    struct termios TermAttributes;
    char *name;

    // Make sure stdin is a terminal.
    if(!isatty(fd)){
        fprintf (stderr, "Not a terminal.\n");
        exit(0);
    }
    // Save the terminal attributes so we can restore them later.
    tcgetattr(fd, savedattributes);

    // Set the funny terminal modes.
    tcgetattr (fd, &TermAttributes);
    TermAttributes.c_lflag &= ~(ICANON | ECHO); // Clear ICANON and ECHO.
    TermAttributes.c_cc[VMIN] = 1;
    TermAttributes.c_cc[VTIME] = 0;
    tcsetattr(fd, TCSAFLUSH, &TermAttributes);
}
void ResetCanonicalMode(int fd, struct termios *savedattributes){
    tcsetattr(fd, TCSANOW, savedattributes);
}
char currentpath[256];
char prepath[256];
int main()
{
    struct termios SavedTermAttributes;
    memset(currentpath,'\0',strlen(currentpath));
    getcwd(currentpath,sizeof(currentpath));
    char command[256];
    char command_case[256];
    //char realcommand[256];
    char RXChar;
    int upcount=0;
    int flag1=0;
    int flag2=0;
    int commandcount=0;
    int displayflag=1;
    int historyflag=0;
    int historycount=0;
    int currenthistory=-1;
    char location[10];
    char history[10][100];
    SetNonCanonicalMode(STDIN_FILENO, &SavedTermAttributes);
    while(1){
        if(displayflag==1){
            displayatbegin(currentpath);
            displayflag=0;
         }
        int i=0;
        read(STDIN_FILENO, &RXChar, 1);
        if(0x04 == RXChar){ // C-d
             break;
        }
        else{
             if(isprint(RXChar)){
                 if(flag1!=1&&flag2!=1){
                   command[commandcount]=RXChar;
                   commandcount=commandcount+1;
                   write(1,&RXChar,1);
                 }

                if(flag2==1){
                   if(RXChar==0x41) {
                        upcount++;
                        if(upcount==1){
                            while(commandcount!=0){
                                write(1,"\b \b",3);
                                commandcount--;
                            }
                            write(1,history[currenthistory-upcount+1],strlen(history[currenthistory-upcount+1]));
                            strcpy(command,history[currenthistory-upcount+1]);
                            commandcount=strlen(history[currenthistory-upcount+1]);
                        }
                       
                        if(upcount>1&&upcount<=currenthistory+1){
                            int temp2=commandcount;
                            while(temp2!=0){
                                write(1,"\b \b",3);
                                temp2--;
                            }
                            write(1,history[currenthistory-upcount+1],strlen(history[currenthistory-upcount+1]));
                            strcpy(command,history[currenthistory-upcount+1]);
                            commandcount=strlen(history[currenthistory-upcount+1]);
                        }
                        if(upcount>currenthistory+1&&upcount!=1){
                            write(1,"\a",1);
                            upcount--;
                        }
                   }
                   if(RXChar==0x42) {
                        if(upcount==0){
                            commandcount=0;
                            write(1,"\a",1);
                        }
                        else if(upcount==1){
                            int temp4=commandcount;
                            while(temp4!=0){
                                write(1,"\b \b",3);
                                temp4--;
                            }
                            commandcount=0;
                            upcount--;
                        }
                        else if(upcount>1){
                            int temp3=commandcount;
                            while(temp3!=0){
                                write(1,"\b \b",3);
                                temp3--;
                            }
                            upcount--;
                            write(1,history[currenthistory-upcount+1],strlen(history[currenthistory-upcount+1]));
                            strcpy(command,history[currenthistory-upcount+1]);
                            commandcount=strlen(history[currenthistory-upcount+1]);
                        }
                   }
                    flag1=0;
                    flag2=0;
                }
                 if(flag1==1&&RXChar==0x5B){
                     flag2=1;
                 }
             }
             else{
                 if (RXChar==0x1B) flag1=1;
                 if (RXChar==0x20) {command[commandcount]=RXChar; commandcount=commandcount+1;}
                 if (RXChar==0x7F)
                 {
                   if(commandcount!=0)
                   {command[commandcount-1]='0';
                   commandcount=commandcount-1;
                   write(1,"\b \b",3);}
                 }
                 if (RXChar==0x0A&&commandcount==0) {
                     write(1,"\n",1);displayflag=1;
                 }
                 if (RXChar==0x0A&&commandcount!=0){
                     upcount=0;
                     displayflag=1;
                     command[commandcount]='\0';
                     commandcount=0 ;
                     if(currenthistory==9){
                         int temp1=0;
                         while(temp1<9){
                             memset(history[temp1],'0',100);
                             strcpy(history[temp1],history[temp1+1]);
                             temp1++;
                         }
                         strcpy(history[currenthistory],command);
                     }
                     if(currenthistory<9)
                     {
                         currenthistory++;
                         strcpy(history[currenthistory],command);
                     }
                     write(1,"\n",1);
                     int num_order=1;
                     int temp_num_order;
                     int flag_redirect=0;
                     for(temp_num_order=0;temp_num_order<strlen(command);temp_num_order++){
                         if(command[temp_num_order]=='|')
                              num_order++;
                         if(command[temp_num_order]=='<'||command[temp_num_order]=='>')
                              flag_redirect=1;
                     }
                     if(num_order>=2){

                         pipel(command,num_order);
                         memset(command,0,256);
                     }
                     else if(flag_redirect==1){
                         redirect(command);
                         memset(command,0,256);
                     }
                     else
                     {
                         strcpy(command_case,command);
                         token =strtok(command," ");

                         while(token!=NULL){
                             argv[i]=token;
                             i++;
                             token=strtok(NULL," ");
                         }

                         if(strcmp(argv[0],"cd")==0){
                             if (argv[1]!=NULL){
                                 chdir(argv[1]);
                                 memset(currentpath, '\0', strlen(currentpath));
                                 getcwd(currentpath,sizeof(currentpath));
                             }
                             else
                             {
                                 chdir(getenv("HOME"));
                                 memset(currentpath, '\0', strlen(currentpath));
                                 getcwd(currentpath,sizeof(currentpath));
                             }
                         }
                         else if (strcmp(argv[0],"ls")==0){

                             int count,j;
                             struct direct **files;
                             struct stat statbuf;
                             if(argv[1]==NULL)
                             {
                                 if(!getcwd(pathname, sizeof(pathname)))
                                     die("Error getting pathnamen");
                                 count = scandir(pathname, &files, one, alphasort);
                             }
                             else{
                                 memset(currentpath, '\0', strlen(currentpath));
                                 getcwd(currentpath,sizeof(currentpath));
                                 chdir(argv[1]);
                                 getcwd(pathname, sizeof(pathname));
                                 count=scandir(pathname, &files, one, alphasort);
                             }
                             if(count > 0){
                                 for (j=0; j<count; ++j){
                                     if (stat(files[j]->d_name, &statbuf) == 0)
                                     {
                                         write(1,get_perms(statbuf.st_mode),strlen(get_perms(statbuf.st_mode)));
                                         write(1,files[j]->d_name,strlen(files[j]->d_name));
                                         write(1,"\n",1);
                                     }
                                     free (files[j]);
                                 }
                                 free(files);
                                 chdir(currentpath);
                             }
                         }
                         else if (strcmp(argv[0],"exit")==0){
                             break;
                         }
                         else if (strcmp(argv[0],"pwd")==0)
                         {

                             write(1,currentpath,strlen(currentpath));
                             write(1,"\n",1);
                         }
                         else if (strcmp(argv[0],"history")==0){
                             int temp=0;
                             while(temp<=currenthistory){
                                 location[temp]=temp+'0';
                                 write(1," ",1);
                                 write(1,&location[temp],1);
                                 write(1," ",1);
                                 write(1,history[temp],strlen(history[temp]));
                                 write(1,"\n",1);
                                 temp++;
                             }
                         }
                         else{
                             redirect(command_case);
                             memset(command_case,0,256);
                         }
                         memset(command,0,256);
                     }

                 }

             }
        }
    }
    ResetCanonicalMode(STDIN_FILENO, &SavedTermAttributes);
}
char *is_file_exist(const char *order){
    char * path, * p;
    char *buffer;
    int i,max_length;
    i = 0;
    path=getenv("PATH");
    p=path;
    max_length=strlen(path)+strlen(order)+2;
    buffer=(char *)malloc(max_length*(sizeof(char)));
    while(*p != '\0'){
        if(*p != ':')
            buffer[i++] = *p;
        else{
            buffer[i++] = '/';
            buffer[i] = '\0';
            strcat(buffer,order);
            if(access(buffer,F_OK) == 0)
                return buffer;
            else
                i=0;
        }
        p++;
    }
    return NULL;
}
int redirect(char * input){
    char * command_path, *real_command;
    char * argv_re[10]={NULL};
    char *out_file,*in_file;
    int i=0,j;
    int flag_out=0,flag_in=0,fd_out,fd_in,pid,status;
    int len=strlen(input);
    out_file=(char *)malloc((len+1)*(sizeof(char)));
    in_file=(char *)malloc((len+1)*(sizeof(char)));
    real_command=(char *)malloc((len+1)*(sizeof(char)));
    for(i=0;i<len;i++){
        if (input[i]!='>'&&input[i]!='<')
            real_command[i]=input[i];
        else{
            if (input[i]=='>')
                flag_out=1;
            if (input[i]=='<')
                flag_in=1;
            break;
        }
    }
    real_command[i]='\0';
    i++;

    if(flag_out==1&&input[i]=='>')
    {
        flag_out=2;
        i++;
    }
    while ((input[i]==' ')&&i<len)
        i++;
        j=0;
    out_file[0]='\0';
    in_file[0]='\0';
    if(flag_out>0){
        while (i<=len){
   
            if(input[i]=='<'){
                out_file[j]='\0';
                break;
            }
            out_file[j]=input[i];
            i++;
            j++;
        }
    }
    if(flag_in>0){
        while(i<len){
            if (input[i]=='>'){
                in_file[j]='\0';
                break;
            }
            in_file[j]=input[i];
            i++;
            j++;
        }
    }
    if (i<len)
    {
        j=0;
        if (flag_in>0&&input[i]=='>')
        {
            i++;
            flag_out=1;
            if(input[i]=='>')
            {
                flag_out=2;
                i++;
            }
            while (input[i]==' '&&i<len)
                i++;
            while (i<=len){
                out_file[j]=input[i];
                i++;
                j++;
            }
        }
    }
    int q=0;
    token1 =strtok(real_command," ");
    while(token1!='\0')
    {
        argv_re[q]=token1;
        q++;
        token1=strtok(NULL," ");
    }
    argv_re[q]='\0';

    command_path=is_file_exist(argv_re[0]);
    if((pid=fork())==0){
        if(flag_out==1)
            fd_out = open(out_file,O_WRONLY|O_CREAT|O_TRUNC, S_IRUSR|S_IWUSR );
        if(flag_out==2)
            fd_out = open(out_file, O_WRONLY|O_CREAT|O_APPEND, S_IRUSR|S_IWUSR );
        if(flag_in==1)
            fd_in = open(in_file, O_RDONLY, S_IRUSR|S_IWUSR );
        if(flag_out>0) dup2(fd_out,STDOUT_FILENO);
        if(flag_in>0) dup2(fd_in,STDIN_FILENO);
        if (strcmp(argv_re[0],"ls")==0){
            int count,j;
            struct direct **files;
            struct stat statbuf;
            if(argv_re[1]==NULL){
                if(!getcwd(pathname, sizeof(pathname)))
                    die("Error getting pathnamen");
                count = scandir(pathname, &files, one, alphasort);
            }
            else{
                memset(currentpath, '\0', strlen(currentpath));
                getcwd(currentpath,sizeof(currentpath));
                chdir(argv[1]);
                getcwd(pathname, sizeof(pathname));
                count=scandir(pathname, &files, one, alphasort);
            }
            if(count > 0){
                for (j=0; j<count; ++j)
                {
                    if (stat(files[j]->d_name, &statbuf) == 0)
                    {
                        write(1,get_perms(statbuf.st_mode),strlen(get_perms(statbuf.st_mode)));
                        write(1,files[j]->d_name,strlen(files[j]->d_name));
                        write(1,"\n",1);
                    }
                    free (files[j]);
                }
                free(files);
                chdir(currentpath);
            }
        }
        else{
            execv(command_path,argv_re);}
        exit(1);
    }
    else
        pid=waitpid(pid, &status, 0);
    return 1;
}

int pipel(char * input,int num_order){
    int redirect(char * input);
    int len=strlen(input);
    int i,j=0,k=0,status;
    int *fd[num_order];
    int child[num_order];
    char order[num_order][20];
    for(i=0;i<=len;i++){
        if(input[i]!='|'){
   
            order[k][j]=input[i];
            j++;
        }
        else{
            order[k][i]='\0';
            j=0;
            k++;
        }
    }
    for(i=0;i<num_order;i++) pipe(fd[i]);
    i=0;
    if((child[i]=fork())==0){
        close(fd[i][0]);
        if(fd[i][1] != STDOUT_FILENO)
        {
            if(dup2(fd[i][1], STDOUT_FILENO) == -1)
            {
                fprintf(stderr, "Redirect Standard Out error !\n");
                return -1;
            }
            close(fd[i][1]);
        }
        redirect(order[i]);
        exit(1);
    }
    else{
        waitpid(child[i],&status,0);
        close(fd[i][1]);
    }
    i++;
    while(i<k){
        if ((child[i]=fork())==0){
            if(fd[i][0] != STDIN_FILENO){
                dup2(fd[i-1][0], STDIN_FILENO);
                close(fd[i-1][0]);
                dup2(fd[i][1], STDOUT_FILENO);
                close(fd[i][1]);
            }
            redirect(order[i]);
            exit(1);
        }
        else{
            waitpid(child[i],&status,0);
            close(fd[i][1]);
            i++;
        }
    }
    if((child[i] = fork()) == 0){
        close(fd[i-1][1]);
        if(fd[i-1][0] != STDIN_FILENO){
            dup2(fd[i-1][0], STDIN_FILENO);
            close(fd[i-1][0]);
        }
        redirect(order[i]);
        exit(1);
    }
    else{
        waitpid(child[i], NULL, 0);
        close(fd[i-1][1]);
    }
    return 1;
}

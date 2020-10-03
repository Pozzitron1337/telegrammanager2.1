#include <fcntl.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <pwd.h>
#include <string.h>
#include <time.h>

class TelegramCLI{
    private:
        const char* path_to_telegramcli="/tg/bin/telegram-cli";
        const char* telegramcli_name="telegram-cli";
        char* full_path_to_telegramcli;
        int fd[2];//pipe: 0-pipe_in, 1-pipe-out
        pid_t child_pid;
        FILE* writer;//writer to pipe;
    public:

        TelegramCLI(){
            struct passwd* pw;
            uid_t uid;
            uid = geteuid ();
            pw = getpwuid (uid);

            size_t len=strlen(pw->pw_dir)+strlen(path_to_telegramcli);
            full_path_to_telegramcli=(char*)malloc(len);
            strcat(full_path_to_telegramcli,pw->pw_dir);
            strcat(full_path_to_telegramcli,path_to_telegramcli);
           
            pipe(fd);
            child_pid=fork();

            if (child_pid != 0) { //parent
                sleep(2);
                printf("%s\n","im here");
                writer=fdopen(fd[1],"w");
                fprintf(writer,"%s\n","dialog_list");
                fflush(writer);
                sleep(2);
                fprintf(writer,"%s\n","contact_list");
                fflush(writer);
                sleep(2);
            }
            else //child
            {
                /*redirect fd[0] to stdin*/
                dup2(fd[0],fileno(stdin));
                
                /*redirect stdout to file*/
                char* file_name=(char*)"log.txt";
                fflush(stdout);
                int new_stdout = open(file_name, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
                dup2(new_stdout, fileno(stdout));
                close(new_stdout);
                
                /*execute telegram-cli*/
                execl(full_path_to_telegramcli,telegramcli_name,NULL);
            }   
        }
        void sendMsg(char* chat_name,char* message){
            fprintf(writer,"msg %s %s\n",chat_name,message);
            fflush(writer);
        }
        
        void sendPhoto(char* chat_name,char* path_to_photo){
            fprintf(writer,"send_photo %s %s\n",chat_name,path_to_photo);
            fflush(writer);
        }

        void sendFile(char* chat_name,char* path_to_file){
            fprintf(writer,"send_file %s %s\n",chat_name,path_to_file);
            fflush(writer);
        }
    
        ~TelegramCLI(){
            printf("%s\n","Destructor is called!The app will be closed in 5 seconds");
            sleep(5);
            fclose(writer);
            close(fd[0]);
            close(fd[1]);
            free(full_path_to_telegramcli);
            kill(child_pid,SIGKILL);
        }

};

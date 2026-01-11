#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/stat.h>

#define MAX_LINE 1024
#define MAX_ARGS 64

void sigchld_handler(int sig) {
    //Wait for all the child processes that have terminated
    while(waitpid(-1, NULL, WNOHANG) > 0);
}

int main (void) {
    char line [MAX_LINE];
    char *args [MAX_ARGS];

    //Setup the treatment of SIGCHLD
    struct sigaction sa; //Define the variable sa of type struct sigaction
    sa.sa_handler = sigchld_handler; //Point to the handler function 
    sigemptyset(&sa.sa_mask); //Initialize the signal set to empty
    sa.sa_flags = SA_RESTART; // With sa_restart fgets will work if a signal is caught

    //Register the signal and treatment for SIGCHLD
    sigaction(SIGCHLD, &sa, NULL);

    while (1) {

        //Print shell
        printf ("myshell> ");
        fflush (stdout);

        //Read from command line with fgets
        if (fgets(line, MAX_LINE, stdin) == NULL){
            perror("fgets failed");
            continue;
        };

        //Remove new line 
        line[strcspn(line , "\n")] = 0;
        //Skip empty lines 
        if (strlen(line) == 0) {
            continue;
        }

        //Check for exit command
        if (strcmp(line, "exit") == 0) {
            exit(0);
        };

        //Separate line into arguments (Ex: "ls -l -a" -> args[0] = "ls", args[1] = "-l", args[2] = "-a", args[3] = NULL)

        int argc = 0; //argument counter
        char *token = strtok(line, " \t");  //Split line into tokens using space or tab as delimiters
        while (token != NULL && argc < MAX_ARGS - 1) {
            args[argc++] = token; //Store the current word (token) into args array
            token = strtok(NULL, " \t"); //Get the next word (token)
        };
        args[argc] = NULL; //Puts NULL at the end of args array 

        //Add cd <directory> command
        if (strcmp(args[0], "cd") == 0) {
            if (argc < 2) {
                fprintf(stderr, "cd: missing argument\n");
            } else {
                if (chdir(args[1]) == -1) {
                    perror("cd failed");
                }
            }
            continue;
        }

        //Add mkdir <directory> command
        if (strcmp(args[0], "mkdir") == 0) {
            if (argc < 2) {
                fprintf(stderr, "mkdir: missing argument\n");
            } else {
                if (mkdir(args[1], 0777) == -1) {
                    perror("mkdir failed");
                }
            }
            continue;
        }

        //add background execution with & at the end
        int background = 0;
        if (argc > 0 && strcmp(args[argc -1], "&") == 0) {
            background = 1;
            args[argc -1] = NULL; //Remove & from args because it isnt a command to be executed by execvp
        }

        //Create pipe
        int pipe_fromchld_to_par[2];
        int pipe_frompar_to_chld[2];
        pipe(pipe_fromchld_to_par);
        pipe(pipe_frompar_to_chld);

        //Fork 
        pid_t pid = fork();

        if (pid == -1) {
            perror("fork failed");
            continue;
        }

        if (pid == 0) {

            //close unused pipe ends in child
            close(pipe_fromchld_to_par[0]); //close the read because child writes for parent in this pipe
            close(pipe_frompar_to_chld[1]); //close the write because child reads from parent in this pipe

            dup2(pipe_fromchld_to_par[1], STDOUT_FILENO); //redirect stdout to pipe to parent (the child is going to write not in the terminal but in the pipe for the parent)
            dup2(pipe_frompar_to_chld[0], STDIN_FILENO); //redirect stdin to pipe from parent (the child is going to read not from the terminal but from the pipe from the parent)

            //close all the ends
            close(pipe_fromchld_to_par[1]);
            close(pipe_frompar_to_chld[0]);

            //Execute command
            execvp(args[0], args);
            //If execvp returns, there was an error
            perror("execvp failed");
            exit(EXIT_FAILURE);
        }
        else {
            //Parent process

            close(pipe_fromchld_to_par[1]); //close the write because parent reads from child in this pipe
            close(pipe_frompar_to_chld[0]); //close the read because parent writes for child in this pipe

            //Read from child process
            int n;
            char buffer[MAX_LINE - 1];
            n = read(pipe_fromchld_to_par[0], buffer, MAX_LINE - 1); //Read from child process and saves it in n 
            while (n > 0) {
                buffer[n] = '\0'; //Null terminate the string
                printf("%s", buffer); //Print what was read from child process
                n = read(pipe_fromchld_to_par[0], buffer, MAX_LINE - 1); //Read again
            }

            //Close all the ends
            close(pipe_fromchld_to_par[0]);
            close(pipe_frompar_to_chld[1]);

            //Wait for child process to finish
            int status;
            if (background == 0) {
                waitpid(pid, &status, 0); //Wait for the child process to finish
                if  (WIFEXITED(status)){
                    //Child exited normally
                    int exit_status = WEXITSTATUS (status);
                    printf ("Child exited with status %d\n", exit_status);
                }
            }else{
                printf("Background process %d \n", pid);    
            }
        }
    }

    printf("Shell terminated.\n");
    return EXIT_SUCCESS;
}
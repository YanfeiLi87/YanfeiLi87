#define _GNU_SOURCE
#define _XOPEN_SOURCE

#include "myshell.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>



char *prompt;

// array of special characters
int symbols[] = { PIPE, LEFT, RIGHT };



// list of built-in commands
char *builtin_cmd[] = { "cd", "clr", "echo", "environ", "path", "help", "pause", "quit" };

// pointers to built-in functions that takes char** and returns an int
int (*builtin_func[]) (char **) = {
    &shell_cd,
    &shell_clr,
    &shell_echo,
    &shell_environ,
    &shell_path,
    &shell_help,
    &shell_pause,
    &shell_quit
};



//  MAIN SHELL LOOP
int main(void) {

    char *line;
    char **args;
    int status;

    do {

        prompt = getcwd(prompt, BUF_SIZE);

        // print the prompt
        printf("%s Type your prompt here - ", prompt);

        // read and get commands from the prompt
        line = readLine();
        args = splitLine(line);

        // update status
        status = shell_execute(args);

        free(line);
        free(args);

    } while(status);


}


int numBuiltins() {

    return sizeof(builtin_cmd) / sizeof(char *);

}


// read line
char *readLine() {

    char *line = NULL;
    size_t len = 0;
    getline(&line, &len, stdin);
    return line;

}


// split line to get command name and arguments
char **splitLine(char *line) {

    char **tokens;
    char *tok;
    int i = 0;


    if ((tokens = malloc(sizeof(char*) * TOK_BUFSIZE)) == NULL)
        exit(EXIT_FAILURE);

    tok = strtok(line, TOK_DELIM);

    while (tok != NULL) {

        tokens[i] = tok;
        i++;
        tok = strtok(NULL, TOK_DELIM);
    }

    tokens[i] = NULL;
    return tokens;
}


pid_t Fork(void) {
    pid_t pid;

    if ((pid = fork()) < 0) {

        fprintf(stderr, "fork error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    return pid;
}


// launch a process
int procLaunch(char **args) {

    pid_t pid, wpid;
    int status;

    // background flag
    int flag = FALSE;


    char procName[strlen(args[0])];
    memset(procName, '\0', sizeof(procName));


    // check if there is '&' for background process
    if (args[0][strlen(args[0]) - 1] == '&') {

        strncpy(procName, args[0], strlen(args[0]) - 1);
        flag = TRUE;

    } else {

        strcpy(procName, args[0]);
    }



    if ( (pid = Fork()) == 0) {

        if (execvp(procName, args) < 0) {

            fprintf(stderr, "Error: Command not found: %s\n", args[0]);
            exit(EXIT_FAILURE);
        }

        exit(-1);

    } else {            //parent

         // no background process --> have to wait for child process
        if (!flag) {

            do {
                wpid = waitpid(pid, &status, WUNTRACED);

            } while (!WIFEXITED(status) && !WIFSIGNALED(status));

        }
    }

    return 1;
}


// shell execute
int shell_execute(char **args) {

    if (args[0] == NULL) {               // empty command

        return 1;

    }

    // find special character and its position
    int symbol = detectSymbol(args);
    int symPos = detectSymbolPos(args);

    // in the event there is no pipe or redirection
    if (symbol < 0 || symPos < 0) {

        for (int i = 0; i < numBuiltins(); i++) {

            if (strcmp(args[0], builtin_cmd[i]) == 0) {
                // this is a built in command
                return (*builtin_func[i])(args);
            }
        }
        return procLaunch(args);


    // there is pipe and redirection
    } else {

        // length of args array
        int len = args_len(args);
        // args to the left
        char *args_left[symPos + 1];
        // args to the right
        char *args_right[len - symPos];


        // copy arguments on the left side
        int i = 0;
        for (; i < symPos; i++)
            args_left[i] = args[i];

        args_left[i++] = NULL;


        // copy arguments on the right side
        int j = 0;
        for (; args[i] != NULL; i++, j++)
            args_right[j] = args[i];

        args_right[j] = NULL;


        // if the special character is a PIPE
        if (symbols[symbol] == PIPE)

            return invokePipe(args_left, args_right);

        else if (symbols[symbol] == LEFT)

            return redirect(args_left, args_right, 0);          // < or left direction

        else

            return redirect(args_left, args_right, 1);          // > or right direction
    }

    return 1;
}


int invokePipe(char **args1, char **args2) {

    pid_t pid1, pid2;
    int status1, status2;
    int fd[2];              // file descriptor with read end and write end



    if (pipe(fd) == -1) {
        fprintf(stderr, "pipe error\n");
        return EXIT_FAILURE;
    }


    // first look through the list of built-in commands to see if 1st argument is a built-in func

    for (int i = 0; i < numBuiltins(); i++)

        if (strcmp(args1[0], builtin_cmd[i]) == 0) {        // check if args1 is a builtin


            if ( (pid1 = Fork()) == 0) {

                // child process

                close(fd[WRITE]);
                dup2(fd[READ], STDIN_FILENO);

                if (execvp(args2[0], args2) < 0) {
                    fprintf(stderr, "Error: command not found: %s\n", strerror(errno));
                    exit(EXIT_FAILURE);
                }

            } else {

                if ( (pid2 = Fork()) == 0) {

                close(fd[READ]);
                dup2(fd[WRITE], STDOUT_FILENO);

                (*builtin_func[i])(args1);
                waitpid(pid1, &status1, WUNTRACED);

                }

                return 1;
            }
        }



    // if args1 is not built in

    pid1 = Fork();

    if (pid1 == 0) {

        // send input to pipe
        close(fd[READ]);
        dup2(fd[WRITE], STDOUT_FILENO);
        return procLaunch(args1);

    } else {

        pid2 = Fork();
        if (pid2 == 0) {

            close(fd[WRITE]);
            dup2(fd[READ], STDIN_FILENO);

            return procLaunch(args2);

        } else {

            close(fd[WRITE]);
            close(fd[READ]);

            waitpid(pid1, &status1, WUNTRACED);
            waitpid(pid2, &status2, WUNTRACED);

            return 1;
        }
    }
}



int redirect(char **args1, char **args2, int direction) {

    pid_t pid;
    int status;
    int fd;
    char *fileName;
    fileName = args2[0];

    if (direction == 0) {                                       // <, file serves as source of input

        if ((fd = open(fileName, O_RDONLY, 0755)) == -1) {

            fprintf(stderr, "Error: cannot find such file or directory: %s\n", strerror(errno));

            return EXIT_FAILURE;
        }

        dup2(fd, STDIN_FILENO);

    } else {                                                    // >, file serves as target of output

        if ((fd = open(fileName, O_RDWR | O_CREAT | O_TRUNC, 0755)) == -1) {

            fprintf(stderr, "Error opening or creating file: %s\n", strerror(errno));

            return EXIT_FAILURE;
        }

        dup2(fd, STDOUT_FILENO);
    }

    return 1;
}


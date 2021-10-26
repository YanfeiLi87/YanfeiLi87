
#define _GNU_SOURCE
#define _XOPEN_SOURCE


#include "myshell.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>



extern char **environ;      //global variable


// Most built-in commands return a value of 1 to indicate a successfull execution - with assumption of user adhering to the rules of CLI prompt


int args_len(char **args) {

    int i = 0;                              //counting variable, will go through the args array and increment.

    for (i = 0; args[i] != NULL; i++) {}

    return i;

}


int shell_quit(char **args) {

    return 0;
}


int shell_help(char **args) {

    char *man[] = {"more", "readme", NULL};

    return procLaunch(man);

}


int shell_clr(char **args) {

    system("clear");
    return 1;

}


int shell_pause(char **args) {

    printf("The shell has been paused. Press 'Enter' to resume...");

    //while loop waiting for Enter (\n) to be pressed to end. when it ends, return successfull execution
    while (getchar() != '\n');

    return 1;
}


int shell_cd(char **args) {

    // if no argument --> go to $HOME

    if (args[1] == NULL) {

        char *home;
        if ((home = getenv("HOME")) == NULL)
            fprintf(stderr, "Error: Cannot get $HOME");
        else {
            if (chdir(home) == -1)
                fprintf(stderr, "cd: %s\n", strerror(errno));
        }

    } else {

        if (chdir(args[1]) == -1)
            fprintf(stderr, "cd: %s: %s\n", strerror(errno), args[1]);
    }

    return 1;
}


int shell_echo(char **args) {

    if (args[1] == NULL)
        fprintf(stdout, "\n");  //print nothing

    else {

        //print what's in the string arguments
        for (int i = 1; args[i] != NULL; i++)
            printf("%s ", args[i]);

        printf("\n");

    }
    return 1;
}


int shell_environ(char **args) {

    // run through the whole array of environ strings and print out each
    for (int i = 0; environ[i] != NULL; i++)
        printf("%s\n", environ[i]);

    return 1;
}



int shell_path(char **args) {

    if (args[1] == NULL) {                  // no 2nd argument --> works like environ

        char *env[] = {"env", NULL};

        return procLaunch(env);

    } else {

        if (putenv(args[1]) != 0) {

            fprintf(stderr, "Error: Cannot export environment");

        }
    }
    return 1;

}



int detectSymbol(char **args) {

    /*
     | : return 0
     < : return 1
     > : return 2

     no symbol or empty command: return -1
    */

    // command is empty
    if (args[0] == NULL)
        return -1;

    for (int i = 0; args[i] != NULL; i++) {

        if (args[i][0] == PIPE)             // |
            return 0;

        if (args[i][0] == LEFT)             // <
            return 1;

        if (args[i][0] == RIGHT)            // >
            return 2;
    }

    return -1;
}


// return the position of the special character
int detectSymbolPos(char **args) {

    // command is empty
    if (args[0] == NULL)
        return -1;

    for (int i = 0; args[i] != NULL; i++) {

        if (args[i][0] == PIPE || args[i][0] == LEFT || args[i][0] == RIGHT)

            return i;

    }
    // return -1 if nothing found
    return -1;
}

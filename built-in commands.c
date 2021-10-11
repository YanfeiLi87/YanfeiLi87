/*
clr - Clear the screen.
dir <directory> - List the contents of directory <directory>.
environ - List all the environment strings.
echo <comment> - Display <comment> on the display followed by a new line (multiple spaces/tabs may be reduced to a single space).
help - Display the user manual using the more filter.
pause - Pause operation of the shell until 'Enter' is pressed.
quit - Quit the shell.
The shell environment should contain shell=<pathname>/myshell where <pathname>/myshell is the full path for the shell executable(not a hardwired path back to your directory, but the one from which it was executed).*/
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
//#include "myshell.h"

#define PROMPT_MYSHELL "myshell"
#define MAX_STRINGSIZE 1024
#define MAX_ARG_LENGTH 64
#define MAX_NUM_OF_ARGUMENTS 64

typedef struct StrCmd{
    int iCmdID;
    int iCmdType;
    char* sInputFile;
    char* sOutputFile;
    char** args;
    int isRedirInputFile;
    int isRedirOutputFile;
    int isBackgroundRunning;
    int isOutputTruncated;
} StrCmd;


// build in function
int Process_CD (StrCmd *cmd);
int Process_CLR ();
int Process_DIR (StrCmd *cmd);
int Process_ENVIORN (StrCmd *cmd);
int Process_ECHO (StrCmd *cmd);
int Process_HELP (StrCmd *cmd);
int Process_PAUSE ();
int Process_QUIT ();
int Process_test(); 


//buildin function - CD
int Process_CD (StrCmd *cmd){
    int num_of_args = GetArgsCount(cmd->args);

    if (num_of_args  > 1){
        ShowError();
        return 1;
    }
    else if (num_of_args == 0){
        char cwd[MAX_STRINGSIZE];
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            printf("%s\n", cwd);
        }
    }
    else{
        if (chdir(cmd->args[num_of_args])){
            ShowError();
            printf("\n");
            return 1;
        }
    }
    return EXIT_SUCCESS;
}

//buildin function - clr
int Process_CLR (){
    printf("\033[H\033[J");
    return EXIT_SUCCESS;
}

//buildin function - dir
int Process_DIR (StrCmd *cmd){
    FILE * outFileFp=NULL;
    int num_of_args = GetArgsCount(cmd->args);

    struct dirent *de;  //directory entry

    if (cmd->isRedirOutputFile){
        char max_file_path[100] = {0};
        GetAbsPath(max_file_path, cmd->sOutputFile);
        outFileFp=freopen(max_file_path, cmd->isOutputTruncated ==  1 ? "w": "a",stdout);
    }

    DIR *dr;
    if (num_of_args == 0){
        dr = opendir(".");
    }
    else{
        dr = opendir(cmd->args[num_of_args]);
    }

    // opendir returns NULL if couldn't open directory
    if (dr == NULL){
        ShowError();
        return 1;
    }

    while ((de = readdir(dr)) != NULL)
        printf("%s\n", de->d_name);

    closedir(dr);

    if(outFileFp){
        fclose(outFileFp);
        freopen("/dev/tty","w",stdout);
    }

    return EXIT_SUCCESS;
}

//buildin function - enviorn
int Process_ENVIORN (StrCmd *cmd){
    FILE * outFileFp=NULL;
    int i = 0;

    if (cmd->isRedirOutputFile){
        char max_file_path[100] = {0};
        GetAbsPath(max_file_path, cmd->sOutputFile);
        outFileFp=freopen(max_file_path, cmd->isOutputTruncated ==  1 ? "w": "a",stdout);
    }

    while(__environ[i]) {
        printf("%s\n", __environ[i++]); 
    }

    if(outFileFp){
        fclose(outFileFp);
        freopen("/dev/tty","w",stdout);
    }
    return EXIT_SUCCESS;
}

//buildin function - echo
int Process_ECHO (StrCmd *cmd){
    int num_of_args;
    FILE * outFileFp=NULL;

    if (cmd->isRedirOutputFile){
        char max_file_path[100] = {0};
        GetAbsPath(max_file_path, cmd->sOutputFile);
        outFileFp=freopen(max_file_path, cmd->isOutputTruncated ==  1 ? "w": "a",stdout);
    }

    for (num_of_args = 1; ((num_of_args < MAX_NUM_OF_ARGUMENTS) && (cmd->args[num_of_args] != NULL)); num_of_args++){
        printf("%s \t",cmd->args[num_of_args]);
    }
    
    
    printf("\n");

    if(outFileFp){
        fclose(outFileFp);
        freopen("/dev/tty","w",stdout);
    }
    return 1;
}

//buildin function - help
int Process_HELP (StrCmd *cmd){
    FILE * outFileFp=NULL;
    if (cmd->isRedirOutputFile){
        char max_file_path[100] = {0};
        GetAbsPath(max_file_path, cmd->sOutputFile);
        outFileFp=freopen(max_file_path, cmd->isOutputTruncated ==  1 ? "w": "a",stdout);
    }

    if (fork() == 0){
        char * const help[] = { "more", "readme", NULL };
        if (execvp(help[0], help) == -1){
            exit(EXIT_FAILURE);
        }
    }
    if(outFileFp){
        fclose(outFileFp);
        freopen("/dev/tty","w",stdout);
    }

    return EXIT_SUCCESS;
}

//buildin function - pause
int Process_PAUSE (){
    getpass(" Myshell is paused. \n Press <ENTER> key to continue.");
    return EXIT_SUCCESS;
}

//buildin function - quit
int Process_QUIT (){
    exit(0);
}

int main(int argc, char **argv){
    char * defaultPath = "/bin";
    char shell_path[MAX_STRINGSIZE]="shell=";
    strcat(shell_path, getenv("PWD"));
    strcat(shell_path,"/myshell");
    putenv(shell_path); //add the working directory 
    setenv("PATH",defaultPath,1);// add the current working directory  in the "PATH"  
    
    //Interactive mode or Script Mode
    if (argc == 1)
        
        InteractiveMode();
    else if (argc == 2)
        BatchMode(argv[1]);
    else
        ShowError();

    return EXIT_SUCCESS;
}

#ifndef SHELL_H
#define SHELL_H

#define BUF_SIZE 200
#define TOK_BUFSIZE 128
#define TOK_DELIM " \t\r\n\a"


#define FALSE 0
#define TRUE 1

#define READ 0
#define WRITE 1


#define RIGHT '>'
#define PIPE '|'
#define LEFT '<'
#define AMP '&'

#include <sys/types.h>
#include <unistd.h>




char *readLine();                           // read line

char **splitLine(char *);                   // split line

int procLaunch(char **);                    // process launch

int shell_execute(char **);                  // shell execute

int invokePipe(char **, char **);           // invoke pipe

int redirect(char **, char **, int);        // redirection

int detectSymbol(char **);                  // detect symbol

int detectSymbolPos(char **);               // detect symbol with position

pid_t Fork(void);                           // Fork wrapper


int args_len(char **);                      // len of args array



//BUILT-IN COMMANDS


int numBuiltins();                          // print the number of built-in functions

int shell_pause(char **);                   // pause command

int shell_help(char **);                    // help command

int shell_quit(char **);                    // quit command

int shell_clr(char **);                     // clear command

int shell_cd(char **);                      // cd command

int shell_echo(char **);                    // echo command

int shell_environ(char **);                 // environ command

int shell_path(char **);                    // path command




#endif

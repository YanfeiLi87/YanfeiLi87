//this program is designed for project 0 PartA 
//by Yanfei Li
//last edited on 09/01/2021

#include <stdio.h>
#include <stdlib.h>


int main(int argc, char *argv[])
{
    FILE *fp;									//create filepointer
    char  buffer[100]; 							//create buffer
    while(--argc > 0){							//while there is argument(s)
        if ((fp=fopen(*++argv, "r")) == NULL)	//if filepointer has nothing
        {
            printf("wcat: cannot open file\n");	//print error message.
            return 1;
        }
        else
        {
            while(fgets(buffer,100,fp) != NULL)	//or while there is something in the filepointer
			{
		  	 printf("%s",buffer);				//print out the contents
		   	}
            
        }
        fclose(fp);								//close the file
    }
    if(fp == NULL){								//this error message is for in the case of not type in any file.
		printf("no filename was entered\n");
	}
    return 0;
}
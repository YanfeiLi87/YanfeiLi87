//this is a program designed for project 0 part B
//by Yanfei Li
//last edited at 9/7/2021
#include <stdio.h>
#include <string.h>
#include <stdlib.h>//gcc -o wzip wzip.c -Wall -Werror

FILE* fp=NULL;
char str[100],ch,in;
int Counter=1;
size_t size;
int main(int argc, char* argv[]){
   switch (argc)
   {
        case 1:                                       //if user only type in wzip without file names
            printf("wzip:[file1] [file2...]\n");       //error pops up
            break;
            
        default:
            while(--argc > 0)
            {                            //while there is argument(s)
                if ((fp=fopen(*++argv, "r")) == NULL)	//if filepointer has nothing
                {
                    printf("wzip: cannot open file\n");	//print error message.
                    return 1;
                }
                else
                {
                    while(fgets(str,100,fp) != NULL)	//or while there is something in the filepointer
                    {
                        // c = str[0];
                        for(int i=0;i<sizeof(str);i++)//in the for loop of whole file
                        {
                            if(str[i]==str[i+1])// if first char is the same as the second char
                            {
                                Counter++;//increase the counter
                                in=Counter+'0';
                                fwrite(&in,sizeof(char),1,stdout);
                                return(0);
                            }
                            else//else write the char to the compressed file.
                            {
                                //what do i need? all the arguments in fwrite()
                                //what is it? 1.char array 2.size of ele 3.# of ele 4.fp
                                //what stop me? 
                                //passing the value of int(checked) and value of a element of a char array to char array
                                // in=Counter+'0';

                                // strncpy(temp,str[i]);
                                // printf("%c\n",str[i]);
                                fwrite(&str[i],sizeof(char),1,stdout);
                                printf("\n");
                                return(0);
                                // fwrite(&in,sizeof(char),1,stdout);
                            }
                            // printf("%c",str[i]);
                        }
                    }
                }
                fclose(fp);								//close the file
            }
        break;
   }
   return(0);
}
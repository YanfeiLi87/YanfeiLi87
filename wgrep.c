//this program is designed for project 0 PartA 
//by Yanfei Li
//last edited on 09/01/2021

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main(int argc, char *argv[])
{
    FILE *fp;												//create a filepointer
    char String[100];										//create a string
    size_t length=100;										//create a length
    int total;												//create a total characters
    int Counter=2;											//create a counter
	char *targetline=NULL;
	switch(argc){
		case 1:											//if missing keyword and filename
		printf("please enter the keyword and filename\n");	//error message pops up
    	exit(1);
		case 2:											//if missing filename
		printf("please enter the filename\n");				//error message pops up
		if(fgets(String,100,stdin)!=NULL){
			printf("entered filename: %s",String);
			exit(1);
		}
		default:											//if nothing was missing
		while(--argc>1){									//before reaching the last file
			fp=fopen(argv[Counter], "r");	
	        if (fp == NULL){								//if couldn't find the file
	            printf("Can't open file\n");				//error message pops up
	            exit(1);
	        }
	        else{											//if there is a file
	        	while((total=getline(&targetline, &length,fp))!= -1){//checking for if any content in the file
	        		if(strstr(targetline, argv[1])!=NULL){ 	//looking for the key word, from the target file
	        			printf("%s\n",targetline);			//print the targetline
	        		}
	        	}
			}
			Counter++;										//increasing counter for switching to the next file
		}
		fclose(fp);											//close the file
		exit(0);
	}
}
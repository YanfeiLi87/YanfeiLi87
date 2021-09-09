//this is a program designed for project 0 part B
//by Yanfei Li
//last edited at 9/8/2021
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

int main(int argc, char *argv[]){
	
	FILE *fp;
	char currentChar;
	char lastChar;
	int fileCounter = 1;
	int count;

	if(argc == 1){
		printf("wunzip: [FILE1][FILE2]...\n");
	}else{
		while(--argc > 0){
			fp = fopen(argv[fileCounter],"r");
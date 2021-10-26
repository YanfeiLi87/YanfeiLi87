CC=gcc
CFLAGS=-g -std=c99

myshell: myshell.c utilities.c myshell.h
	$(CC) $(CFLAGS) -o myshell myshell.c utilities.c

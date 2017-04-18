/**************************************************
 *
 * File:            fat32.c
 *
 * Author:          Maxine Fehr
 * Student ID:      6854954
 *
 * Course:          COMP 3430: Operating Systems
 * Assignment:      Assignment #3
 * Due Date:        April 7, 2017 
 *
 **************************************************/

#define _FILE_OFFSET_BITS 64

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include "fat32.h" 
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#define BUFSIZE 81
#define MAX_STRING 100

/*
 * Function:    cdFcn
 * Return:      void
 */
void cdFcn(char *dir) {
    printf("change directory to %s\n", dir);
}

/*
 * Function:    dirFcn 
 * Return:      void
 */
void dirFcn() {
    printf("\nDIRECTORY LISTING\n\n---DONE\n");
}

/*
 * Function:    getFcn
 * Return:      void
 */
void getFcn(char *filename) {
    printf("getting file %s\n\nDone.\n", filename);
}

/*
 * Function:    infoFcn
 * Return:      void
 */
void infoFcn() {
    printf("\n---- Device Info ----\n\n--- Geometry ---\n\n--- FS Info ---\n");
}

/*
 * Function:    processInput 
 * Return:      void
 */
void processInput(char *userInput) {
    char *tokenArray[MAX_STRING];
    int len;
    int tokens;

    len = strlen(userInput);
    if (userInput[len-1] == '\n')
        userInput[len-1] = '\0';

    tokens = tokenizeString(tokenArray, userInput, " ");
    if (0 == strcmp(tokenArray[0], "cd") && 2 == tokens)
        cdFcn(tokenArray[1]);
    else if (0 == strcmp(tokenArray[0], "dir") && 1 == tokens)
        dirFcn();
    else if (0 == strcmp(tokenArray[0], "get") && 2 == tokens)
        getFcn(tokenArray[1]);
    else if (0 == strcmp(tokenArray[0], "info") && 1 == tokens)
        infoFcn();
    else
        printf("Command not found\n");
}

/*
 * Function:    startShell
 * Return:      void
 */
void startShell(char *device){
    char buffer[BUFSIZE];       /* room for 80 chars plus \0 */
    char *userInput;            /* pointer to entered command */
    char prompt = '>';
    int fd = -1;
    fat32BS *sector0 = malloc(sizeof(fat32BS));
  
    /* open file here, exiting on failure */
    if ((fd = open(device, O_RDONLY)) == -1) {
        printf("Device '%s' not found\n", device);
        exit(0);
    }
    if ((read(fd, sector0, sizeof(fat32BS))) == -1) {
        printf("Read error\n");
        /* print info regarding error */
        exit(0);
    }
 
    printf("Reading from device: %s\n", device); /* Do some error checking to make sure device is valid */
    printf("%c", prompt);
    userInput = fgets(buffer, BUFSIZE, stdin);

    while (userInput != NULL) {
      processInput(userInput);
      printf("%c", prompt);
      userInput = fgets(buffer, BUFSIZE, stdin);
    }
    close(fd);
    printf("\nExited..\n");
}

/* 
 * Function:    tokenizeString
 * Purpose:     Tokenize a string based on the delimiter provided.
 * Return:      number of tokens (int)
 */
int tokenizeString(char **array, char *string, char *delim) {
    char *token;
    int count = 0;

    while((token = strtok_r(string, delim, &string))) {
        array[count] = token;
        count++;
    }
    return count;
}

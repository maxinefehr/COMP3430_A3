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

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include "fat32.h" 

#define BUFSIZE 81
#define MAX_STRING 100

/*
 * Function:    cdFcn
 * Return:      void
 */
void cdFcn(char *input) {
    printf("cdnFcn: User entered %s\n", input);
}

/*
 * Function:    dirFcn 
 * Return:      void
 */
void dirFcn(char *input) {
    printf("dirFcn: User entered %s\n", input);
}

/*
 * Function:    getFcn
 * Return:      void
 */
void getFcn(char *input) {
    printf("getFcn: User entered %s\n", input);
}

/*
 * Function:    infoFcn
 * Return:      void
 */
void infoFcn(char *input) {
    printf("infoFcn: User entered %s\n", input);
}

/*
 * Function:    processInput 
 * Return:      void
 */
void processInput(char *userInput) {
    int len;
    len = strlen(userInput);
    if (userInput[len-1] == '\n')
        userInput[len-1] = '\0';

    if (0 == strcmp(userInput, "cd"))
        cdFcn(userInput);
    else if (0 == strcmp(userInput, "dir"))
        dirFcn(userInput);
    else if (0 == strcmp(userInput, "get"))
        getFcn(userInput);
    else if (0 == strcmp(userInput, "info"))
        infoFcn(userInput);
    else
        printf("Command not found\n");
}

/*
 * Function:    startShell
 * Return:      void
 */
void startShell(){
    char buffer[BUFSIZE];       /* room for 80 chars plus \0 */
    char *userInput;            /* pointer to entered command */
    char prompt = '>';
   
    printf("%c", prompt);
    userInput = fgets(buffer, BUFSIZE, stdin);

    while (userInput != NULL) {
      processInput(userInput);
      printf("%c", prompt);
      userInput = fgets(buffer, BUFSIZE, stdin);
    }
    printf("Exited..\n");
}

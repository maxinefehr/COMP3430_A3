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
#define HARD_DISK_NUM 128
#define FLOPPY_DISK_NUM 0
#define MEDIA_FIXED 248
#define MEDIA_REMOVABLE 240
#define BSSIG_A 85
#define BSSIG_B 170
#define MB_DIV 1000000
#define GB_DIV 1000000000
#define MIRRORED 0
#define ZERO 0
 
fat32BS *sector0;
int fd;

/*
 * Function:    exitFCN 
 * Return:      void
 */
void exitFcn(char *msg) {
    close(fd);
    printf("%s\n", msg);
    exit(0);
}

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
void infoFcn(fat32BS *bpb) {
    int i;
    long long llBytesPerSec = (long long)bpb->BPB_BytesPerSec;
    long long llTotalSec = (long long)bpb->BPB_TotSec32;
    long long llTotalSize = llBytesPerSec * llTotalSec;

    /* DEVICE INFO */
    printf("---- Device Info ----\n");
    printf("OEM Name: ");
    for (i = 0; i < BS_OEMName_LENGTH; i++) {
        printf("%c", bpb->BS_OEMName[i]);
    } 
    printf("\nLabel: ");    
    for (i = 0; i < BS_OEMName_LENGTH; i++) {
        printf("%c", bpb->BS_VolLab[i]);
    }
    printf("\nFile System Type: "); 
    for (i = 0; i < BS_FilSysType_LENGTH; i++) {
        printf("%c", bpb->BS_FilSysType[i]);
    }
    printf("\nMedia Type: ");
    if (bpb->BPB_Media == MEDIA_FIXED)
        printf("0x%02X (fixed)\n", bpb->BPB_Media);
    if (bpb->BPB_Media == MEDIA_REMOVABLE)
        printf("0x%02X (removable)\n", bpb->BPB_Media);
    printf("Size: %lld bytes (%lldMB, %.3fGB)\n", llTotalSize, llTotalSize/MB_DIV, (double)llTotalSize/GB_DIV);
    printf("Drive Number: ");
    if (bpb->BS_DrvNum == HARD_DISK_NUM)
        printf("%d (hard disk)\n",HARD_DISK_NUM );
    else if (bpb->BS_DrvNum == FLOPPY_DISK_NUM)
        printf("%d (floppy disk)\n", FLOPPY_DISK_NUM);

    /* GEOMETRY */
    printf("\n--- Geometry ---\n");
    printf("Bytes Per Sector: %d\n", bpb->BPB_BytesPerSec);    
    printf("Sectors Per Cluster: %d\n", bpb->BPB_SecPerClus);
    printf("Total Sectors: %d\n", bpb->BPB_TotSec32);
    printf("Geom: Sectors Per Track: %d\n", bpb->BPB_SecPerTrk  );
    printf("Geom: Heads: %d\n", bpb->BPB_NumHeads);
    printf("Hidden Sectors: %d\n", bpb->BPB_HiddSec);

    /* FS Info */
    printf("\n--- FS Info ---\n");
    printf("Volume ID: %d\n", bpb->BS_VolID);
    printf("Version: %d:%d\n", bpb->BPB_FSVerHigh, bpb->BPB_FSVerLow);
    printf("Reserved Sectors: %d\n", bpb->BPB_RsvdSecCnt);
    printf("Number of FATs: %d\n", bpb->BPB_NumFATs);
    printf("Mirrored FAT: ");
    if (bpb->BPB_ExtFlags == MIRRORED)
        printf("0 (yes)\n");
    else
        printf("1 (no)\n");
    printf("Boot Sector Backup Sector No: %d\n", bpb->BPB_BkBootSec);
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
        infoFcn(sector0);
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
    fd = -1;
  
    /* open file here, exiting on failure */
    if ((fd = open(device, O_RDONLY)) == -1) {
        exitFcn("Device not found");
    }
    sector0 = malloc(sizeof(fat32BS));
    if ((read(fd, sector0, sizeof(fat32BS))) == -1) {
        exitFcn("Read error");
    }
    if ((sector0->BS_SigA != BSSIG_A) || (sector0->BS_SigB != BSSIG_B)) {
        exitFcn("Boot sector sig btyes are incorrect.");
    }       
    if ((sector0->BPB_TotSec16 != ZERO) || (sector0->BPB_FATSz16 != ZERO)) {
        exitFcn("FAT16 Descriptors are not 0.");
    }  
    if (( (sector0->BPB_RootEntCnt * 32) + (sector0->BPB_BytesPerSec - 1) ) / (sector0->BPB_BytesPerSec) != ZERO) {
        exitFcn("FAT Type Determination: not a FAT32 volume.");
    }
        
    printf("Reading from device: %s\n", device);
    printf("%c", prompt);
    userInput = fgets(buffer, BUFSIZE, stdin);

    while (userInput != NULL) {
      processInput(userInput);
      printf("%c", prompt);
      userInput = fgets(buffer, BUFSIZE, stdin);
    }

    exitFcn("\nExited..");
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

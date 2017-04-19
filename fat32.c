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

#define BUFSIZE             81
#define MAX_STRING          100
#define HARD_DISK_NUM       128
#define FLOPPY_DISK_NUM     0
#define MEDIA_FIXED         248
#define MEDIA_REMOVABLE     240
#define BSSIG_A             85
#define BSSIG_B             170
#define MB_DIV              1000000
#define GB_DIV              1000000000
#define MIRRORED            0
#define ZERO                0
#define FAT32_MIN_CLUS      65525
 
#define FAT_ENT_PER_SEC     128 
#define EOC_MARK32          0x0FFFFFFF
#define BAD_CLUS32          0x0FFFFFF7

uint64_t rootDirSectors;
uint64_t dataSec;
uint64_t firstDataSector;
uint64_t firstFATSector;
uint64_t countOfClusters;
uint32_t FATSz;
uint16_t cluster_size_in_bytes;
uint16_t sector_size_in_bytes;
fat32BS *sector0;
int *FATSecPtr;
int fd;
fat32FSInfo *FSInfo; 
fat32Dir *CurrentDir;
fat32Dir *RootDir;
uint8_t *RootVolName;

/*
 * Function:    exitFCN 
 * Return:      void
 */
void exitFcn(char *msg) {
    free(FATSecPtr);
    free(sector0);
    free(FSInfo);
    free(RootDir);
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
    printf("\nDIRECTORY LISTING\n");
    printf("VOL_ID: %s\n\n", CurrentDir->DIR_Name);

    printf("Bytes Free: \n");
    printf("---DONE\n");
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
    uint64_t u64TotalSize = (uint64_t)bpb->BPB_BytesPerSec * (uint64_t)bpb->BPB_TotSec32;

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
    printf("Size: %"PRIu64" bytes (%"PRIu64"MB, %.3fGB)\n", u64TotalSize, u64TotalSize/MB_DIV, (double)u64TotalSize/GB_DIV);
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
    printf("Volume ID: %s\n", RootVolName);
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
 * Function:    mapDrive 
 * Return:      void
 */
void mapDrive() {
//    uint8_t cluster[cluster_size_in_bytes];
    fat32Dir *dir = malloc(sizeof(fat32Dir));
    uint64_t i;    
    uint64_t offset = firstDataSector + cluster_size_in_bytes;
    lseek(fd, offset, SEEK_SET);
    for (i = 1; i < countOfClusters; i++){
        if ((read(fd, dir, sizeof(fat32Dir))) == -1) {
            free(dir);
            exitFcn("Cluster read error");
        }
        //printf("Cluster %"PRIu64" DIR_Name: %s\n", i, dir->DIR_Name);
        lseek(fd, cluster_size_in_bytes, SEEK_CUR);
    }
    free(dir);   
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

uint32_t readFAT(uint32_t cluster) {
    uint32_t sector = cluster / FAT_ENT_PER_SEC;
    uint16_t entry = cluster % FAT_ENT_PER_SEC;
    uint32_t returnVal;
    uint32_t mask = 0x0FFFFFFF; 
    lseek(fd, FATSecPtr[sector], SEEK_SET);
    fatSector *fatSec = malloc(sizeof(fatSector));
    if ((read(fd, fatSec, sizeof(fatSector))) == -1) {
        free(fatSec);
        exitFcn("Read error");
    }
    returnVal = fatSec->entry[entry];
    free(fatSec);
    return (returnVal & mask);
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
    int i;

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
    if (sector0->BPB_RootEntCnt != ZERO) {
        exitFcn("FAT Type Determination: not a FAT32 volume.");
    }
    firstFATSector = sector0->BPB_RsvdSecCnt;
    firstDataSector = sector0->BPB_RsvdSecCnt + (sector0->BPB_NumFATs * sector0->BPB_FATSz32) + rootDirSectors;     
    rootDirSectors = (((sector0->BPB_RootEntCnt * 32) + (sector0->BPB_BytesPerSec - 1) ) / (sector0->BPB_BytesPerSec));
    dataSec = sector0->BPB_TotSec32 - (sector0->BPB_RsvdSecCnt + (sector0->BPB_NumFATs * sector0->BPB_FATSz32) + rootDirSectors);     
    countOfClusters = dataSec/sector0->BPB_SecPerClus;
    if (countOfClusters >= FAT32_MIN_CLUS)
        printf("This is a FAT32 volume with %"PRIu64" clusters\n", countOfClusters);
    else
        exitFcn("Not a FAT32 volume");
    
    sector_size_in_bytes = sector0->BPB_BytesPerSec;
    cluster_size_in_bytes = sector_size_in_bytes * (uint16_t)sector0->BPB_SecPerClus;
    printf("Sector size in bytes: %u, cluster size in bytes: %u\n", sector_size_in_bytes, cluster_size_in_bytes); 

    FATSz = sector0->BPB_FATSz32;
    FATSecPtr = malloc(sizeof(int) * FATSz);
    for (i = 0; i < FATSz; i++) {
        FATSecPtr[i] = (sector0->BPB_RsvdSecCnt + i) * sector_size_in_bytes;
    }

    FSInfo = malloc(sizeof(fat32FSInfo));
    lseek(fd, sector0->BPB_FSInfo * sector_size_in_bytes, SEEK_SET);
    if ((read(fd, FSInfo, sizeof(fat32FSInfo))) == -1) {
        exitFcn("FSInfo: Read error");
    }
    printf("FSI_LeadSig: 0x%08X\n", FSInfo->FSI_LeadSig);
    printf("FSI_StrucSig: 0x%08X\n", FSInfo->FSI_StrucSig);
    printf("FSI_TrailSig: 0x%08X\n", FSInfo->FSI_TrailSig);
    for (i = 0; i < 3; i++) {
        printf("Entry for cluster %d: 0x%08X\n", i, readFAT((uint32_t)i));
    }
    //printf("Entry for cluster %d: 0x%08X\n", FATSz*128, readFAT((uint32_t)FATSz*128));
    

    /* Reading root dir */
    RootDir = malloc(sizeof(fat32Dir));
    lseek(fd, firstDataSector * sector_size_in_bytes, SEEK_SET);
    if ((read(fd, RootDir, sizeof(fat32Dir))) == -1) {
        exitFcn("FSInfo: Read error");
    }
    if (RootDir->DIR_Attr != 8)
        exitFcn("Root dir not successfully accessed\n");
    printf("DIR_Attr: 0x%02X\n", RootDir->DIR_Attr);
    printf("DIR_Name: %s\n", RootDir->DIR_Name);
    printf("DIR_FstClusHI: %d\n", RootDir->DIR_FstClusHI);
    printf("DIR_FstClusLO: %d\n", RootDir->DIR_FstClusLO);
    printf("DIR_FileSize: %d\n", RootDir->DIR_FileSize);
//    printf("DIR_Name: %s", RootDir->DIR_Name);
    RootVolName = RootDir->DIR_Name;    
    CurrentDir = RootDir;

    mapDrive();
    printf("\nReading from device: %s\n", device);
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

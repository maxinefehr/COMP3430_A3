/************************************************************
 *
 * Module:  fat32.h
 * Author:  Maxine Fehr
 * Date:    April 7, 2017
 *
 ***********************************************************/

#ifndef FAT32_H
#define FAT32_H

#include <inttypes.h>

/* boot sector constants */
#define BS_OEMName_LENGTH 8
#define BS_VolLab_LENGTH 11
#define BS_FilSysType_LENGTH 8 
#define FAT_ENTRIES_IN_SEC 128
#define DIR_NAME_LENGTH 11

#pragma pack(push)
#pragma pack(1)
struct fat32BS_struct {
    char BS_jmpBoot[3];
    char BS_OEMName[BS_OEMName_LENGTH];
    uint16_t BPB_BytesPerSec;
    uint8_t BPB_SecPerClus;
    uint16_t BPB_RsvdSecCnt;
    uint8_t BPB_NumFATs;
    uint16_t BPB_RootEntCnt;
    uint16_t BPB_TotSec16;
    uint8_t BPB_Media;
    uint16_t BPB_FATSz16;
    uint16_t BPB_SecPerTrk;
    uint16_t BPB_NumHeads;
    uint32_t BPB_HiddSec;
    uint32_t BPB_TotSec32;
    uint32_t BPB_FATSz32;
    uint16_t BPB_ExtFlags;
    uint8_t BPB_FSVerLow;
    uint8_t BPB_FSVerHigh;
    uint32_t BPB_RootClus;
    uint16_t BPB_FSInfo;
    uint16_t BPB_BkBootSec;
    char BPB_reserved[12];
    uint8_t BS_DrvNum;
    uint8_t BS_Reserved1;
    uint8_t BS_BootSig;
    uint32_t BS_VolID;
    char BS_VolLab[BS_VolLab_LENGTH];
    char BS_FilSysType[BS_FilSysType_LENGTH];
    char BS_CodeReserved[420];
    uint8_t BS_SigA;
    uint8_t BS_SigB;
};

struct fat32FSInfo_struct {
    uint32_t FSI_LeadSig;
    uint8_t FSI_Reserved1[480];
    uint32_t FSI_StrucSig;
    uint32_t FSI_Free_Count;
    uint32_t FSI_Nxt_Free;
    uint8_t FSI_Reserved2[12];
    uint32_t FSI_TrailSig;
};

struct fatSector_struct {
    uint32_t entry[FAT_ENTRIES_IN_SEC];
};

struct fat32Dir_struct {
    uint8_t DIR_Name[DIR_NAME_LENGTH];
    uint8_t DIR_Attr;
    uint8_t DIR_NTRes;
    uint8_t DIR_CrtTimeTenth;
    uint16_t DIR_CrtTime;
    uint16_t DIR_CrtDate;
    uint16_t DIR_LstAccDate;
    uint16_t DIR_FstClusHI;
    uint16_t DIR_WrtTime;
    uint16_t DIR_WrtDate;
    uint16_t DIR_FstClusLO;
    uint32_t DIR_FileSize;
};

#pragma pack(pop)

typedef struct fat32BS_struct fat32BS;
typedef struct fat32FSInfo_struct fat32FSInfo;
typedef struct fatSector_struct fatSector;
typedef struct fat32Dir_struct fat32Dir;

void exitFcn(char *);
void cdFcn(char *);
void dirFcn();
void getFcn(char *);
void infoFcn(fat32BS *);
void processInput(char *);
uint32_t readFAT(uint32_t);
void startShell(char *);
int tokenizeString(char **, char *, char *);

#endif

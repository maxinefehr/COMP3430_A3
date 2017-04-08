/************************************************************
 *
 * Module:  fat32.h
 * Author:  Maxine Fehr
 * Date:    April 7, 2017
 *
 ***********************************************************/

#ifndef FAT32_H
#define FAT32_H

void cdFcn(char *);
void dirFcn();
void getFcn(char *);
void infoFcn();
void processInput(char *);
void startShell(char *);
int tokenizeString(char **, char *, char *);

#endif

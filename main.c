/*************************************************************
 *
 * Module:  main.c
 * Author:  Maxine Fehr
 * Date:    March 27, 2017
 *
 * Purpose:
 *
 *************************************************************/

#include <stdio.h>
#include "fat32.h" 

int main(int argc, char *argv[]) {
    if (2 == argc)
        startShell(argv[1]);
    else
        printf("Invalid input. Use format \"./%s /dev/device\"\n", argv[0]);
    return 0;
}

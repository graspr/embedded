//
//  utils.c
//  rpi
//
//  Created by Anton de Winter on 4/12/15.
//  Copyright (c) 2015 Anton de Winter. All rights reserved.
//

#include <stdio.h>
#include <stdint.h>
#include <sys/time.h>
#include "utils.h"

uint64_t getTimeStamp() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec*(uint64_t)1000000+tv.tv_usec;
}

void print_array(uint32_t* arr, int len) {
    int i;
    for (i=0; i<len; i++) {
        printf("%d,", arr[i]);
    }
    printf("\n");
}

//assumes little endian
void printBits(size_t const size, void const * const ptr)
{
    unsigned char *b = (unsigned char*) ptr;
    unsigned char byte;
    int i, j;
    
    for (i=size-1;i>=0;i--)
    {
        for (j=7;j>=0;j--)
        {
            byte = b[i] & (1<<j);
            byte >>= j;
            printf("%u", byte);
        }
    }
    puts("");
}

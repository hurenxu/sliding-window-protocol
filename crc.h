#ifndef __CRC_H__
#define __CRC_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <unistd.h>
#include <netdb.h>
#include <math.h>
#include <sys/time.h>
#include "common.h"

char get_bit (char byte, int pos); 
// return a char with a value of 0 or 1 depending on whether the bit in the pos is 0 or 1
char crc8 (char* array, int byte_len); 
void append_crc (char* array, int array_len); 
// append crc remainder to the char array
int is_corrupted (char* array, int array_len); 
// return 1 if a frame is corrupted, otherwise return 0

#endif

#include "crc.h"

char get_bit (char byte, int pos) 
{
  // return a char with a value of 0 or 1 depending on whether the bit in the pos is 0 or 1
  char tmp = byte >> pos;
  if ((tmp & 0x01) == 0) 
  {
    return 0;
  }
  else {
    return 1;
  }
}

// Function returns the remainder from a CRC calculation on a char* array of length byte_len 
char crc8(char* array, int array_len){       
  // The most significant bit of the polynomial can be discarded in the computation, because:      
  // (1) it is always 1      
  // (2) it aligns with the next '1' of the dividend; the XOR result for this bit is always 0      
  char poly =0x07;  //00000111       
  char crc = array[0];       
  int i, j;       
  for(i = 1; i < array_len; i++){            
    char next_byte = array[i];            
    for(j = 7; j >= 0; j--){  
      // Start at most significant bit of next byte and work our way down                 
      // crcs most significant bit is 0
      //if(get_bit(crc, j) == 0){   
      if((crc & 0x80) == 0) {
        // left shift crc by 1;  
	      crc = crc << 1;
        crc = crc | get_bit(next_byte, j); 
        // get_bit(next_byte, j) returns the a bit in position j from next_byte                  
      } 
      else{ 
        // crcs most significant bit is 1                      
        // left shift crc by 1;  
	      crc = crc << 1;
        crc = crc | get_bit(next_byte, j);   
        crc = crc ^ poly;                 
      }            
    }      
  }       
  return crc;  
} 

void append_crc (char* array, int array_len)
{
  // append crc remainder to the char array
  char crc = crc8(array, array_len); 
  array[array_len - 1] = array[array_len - 1] ^ crc;
}

int is_corrupted (char* array, int array_len) 
{
  // return 1 if a frame is corrupted, otherwise return 0
  char crc = crc8(array, array_len);
  if(crc == 0) 
  {
    // means no error
    return 0;
  }
  else 
  {
    return 1;
  }
}


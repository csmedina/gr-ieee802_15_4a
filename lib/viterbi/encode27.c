/* Encode standard input in the r=1/2 K=7 standard convolutional code.  
 * Encoded output is big-endian, one byte per symbol, with 1 represented  
 * by 255 and 0 by 0  
 */   
   
#include <stdio.h>   
#include "viterbi27.h"   
   
extern unsigned char Partab[];  /* Parity lookup table */   
   
main(int argc,char *argv[])   
{   
  unsigned int encstate;   
  int c,i;   
   
  encstate = 0;   
  while((c = getchar()) != EOF){   
    for(i=7;i>=0;i--){   
      encstate = (encstate << 1) | ((c >> 7) & 1);   
      c <<= 1;   
      /* 1-sym -> 255, 0-sym -> 0 */   
      putchar(0 - Partab[encstate & POLYA]);   
      putchar(0 - Partab[encstate & POLYB]);   
    }   
  }   
  /* Generate tail */   
  for(i=5;i>=0;i--){   
    encstate = encstate << 1;   
    putchar(0 - Partab[encstate & POLYA]);   
    putchar(0 - Partab[encstate & POLYB]);   
  }   
  return 0;   
}   
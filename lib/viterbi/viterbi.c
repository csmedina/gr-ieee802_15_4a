/* Viterbi decoder for arbitrary convolutional code  
 * viterbi27 and viterbi37 for the r=1/2 and r=1/3 K=7 codes are faster  
 * Copyright 1999 Phil Karn, KA9Q  
 * May be used under the terms of the GNU Public License  
 */   
#include <stdio.h>   
/* Select code here */   
#define K 3            /* Constraint length */   
#define N 2         /* Number of symbols per data bit */   
#define Polys   Poly23a     /* Select polynomials here */   
   
/* Rate 1/2 codes */   
unsigned int Poly23a[] = { 0x2, 0x5 };       /* K=3 , used in 802.15.4a */
unsigned int Poly23[] = { 0x7, 0x5 };       /* K=3 */   
unsigned int Poly24[] = { 0xf, 0xb };       /* k=4 */   
unsigned int Poly25[] = { 0x17, 0x19 };     /* k=5 */   
unsigned int Poly25a[] = { 0x13, 0x1b };    /* k=5, used in GSM?  */   
unsigned int Poly26[] = { 0x2f, 0x35 };     /* k=6  */   
unsigned int Poly27[] = { 0x6d, 0x4f };     /* k=7; very popular with NASA and industry  */   
unsigned int Poly28[] = { 0x9f, 0xe5 };     /* k=8  */   
unsigned int Poly29[] = { 0x1af, 0x11d };   /* k=9; used in IS-95 CDMA  */   
unsigned int Poly215[] = { 0x45dd, 0x69e3 };    /* k = 15  */   
   
/* Rate 1/3 codes */   
unsigned int Poly33[] = { 0x7, 0x7, 0x5 };  /* k = 3 */   
unsigned int Poly34[] = { 0xf, 0xb, 0xd };  /* k = 4 */   
unsigned int Poly35[] = { 0x1f, 0x1b, 0x15 };   /* k = 5 */   
unsigned int Poly36[] = { 0x2f, 0x35, 0x39 };   /* k = 6 */   
unsigned int Poly37[] = { 0x4f, 0x57, 0x6d };   /* k = 7; also popular with NASA and industry  */   
unsigned int Poly38[] = { 0xef, 0x9b, 0xa9 };   /* k = 8  */   
unsigned int Poly39[] = { 0x1ed, 0x19b, 0x127 }; /* k = 9; used in IS-95 CDMA  */   
   
/* Rate 1/6 code */   
unsigned int Poly615[] = { 042631, 047245, 073363, 056507, 077267, 064537 }; /* k=15 on Mars Pathfinder */   
   
   
#if (K>=6)   
#define D   (1 << (K-6))   
#else   
#define D   1   
#endif   
   
#include <memory.h>   
#define NULL ((void *)0)   
   
extern unsigned char Partab[];  /* Parity lookup table */   
   
int Rate = N;   
   
int Syms[1 << K];   
int VDInit = 0;   
   
inline int   
parity(int x)   
{   
  x ^= (x >> 16);   
  x ^= (x >> 8);   
  return Partab[x & 0xff];   
}   
   
/* Convolutionally encode data into binary symbols */   
int encode(   
unsigned char *symbols,   
unsigned char *data,   
unsigned int nbytes,   
unsigned int startstate,   
unsigned int endstate)   
{   
  int i,j;   
  unsigned int encstate = startstate;   
   
  while(nbytes-- != 0){   
	  //csmedina each input data contains only one bit
    //for(i=7;i>=0;i--){   
      //encstate = (encstate << 1) | ((*data >> i) & 1);   
      encstate = (encstate << 1) | (*data & 1);   
      for(j=0;j<N;j++)   
    *symbols++ = parity(encstate & Polys[j]);   
    //}   
    data++;   
  }   
  /* Flush out tail */   
  for(i=K-2;i>=0;i--){   
    encstate = (encstate << 1) | ((endstate >> i) & 1);   
    for(j=0;j<N;j++)   
      *symbols++ = parity(encstate & Polys[j]);   
  }   
  return 0;   
}   
   
/* Viterbi decoder */   
int   
viterbi_802154a(   
unsigned long *metric,  /* Final path metric (returned value) */   
unsigned char *data,    /* Decoded output data */   
unsigned char *symbols, /* Raw deinterleaved input symbols */   
unsigned int nbits, /* Number of output bits */   
int mettab[2][256], /* Metric table, [sent sym][rx symbol] */   
unsigned int startstate,         /* Encoder starting state */   
unsigned int endstate            /* Encoder ending state */   
){   	
  int bitcnt = -(K-1);   
  long m0,m1;   
  int i,j,sym;   
  int mets[1 << N];   
  unsigned long paths[(nbits+K-1)*D],*pp,mask; 
  long cmetric[1 << (K-1)],nmetric[1 << (K-1)];   
  memset(paths,0,sizeof(paths));   
  if(!VDInit){   
    for(i=0;i<(1<<K);i++){   
      sym = 0;   
      for(j=0;j<N;j++)   
    sym = (sym << 1) + parity(i & Polys[j]);   
      Syms[i] = sym;   
    }   
    VDInit++;   
  }   
   
  startstate &= ~((1<<(K-1)) - 1);   
  endstate &= ~((1<<(K-1)) - 1);   
   
  /* Initialize starting metrics */   
  for(i=0;i< 1<<(K-1);i++)   
    cmetric[i] = -999999;   
  cmetric[startstate] = 0;   
   
  pp = paths;   
  for(;;){ /* For each data bit */   
    /* Read input symbols and compute branch metrics */  
   
    for(i=0;i< 1<<N;i++){   
      mets[i] = 0;   
      for(j=0;j<N;j++){   
    mets[i] += mettab[(i >> (N-j-1)) & 1][symbols[j]];   
      }   
    }   
   
    symbols += N;   
    /* Run the add-compare-select operations */   
    mask = 1;   
    for(i=0;i< 1 << (K-1);i+=2){   
      int b1,b2;   
         
      b1 = mets[Syms[i]];   
      nmetric[i] = m0 = cmetric[i/2] + b1;    
      b2 = mets[Syms[i+1]];   
      b1 -= b2;   
      m1 = cmetric[(i/2) + (1<<(K-2))] + b2;   
      if(m1 > m0){   
    nmetric[i] = m1;   
    *pp |= mask;   
      }   
      m0 -= b1;   
      nmetric[i+1] = m0;   
      m1 += b1;   
      if(m1 > m0){   
    nmetric[i+1] = m1;   
    *pp |= mask << 1;   
      }   
      mask <<= 2;   
      if(mask == 0){   
    mask = 1;   
    pp++;   
      }   
    }   
    if(mask != 1){   
      pp++;   
    }   
   
    if(++bitcnt == nbits){   
      *metric = nmetric[endstate];   
   
      break;   
    }   
   
    memcpy(cmetric,nmetric,sizeof(cmetric));  
   
  }   
   
  /* Chain back from terminal state to produce decoded data */   
  if(data == NULL)   
    return 0;/* Discard output */   
  memset(data,0,(nbits+7)/8); /* round up in case nbits % 8 != 0 */   
   
  for(i=nbits-1;i >= 0;i--){   
    pp -= D;   
    if(pp[endstate >> 5] & (1 << (endstate & 31))){   
      endstate |= (1 << (K-1));   
      data[i>>3] |= 0x80 >> (i&7);   
    }   
    endstate >>= 1;   
  }   
  
  return 0;   
}   

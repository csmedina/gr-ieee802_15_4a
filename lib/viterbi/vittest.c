/* Test a soft decision viterbi decoder  
 * Copyright 1999 Phil Karn, KA9Q  
 * May be used under the terms of the GNU Public License  
 */   
   
#include <stdio.h>   
#include <stdlib.h>   
#include <math.h>   
#include <memory.h>   
   
void modnoise(unsigned char *,unsigned int,double,double);   
int viterbi(   
long *metric,           /* Final path metric (returned value) */   
unsigned char *data,    /* Decoded output data */   
unsigned char *symbols, /* Raw deinterleaved input symbols */   
unsigned int nbits, /* Number of output bits; 2*(nbits+6) input symbols will be read */   
int mettab[2][256], /* Metric table, [sent sym][rx symbol] */   
unsigned int startstate,         /* Encoder starting state */   
unsigned int endstate            /* Encoder ending state */   
);   
   
/* Convolutionally encode data into binary symbols */   
int encode(   
unsigned char *symbols,   
unsigned char *data,   
unsigned int nbytes,   
unsigned int startstate,   
unsigned int endstate   
);   
   
   
   
/* Lookup table giving count of 1 bits for integers 0-255 */   
unsigned char Bitcnt[] = {   
 0, 1, 1, 2, 1, 2, 2, 3,   
 1, 2, 2, 3, 2, 3, 3, 4,   
 1, 2, 2, 3, 2, 3, 3, 4,   
 2, 3, 3, 4, 3, 4, 4, 5,   
 1, 2, 2, 3, 2, 3, 3, 4,   
 2, 3, 3, 4, 3, 4, 4, 5,   
 2, 3, 3, 4, 3, 4, 4, 5,   
 3, 4, 4, 5, 4, 5, 5, 6,   
 1, 2, 2, 3, 2, 3, 3, 4,   
 2, 3, 3, 4, 3, 4, 4, 5,   
 2, 3, 3, 4, 3, 4, 4, 5,   
 3, 4, 4, 5, 4, 5, 5, 6,   
 2, 3, 3, 4, 3, 4, 4, 5,   
 3, 4, 4, 5, 4, 5, 5, 6,   
 3, 4, 4, 5, 4, 5, 5, 6,   
 4, 5, 5, 6, 5, 6, 6, 7,   
 1, 2, 2, 3, 2, 3, 3, 4,   
 2, 3, 3, 4, 3, 4, 4, 5,   
 2, 3, 3, 4, 3, 4, 4, 5,   
 3, 4, 4, 5, 4, 5, 5, 6,   
 2, 3, 3, 4, 3, 4, 4, 5,   
 3, 4, 4, 5, 4, 5, 5, 6,   
 3, 4, 4, 5, 4, 5, 5, 6,   
 4, 5, 5, 6, 5, 6, 6, 7,   
 2, 3, 3, 4, 3, 4, 4, 5,   
 3, 4, 4, 5, 4, 5, 5, 6,   
 3, 4, 4, 5, 4, 5, 5, 6,   
 4, 5, 5, 6, 5, 6, 6, 7,   
 3, 4, 4, 5, 4, 5, 5, 6,   
 4, 5, 5, 6, 5, 6, 6, 7,   
 4, 5, 5, 6, 5, 6, 6, 7,   
 5, 6, 6, 7, 6, 7, 7, 8,   
};   
   
main(argc,argv)   
int argc;   
char *argv[];   
{   
  double ebn0,esn0,noise;   
  int mettab[2][256];   
  int amp,nbits,nbytes,i;   
  unsigned char *symbols;   
  unsigned char *data,*decdata;   
  unsigned long metric;   
  long t,ntrials;   
  long seed;   
  extern char *optarg;   
  unsigned long biterrs = 0;   
  unsigned long framerrs = 0;   
  int timetrial;   
  int verbose = 0;   
  int rate; /* 1/rate is the actual code rate */   
  extern int Rate;   
     
  amp = 100;   
  ebn0 = 5.0;   
  nbits = 1152;   
  ntrials = 10;   
  timetrial = 0;   
  time(&seed);   
  while((i = getopt(argc,argv,"a:e:n:N:qs:t")) != EOF){   
    switch(i){   
    case 'a':   
      amp = atoi(optarg);   /* Signal amplitude in units */   
      break;   
    case 'e':   
      ebn0 = atof(optarg);  /* Eb/N0 in dB */   
      break;   
    case 'n':   
      nbits = atoi(optarg); /* Number of data bits */   
      break;   
    case 'N':   
      ntrials = atoi(optarg);   
      break;   
    case 's':   
      seed = atoi(optarg);   
      break;   
    case 't':   /* Generate noise once, for decoder timing */   
      timetrial = 1;   
      break;   
    case 'v':   
      verbose++;   
      break;   
    case '?':   
      usage();   
      exit(1);   
    }   
  }   
  nbytes = nbits/8;   
  srandom(seed);   
     
  esn0 = ebn0 - 10*log10((double)Rate); /* Es/N0 in dB */      
     
  /* Compute noise voltage. The 0.5 factor accounts for BPSK seeing  
   * only half the noise power, and the sqrt() converts power to  
   * voltage.  
   */   
  noise = sqrt(0.5/pow(10.,esn0/10.));   
     
  data = malloc(nbytes);   
  decdata = malloc(nbytes);   
  symbols = malloc((nbits+20)*Rate); /* 20 > max K */   
     
  /* Generate metrics analytically, with gaussian pdf */       
  gen_met(mettab,amp,noise,0.,4);   
  printf("metric table range %d to %d\n",mettab[0][0],mettab[1][0]);   
     
  /* Generate data */   
  memset(data,0x00,nbytes);    /* Default to all 0's */   
  data[0] = 0x01;   
  data[1] = 0x23;   
  data[2] = 0x45;   
  data[3] = 0x67;   
  data[4] = 0x55;   
  data[nbytes-4] = 0x55;   
  data[nbytes-3] = 0xaa;   
  data[nbytes-2] = 0xff;   
  data[nbytes-1] = 0xff;   
    
  i = 0;   
  if(timetrial){   
    encode(symbols,data,nbytes,0,0);   
    modnoise(symbols,(nbits+20)*2,amp,noise);   
    for(t = 1;t <= ntrials;t++)   
      viterbi_802154a(&metric,decdata,symbols,nbits,mettab,0,0);   
    printf("bits decoded = %ld\n",nbits*ntrials);   
    exit(0);   
  }   
  printf("original data:\n",t);   
  for(i=0;i<nbytes;i++)   
    printf("%02x",data[i]);   
  printf("\n");            
  for(t = 1;t <= ntrials;t++){   
    /* Modulate and add noise */   
    encode(symbols,data,nbytes,0,0);   
    modnoise(symbols,(nbits+20)*Rate,amp,noise);   
    viterbi_802154a(&metric,decdata,symbols,nbits,mettab,0,0);   
    if(memcmp(data,decdata,nbytes) != 0){   
     // printf("frame %d decoded data:\n",t);   
      //for(i=0;i<nbytes;i++)   
    //printf("%02x",decdata[i]);   
      //printf("\n");   
      framerrs++;   
      for(i=0;i<nbytes;i++){   
    biterrs += Bitcnt[data[i]^decdata[i]];   
      }   
    }   
  }   
  printf("Seed %ld Amplitude %d units, Eb/N0 = %lg dB\n",seed,amp,ebn0);   
  printf("Amplitude %d units, Eb/N0 = %lg dB\n",amp,ebn0);   
  printf("Frame length = %d bits, #frames = %ld, bits decoded = %ld\n",   
     nbits,ntrials,nbits*ntrials);   
  printf("frame errors: %ld (%g)\n",framerrs,(double)framerrs/ntrials);   
  printf("bit errors: %ld (%g)\n",biterrs,(double)biterrs/(nbits*ntrials));   
}   
   
usage()   
{   
  printf("Usage: vittest [options]\n");   
  printf("Option&default    meaning\n");   
  printf("-a 100        amplitude\n");   
  printf("-e 5.0        Eb/N0 in dB\n");   
  printf("-n 1152       trial frame size, bits\n");   
  printf("-N 10     number of trials\n");   
  printf("-s [cur time] seed for random number generator\n\n");   
  printf("-t        select timetest mode (default off)\n");   
} 

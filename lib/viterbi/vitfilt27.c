/* Viterbi decoder for K=7 rate=1/2 convolutional code  
 * continuous traceback version  
 * Copyright 1996 Phil Karn, KA9Q  
 *  
 * This version of the Viterbi decoder reads a continous stream of  
 * 8-bit soft decision samples from standard input in offset-binary  
 * form, i.e., a 255 sample is the strongest possible "1" symbol and a  
 * 0 is the strongest possible "0" symbol. 128 is an erasure (unknown).  
 *  
 * The decoded output is written to stdout in big-endian form (the first  
 * decoded bit appears in the high order bit of the first output byte).  
 *  
 * The metric table is fixed, and no attempt is made (yet) to find proper  
 * symbol synchronization. These are likely future enhancements.  
 */   
#include <stdio.h>   
#include <limits.h>   
#include "viterbi27.h"   
   
/* This parameter sizes the path memory in bits, which is organized as a  
 * circular buffer through which we periodically "trace back" to  
 * produce the decoded data. PATHMEM must be greater than  
 * MERGEDIST+TRACECHUNK, and for efficiency it should also be a power of 2.  
 * Don't make it *too* large, or it will spill out of the CPU's on-chip cache  
 * and decrease performance. Each bit of path memory costs 8 bytes for the  
 * K=7 code.  
 */   
#define PATHMEM     128   
   
/* In theory, a Viterbi decoder is true maximum likelihood only if  
 * the path memory is as long as the entire message and a single traceback  
 * is made from the terminal state (usually zero) after the entire message  
 * is received.  
 *  
 * In practice, performance is essentially optimum as long as decoding  
 * decisions are deferred by at least 4-5 constraint lengths (28-35 bits  
 * for K=7) from the most recently received symbols. MERGEDIST sets this  
 * parameter. We give ourselves some margin here in case the code is  
 * punctured (which slows merging) and also to let us start each traceback  
 * from an arbitrary current state instead of taking the time to find the  
 * path with the highest current metric.  
 */   
#define MERGEDIST   64  /* Distance to trace back before decoding */   
   
/* Since each traceback is costly (thanks to the overhead of having to  
 * go back MERGEDIST bits before we produce our first decoded bit) we'd like  
 * to decode as many bits as possible per traceback at the expense of  
 * increased decoding delay. TRACECHUNK sets how many bits to  
 * decode on each traceback. Since output is produced in 8-bit bytes,  
 * TRACECHUNK MUST be a multiple of 8.  
 */   
#define TRACECHUNK  64  /* How many bits to decode on each traceback */   
   
/* The path metrics need to be periodicially adjusted downward  
 * to prevent an integer overflow that could cause the signed comparisons  
 * in the butterfly macros to fail.  
 *  
 * It's possible to code the comparisons to work in modulo fashion, e.g.,  
 * as 'if((a-b) > 0)' rather than 'if(a >b)'. A good optimizer would generate  
 * code like 'cmp a,b;js foo' for this, but GCC doesn't.  
 *  
 * This constant should be larger than the maximum path metric spread.  
 * Experimentally this seems to be 2040, which is probably related to the  
 * free distance of the code (10) and the symbol metric scale (0-255).  
 */   
#define RENORMALIZE 10000   
   
#if (TRACECHUNK + MERGEDIST > PATHMEM)   
#error "TRACECHUNK + MERGEDIST > PATHMEM"   
#endif   
   
#if ((TRACECHUNK % 8) != 0)   
#error "TRACECHUNK not multiple of 8"   
#endif   
   
static void traceback(unsigned long paths[],unsigned int pi);   
static void flush(unsigned long paths[],unsigned int pi);   
   
/* Need some options here: user settable metric table, verbosity options, etc */   
main(int argc,char *argv[])   
{   
    unsigned int bitcnt = 0;   
    int beststate,i;   
    long cmetric[64],nmetric[64];   
    unsigned long paths[2*PATHMEM];   
    register unsigned long dec;   
    int mets[4];   
    unsigned int pi = 0,first=1;   
    unsigned char symbols[2];   
    int mettab[2][256];   
       
    /* Initialize metric table (make this an option)  
     * This table assumes a symbol of 0 is the  
     * strongest possible '0', and a symbol  
     * of 255 is the strongest possible '1'. A symbol  
     * of 128 is an erasure  
     */   
    for(i=0;i<256;i++){   
        mettab[0][i] = 128 - i;   
        mettab[1][255-i] = 127 - i;   
    }   
    cmetric[0] = 0;   
    for(i=1;i<64;i++)   
        cmetric[i] = -99999;   
   
    /* Main loop -- read input symbols and run ACS butterflies,  
     * periodically tracing back to produce decoded output data.  
     * The loop is unrolled to process two bits per iteration.  
     */   
    for(;;){   
        /* Renormalize metrics to prevent overflow */   
        if(cmetric[0] > (LONG_MAX - RENORMALIZE)){   
            for(i=0;i<64;i++)   
                cmetric[i] -= LONG_MAX;   
        } else if(cmetric[0] < LONG_MIN+RENORMALIZE){   
            for(i=0;i<64;i++)   
                cmetric[i] += LONG_MAX;   
        }   
        /* Read input symbol pair and compute branch metrics */   
        symbols[0] = getchar();   
        symbols[1] = getchar();   
        if(feof(stdin))   
            break;   
        mets[0] = mettab[0][symbols[0]] + mettab[0][symbols[1]];   
        mets[1] = mettab[0][symbols[0]] + mettab[1][symbols[1]];   
        mets[3] = mettab[1][symbols[0]] + mettab[1][symbols[1]];   
        mets[2] = mettab[1][symbols[0]] + mettab[0][symbols[1]];   
   
        /* On even numbered bits, the butterflies read from cmetrics[]  
         * and write to nmetrics[]. On odd numbered bits, the reverse  
         * is done  
         */   
        dec = 0;   
        BUTTERFLY(0,0);   
        BUTTERFLY(6,0);   
        BUTTERFLY(8,0);   
        BUTTERFLY(14,0);   
        BUTTERFLY(2,3);   
        BUTTERFLY(4,3);   
        BUTTERFLY(10,3);   
        BUTTERFLY(12,3);   
        BUTTERFLY(1,1);   
        BUTTERFLY(7,1);   
        BUTTERFLY(9,1);   
        BUTTERFLY(15,1);   
        BUTTERFLY(3,2);   
        BUTTERFLY(5,2);   
        BUTTERFLY(11,2);   
        BUTTERFLY(13,2);   
        paths[2*pi] = dec;   
        dec = 0;   
        BUTTERFLY(19,0);   
        BUTTERFLY(21,0);   
        BUTTERFLY(27,0);   
        BUTTERFLY(29,0);   
        BUTTERFLY(17,3);   
        BUTTERFLY(23,3);   
        BUTTERFLY(25,3);   
        BUTTERFLY(31,3);   
        BUTTERFLY(18,1);   
        BUTTERFLY(20,1);   
        BUTTERFLY(26,1);   
        BUTTERFLY(28,1);   
        BUTTERFLY(16,2);   
        BUTTERFLY(22,2);   
        BUTTERFLY(24,2);   
        BUTTERFLY(30,2);   
        paths[2*pi+1] = dec;   
        pi++;   
   
        /* Read input symbol pair and compute branch metrics */   
        symbols[0] = getchar();   
        symbols[1] = getchar();   
        if(feof(stdin))   
            break;   
        mets[0] = mettab[0][symbols[0]] + mettab[0][symbols[1]];   
        mets[1] = mettab[0][symbols[0]] + mettab[1][symbols[1]];   
        mets[3] = mettab[1][symbols[0]] + mettab[1][symbols[1]];   
        mets[2] = mettab[1][symbols[0]] + mettab[0][symbols[1]];   
   
        dec = 0;   
        BUTTERFLY2(0,0);   
        BUTTERFLY2(6,0);   
        BUTTERFLY2(8,0);   
        BUTTERFLY2(14,0);   
        BUTTERFLY2(2,3);   
        BUTTERFLY2(4,3);   
        BUTTERFLY2(10,3);   
        BUTTERFLY2(12,3);   
        BUTTERFLY2(1,1);   
        BUTTERFLY2(7,1);   
        BUTTERFLY2(9,1);   
        BUTTERFLY2(15,1);   
        BUTTERFLY2(3,2);   
        BUTTERFLY2(5,2);   
        BUTTERFLY2(11,2);   
        BUTTERFLY2(13,2);   
        paths[2*pi] = dec;   
        dec = 0;   
        BUTTERFLY2(19,0);   
        BUTTERFLY2(21,0);   
        BUTTERFLY2(27,0);   
        BUTTERFLY2(29,0);   
        BUTTERFLY2(17,3);   
        BUTTERFLY2(23,3);   
        BUTTERFLY2(25,3);   
        BUTTERFLY2(31,3);   
        BUTTERFLY2(18,1);   
        BUTTERFLY2(20,1);   
        BUTTERFLY2(26,1);   
        BUTTERFLY2(28,1);   
        BUTTERFLY2(16,2);   
        BUTTERFLY2(22,2);   
        BUTTERFLY2(24,2);   
        BUTTERFLY2(30,2);   
        paths[2*pi+1] = dec;   
        pi = (pi + 1) % PATHMEM;   
        if((pi % TRACECHUNK) == 0){   
            if(!first)   
                traceback(paths,pi);   
            first = 0;   
        }   
    }   
    flush(paths,pi);   
}   
   
/* Periodic traceback to produce decoded data */   
static void   
traceback(unsigned long paths[],unsigned int pi)   
{   
    int beststate,i,j;   
    unsigned char data[TRACECHUNK/8];   
   
    /* Start on an arbitrary path and trace it back until it's almost  
     * certain we've merged onto the best path  
     */   
    beststate = 0;  /* arbitrary */   
    pi = (pi - 1) % PATHMEM;    /* Undo last increment of pi */   
    for(i=0;i < MERGEDIST-6;i++){   
        if(paths[2*pi + (beststate >> 5)] & (1 << (beststate & 31))){   
            beststate |= 64;    /* 2^(K-1) */   
        }   
        beststate >>= 1;   
        pi = (pi - 1) % PATHMEM;   
    }   
    /* bestpath is now the encoder state on the best path, MERGEDIST  
     * bits back. We continue to chain back until we accumulate  
     * TRACECHUNK bits of decoded data  
     */   
    for(j=sizeof(data)-1;j >= 0;j--){   
        data[j] = 0;   
        for(i=0;i<8;i++){   
            if(paths[2*pi + (beststate >> 5)] & (1 << (beststate & 31))){   
                beststate |= 64;    /* 2^(K-1) */   
                data[j] |= 1 << i;   
            }   
            beststate >>= 1;   
            pi = (pi - 1) % PATHMEM;   
        }   
    }   
    fwrite(data,1,sizeof(data),stdout);   
}   
   
/* Final traceback at end of trellis, assuming sender tailed to zero after an  
 * integral number of data bytes. Trailing bits are dropped.  
 */   
static void   
flush(unsigned long paths[],unsigned int pi)   
{   
    int beststate,i,j,n,off;   
    unsigned char data[(MERGEDIST+TRACECHUNK)/8];   
   
    beststate = 0;  /* Assume encoder tailing to 0 state */   
    n = (MERGEDIST-6 + (pi % TRACECHUNK)) / 8;   
    off = (MERGEDIST-6 + (pi % TRACECHUNK)) % 8;    /* ignored partial byte */   
    pi = (pi - off - 1) % PATHMEM;   
    for(j=n-1;j >= 0;j--){   
        data[j] = 0;   
        for(i=0;i<8;i++){   
            if(paths[2*pi + (beststate >> 5)] & (1 << (beststate & 31))){   
                beststate |= 64;    /* 2^(K-1) */   
                data[j] |= 1 << i;   
            }   
            beststate >>= 1;   
            pi = (pi - 1) % PATHMEM;   
        }   
    }   
    fwrite(data,1,n,stdout);   
}
/* -*- c++ -*- */
/* 
 * Copyright 2013 <+YOU OR YOUR COMPANY+>.
 * 
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#ifdef __cplusplus
extern "C" {  // only need to export C interface if
              // used by C++ source code
#endif
#include <gnuradio/fec/rs.h>
#ifdef __cplusplus
};
#endif
#include <memory.h>
#include <stdlib.h>
#include <stdio.h>

#include <ieee802_15_4a/fec_rs.h>

static struct {
  int symsize;
  int genpoly;
  int fcs;
  int prim;
  int nroots;
} Tab = {6, 0x43,    1,   1, 8 };	  
	  
  void bin2symb (const unsigned char *bin,  unsigned char *symb)
  {
	  int idx;
	  
	  for (idx = 0; idx < 63; ++idx)
	  {
		  symb[idx] = bin[6*idx+5] * 32 + bin[6*idx+4] * 16 + bin[6*idx+3] * 8 + bin[6*idx+2] * 4 + bin[6*idx+1] * 2 + bin[6*idx];
		  //fprintf (stdout, "%d\t", symb[idx]);
	  }
  }
   
  void symb2bin (unsigned char *symb, unsigned char *bin)
  {
	  int idx;
	  unsigned char msg;
	  
	  for (idx = 0; idx < 63; ++idx)
	  {
		  msg = symb[idx];
		  bin[6*idx] = msg & 0x01;
		  bin[6*idx + 1] = (msg>>1) & 0x01;
		  bin[6*idx + 2] = (msg>>2) & 0x01;
		  bin[6*idx + 3] = (msg>>3) & 0x01;
		  bin[6*idx + 4] = (msg>>4) & 0x01;
		  bin[6*idx + 5] = (msg>>5) & 0x01;
	  }
	  /*for (int idx = 0; idx < 378; ++idx)
	  {
		  fprintf (stdout, "%d  ", bin[idx]);
	  }*/
  }
  
namespace gr {
  namespace ieee802_15_4a {

	  void ReedSolomon :: encode (const unsigned char *msg, int len, unsigned char *codedMsg)
	  {
		  void *handle;
		  unsigned char padded[378];
		  unsigned char block[63];
		  unsigned char bin[378];
		  
		  if((handle = init_rs_char(Tab.symsize,Tab.genpoly,Tab.fcs,Tab.prim,Tab.nroots)) == NULL)
		  {
			  printf("init_rs_char failed!\n");
			  return;
		  }
		  
		  memset (padded, 0x00, 378);
		  memcpy (padded + 330 - len, msg, len);
		  bin2symb (padded, block);
		  
		  encode_rs_char(handle, block, &block[55]);
		  
		  /*for (int idx = 0; idx < 63; ++idx)
		  {
			  fprintf (stdout, "%d\t", block[idx]);
		  }
		  */ 
		  symb2bin (block, bin);
		  
		  memcpy (codedMsg, bin + 330 - len, len + 48);
		  /*for (int idx = 0; idx < 184; ++idx)
		  {
			  fprintf (stdout, "%d\t", codedMsg[idx]);
		  }*/
		  		  
		  free_rs_char(handle);
	  }
	  
	  void ReedSolomon :: decode (const unsigned char *codedMsg, int len, unsigned char *msg)
	  {
		  void *handle;
		  unsigned char padded[378];
		  unsigned char block[63];
		  unsigned char bin[378];
		  
		  if((handle = init_rs_char(Tab.symsize,Tab.genpoly,Tab.fcs,Tab.prim,Tab.nroots)) == NULL)
		  {
			  printf("init_rs_char failed!\n");
			  return;
		  }
		  
		  memset (padded, 0x00, 378);
		  memcpy (padded + 378 - len, codedMsg, len);
		  bin2symb (padded, block);
		  
		  decode_rs_char(handle, block, NULL,0);
		  
		  symb2bin (block, bin);
		  memcpy (msg, bin + 378 - len, len - 48);
		  
		  free_rs_char(handle);
	  }
  } /* namespace ieee802_15_4a */
} /* namespace gr */
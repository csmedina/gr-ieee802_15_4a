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

#include <string.h>

#include <gnuradio/io_signature.h>
#include <ieee802_15_4a/reedsolomon.h>

#ifdef __cplusplus
extern "C" {  // only need to export C interface if
              // used by C++ source code
#endif
#include <gnuradio/fec/rs.h>
#ifdef __cplusplus
};
#endif

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
  }
  
  
namespace gr {
  namespace ieee802_15_4a {

    reedsolomon::reedsolomon()
    {
		handle = init_rs_char(Tab.symsize,Tab.genpoly,Tab.fcs,Tab.prim,Tab.nroots);
    }

    reedsolomon::~reedsolomon()
    {
		free_rs_char(handle);
    }
    
    int reedsolomon::outputSize (int ninput, int encode)
	{
		int ret;
	  if (encode)
	  {
		  int complete = ninput / 330;
		  int rest = ninput % 330;
		  
		  ret = complete * 378 + (rest == 0 ? 0 : rest + 48);
	  }else
	  {
		  int complete = ninput / 378;
		  int rest = ninput % 378;
		  
		  ret = complete * 330 + (rest == 0 ? 0 : rest - 48);
	  }
	  return ret;
	}
	
	void reedsolomon::int_encode (const unsigned char *msg, int len, unsigned char *codedMsg)
	  {  
		  unsigned char padded[378];
		  unsigned char block[63];
		  unsigned char bin[378];
		  
		  memset (padded, 0x00, 378);
		  memcpy (padded + 330 - len, msg, len);
		  bin2symb (padded, block);
		  
		  encode_rs_char(handle, block, &block[55]);

		  symb2bin (block, bin);
		  
		  memcpy (codedMsg, bin + 330 - len, len + 48);
	  }
	  
	  void reedsolomon::int_decode (const unsigned char *codedMsg, int len, unsigned char *msg)
	  {
		  unsigned char padded[378];
		  unsigned char block[63];
		  unsigned char bin[378];
		  
		  memset (padded, 0x00, 378);
		  memcpy (padded + 378 - len, codedMsg, len);
		  bin2symb (padded, block);
		  
		  decode_rs_char(handle, block, NULL,0);
		  
		  symb2bin (block, bin);
		  memcpy (msg, bin + 378 - len, len - 48);
	  }
	  
	  void reedsolomon::encode (const unsigned char *msg, int len, unsigned char *codedMsg)
	  {
		  int i;
		for (i = 0; i + 330 <= len; i += 330)
		{
			int_encode (msg + i, 330, codedMsg);
			codedMsg +=  378;
		}
			
		// The last frame
		if (len % 330 != 0){
			int_encode (msg + i, len % 330, codedMsg);
		}
	  }
	  
	  void reedsolomon::decode (const unsigned char *codedMsg, int len, unsigned char *msg)
	  {
		  int i;
		for (i = 0; i + 378 <= len; i += 378)
		{
			int_decode (codedMsg + i, 378, msg);
			msg +=  330;
		}
		// The last frame
		if (len % 378 != 0){
			int_decode (codedMsg + i, len % 378, msg);
		}
	  }

  } /* namespace ieee802_15_4a */
} /* namespace gr */


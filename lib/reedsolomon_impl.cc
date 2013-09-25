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

#include <stdio.h>
#include <gnuradio/io_signature.h>
#include "reedsolomon_impl.h"

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

	reedsolomon::sptr
    reedsolomon::make(int datasize, int encode)
    {
      return gnuradio::get_initial_sptr
        (new reedsolomon_impl(datasize, encode));
    }

    /*
     * The private constructor
     */
    reedsolomon_impl::reedsolomon_impl(int datasize, int encode)
      : gr::block("reedsolomon",
              gr::io_signature::make(1, 1, sizeof(char)),
              gr::io_signature::make(1, 1, sizeof(char))), _datasize(datasize), _encode(encode)
    {
		if (_encode)
		{
			set_relative_rate (378./330.);
			set_output_multiple (_datasize < 330 ? _datasize + 48 : 378);
		}else
		{
			set_relative_rate (330./378.);
			set_output_multiple (_datasize < 378 ? _datasize - 48 : 330);
		}
		handle = init_rs_char(Tab.symsize,Tab.genpoly,Tab.fcs,Tab.prim,Tab.nroots);
	}

    /*
     * Our virtual destructor.
     */
    reedsolomon_impl::~reedsolomon_impl()
    {
		free_rs_char(handle);
    }
	  
	void reedsolomon_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
	{
	  if (_encode)
	  {
		  int complete = noutput_items / 378;
		  int rest = noutput_items % 378;
		  
		  ninput_items_required[0] = complete * 330 + (rest == 0 ? 0 : rest - 48);
	  }else
	  {
		  int complete = noutput_items / 330;
		  int rest = noutput_items % 330;
		  
		  ninput_items_required[0] = complete * 378 + (rest == 0 ? 0 : rest + 48);
	  }
	}

    int reedsolomon_impl::general_work(int noutput_items,
		       gr_vector_int &ninput_items,
		       gr_vector_const_void_star &input_items,
		       gr_vector_void_star &output_items)
    {
		const unsigned char *in = (const unsigned char *) input_items[0];
        unsigned char *out = (unsigned char *) output_items[0];
        int i;

		if (_encode)
		{
			fprintf (stderr, "encoding %d\n", ninput_items[0]);fflush(stderr);
			noutput_items = 0;
			for (i = 0; i + 330 <= ninput_items[0]; i += 330)
			{
				encode (in + i, 330, out);
				out +=  378;
				noutput_items +=  378;
				consume_each (330);
			}
			// The last frame
			if (ninput_items[0] % 330 != 0){
				encode (in + i, ninput_items[0] % 330, out);
				consume_each (ninput_items[0] % 330);
				noutput_items +=  (ninput_items[0] % 330 + 48);
			}
		}else
		{
			fprintf (stderr, "decoding %d\n", ninput_items[0]);fflush(stderr);
			noutput_items = 0;
			for (i = 0; i + 378 <= ninput_items[0]; i += 378)
			{
				decode (in + i, 378, out);
				out +=  330;
				noutput_items +=  330;
				consume_each (378);
			}
			// The last frame
			if (ninput_items[0] % 378 != 0){
				decode (in + i, ninput_items[0] % 378, out);
				consume_each (ninput_items[0] % 378);
				noutput_items +=  (ninput_items[0] % 378 - 48);
			}
		}
		// Tell runtime system how many output items we produced.
		return noutput_items;
    }
    
    void reedsolomon_impl::encode (const unsigned char *msg, int len, unsigned char *codedMsg)
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
	  
	  void reedsolomon_impl::decode (const unsigned char *codedMsg, int len, unsigned char *msg)
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
  } /* namespace ieee802_15_4a */
} /* namespace gr */


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
#include "reedsolomon_encoder_impl.h"
#include <ieee802_15_4a/fec_rs.h>

namespace gr {
  namespace ieee802_15_4a {

	reedsolomon_encoder::sptr
    reedsolomon_encoder::make(const std::string& len_tag_key)
    {
      return gnuradio::get_initial_sptr
        (new reedsolomon_encoder_impl(len_tag_key));
    }

    /*
     * The private constructor
     */
    reedsolomon_encoder_impl::reedsolomon_encoder_impl(const std::string& len_tag_key)
      : gr::tagged_stream_block("reedsolomon_encoder",
              gr::io_signature::make(1, 1, sizeof(char)),
              gr::io_signature::make(1, 1, sizeof(char)), len_tag_key)
    {
		handle = ReedSolomon::OpenHandle ();
	}

    /*
     * Our virtual destructor.
     */
    reedsolomon_encoder_impl::~reedsolomon_encoder_impl()
    {
		ReedSolomon::CloseHandle (handle);
    }

    int
    reedsolomon_encoder_impl::calculate_output_stream_length(const gr_vector_int &ninput_items)
    {
      int noutput_items;
      
      int complete = ninput_items[0] / 330;
      int rest = ninput_items[0] % 330;
      
      noutput_items = complete * 378 + (rest == 0 ? 0 : rest + 48);
       
      return noutput_items ;
    }

    int
    reedsolomon_encoder_impl::work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
        const unsigned char *in = (const unsigned char *) input_items[0];
        unsigned char *out = (unsigned char *) output_items[0];
        int i;

        noutput_items = calculate_output_stream_length(ninput_items);
        
        for (i = 0; i + 330 <= ninput_items[0]; i += 330)
        {
			ReedSolomon :: encode (in + i, 330, out, handle);
			out +=  378;
		}
		// The last frame
		if (ninput_items[0] % 330 != 0)
			ReedSolomon :: encode (in + i, ninput_items[0] % 330, out, handle);
		
        // Tell runtime system how many output items we produced.
        return noutput_items;
    }
    
  } /* namespace ieee802_15_4a */
} /* namespace gr */


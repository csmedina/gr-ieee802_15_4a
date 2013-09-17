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
#include "conv_encoder_impl.h"

#ifdef __cplusplus
extern "C" {
#endif
int encode(
unsigned char *symbols,
unsigned char *data,
unsigned int nbytes);
#ifdef __cplusplus
};
#endif

namespace gr {
  namespace ieee802_15_4a {

    conv_encoder::sptr
    conv_encoder::make(int bypass)
    {
      return gnuradio::get_initial_sptr
        (new conv_encoder_impl(bypass));
    }

    /*
     * The private constructor
     */
    conv_encoder_impl::conv_encoder_impl(int bypass)
      : gr::sync_interpolator("conv_encoder",
              gr::io_signature::make(1, 1, sizeof(char)),
              gr::io_signature::make(1, 1, sizeof(char)), bypass == 1 ? 1 : 2*8), _bypass(bypass)
    {}

    /*
     * Our virtual destructor.
     */
    conv_encoder_impl::~conv_encoder_impl()
    {
    }

    int
    conv_encoder_impl::work(int noutput_items,
			  gr_vector_const_void_star &input_items,
			  gr_vector_void_star &output_items)
    {
        const unsigned char *in = (const unsigned char *) input_items[0];
        unsigned char *out = (unsigned char *) output_items[0];
        
        if (_bypass)
        {
			memcpy (out, in, noutput_items);
			return noutput_items;
		}
		
        encode((unsigned char *)out, (unsigned char *)in, noutput_items / (2*8));

        // Tell runtime system how many output items we produced.
        return noutput_items;
    }

  } /* namespace ieee802_15_4a */
} /* namespace gr */


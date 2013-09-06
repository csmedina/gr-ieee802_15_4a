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
#include "spreading_impl.h"

namespace gr {
  namespace ieee802_15_4a {

    spreading::sptr
    spreading::make(const std::vector<gr_complex>& code)
    {
      return gnuradio::get_initial_sptr
        (new spreading_impl(code));
    }

    /*
     * The private constructor
     */
    spreading_impl::spreading_impl(const std::vector<gr_complex>& code)
      : gr::sync_interpolator("spreading",
              gr::io_signature::make(1, 1, sizeof(gr_complex)),
		      gr::io_signature::make(1, 1, sizeof(gr_complex)), code.size()),
		      d_code (code)
    {}

    /*
     * Our virtual destructor.
     */
    spreading_impl::~spreading_impl()
    {
    }

    int
    spreading_impl::work(int noutput_items,
			  gr_vector_const_void_star &input_items,
			  gr_vector_void_star &output_items)
    {
        const gr_complex *in = (const gr_complex *) input_items[0];
        gr_complex *out = (gr_complex *) output_items[0];
        
        size_t codeSize = d_code.size();
        
        // Do spreading
        int no = 0;
        int ninput_items = noutput_items/codeSize;
        for(int i = 0; i < ninput_items; i++)
        {
			for(int j = 0; j < codeSize; j++)
			{
				out[j+no] = in[i] * d_code[j]; //Input data is multipled with the code
			}
			no += codeSize;
		}
        
        // Tell runtime system how many output items we produced.
        return noutput_items;
    }

  } /* namespace ieee802_15_4a */
} /* namespace gr */


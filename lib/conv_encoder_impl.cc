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
    conv_encoder::make(int bypass, const std::string& len_tag_key)
    {
      return gnuradio::get_initial_sptr
        (new conv_encoder_impl(bypass, len_tag_key));
    }

    /*
     * The private constructor
     */
    conv_encoder_impl::conv_encoder_impl(int bypass, const std::string& len_tag_key)
      : gr::tagged_stream_block("conv_encoder",
              gr::io_signature::make(1, 1, sizeof(char)),
              gr::io_signature::make(1, 1, sizeof(char)), len_tag_key), _bypass(bypass)
    {
		//set_tag_propagation_policy(TPP_DONT);
		set_tag_propagation_policy(TPP_ALL_TO_ALL);
		set_relative_rate (2);
	}

    /*
     * Our virtual destructor.
     */
    conv_encoder_impl::~conv_encoder_impl()
    {
    }
    
    int
    conv_encoder_impl::calculate_output_stream_length(const gr_vector_int &ninput_items)
    {
		if (_bypass)
			return ninput_items[0];
		else
			return 2*ninput_items[0];		
    }
    
    
    int
    conv_encoder_impl::work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
        const unsigned char *in = (const unsigned char *) input_items[0];
        unsigned char *out = (unsigned char *) output_items[0];
        long packet_length = ninput_items[0];
        
        noutput_items = calculate_output_stream_length(ninput_items);
        
        if (_bypass)
        {
			memcpy (out, in, noutput_items);
			return noutput_items;
		}
		
		encode((unsigned char *)out, (unsigned char *)in, ninput_items[0]);
	    /*
	    std::vector<tag_t> tags;
		get_tags_in_range(tags, 0, nitems_read(0), nitems_read(0)+packet_length);
		for (size_t i = 0; i < tags.size(); i++) {
			tags[i].offset -= nitems_read(0);
			add_item_tag(0, nitems_written(0) + tags[i].offset, tags[i].key, tags[i].value);
		}
		*/
        // Tell runtime system how many output items we produced.
        return noutput_items;
    }

  } /* namespace ieee802_15_4a */
} /* namespace gr */


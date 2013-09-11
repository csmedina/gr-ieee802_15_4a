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

#ifndef INCLUDED_IEEE802_15_4A_REEDSOLOMON_DECODER_IMPL_H
#define INCLUDED_IEEE802_15_4A_REEDSOLOMON_DECODER_IMPL_H

#include <ieee802_15_4a/reedsolomon_decoder.h>


namespace gr {
  namespace ieee802_15_4a {

    class reedsolomon_decoder_impl : public reedsolomon_decoder
    {
     private:
      void *handle;

     protected:
      int calculate_output_stream_length(const gr_vector_int &ninput_items);

     public:
      reedsolomon_decoder_impl(const std::string& len_tag_key="packet_len");
      ~reedsolomon_decoder_impl();

      // Where all the action really happens
      int work(int noutput_items,
		       gr_vector_int &ninput_items,
		       gr_vector_const_void_star &input_items,
		       gr_vector_void_star &output_items);
    };

  } // namespace ieee802_15_4a
} // namespace gr

#endif /* INCLUDED_IEEE802_15_4A_REEDSOLOMON_DECODER_IMPL_H */


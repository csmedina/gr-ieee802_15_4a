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

#ifndef INCLUDED_IEEE802_15_4A_VITERBI_IMPL_H
#define INCLUDED_IEEE802_15_4A_VITERBI_IMPL_H

#include <ieee802_15_4a/viterbi.h>

namespace gr {
  namespace ieee802_15_4a {

    class viterbi_impl : public viterbi
    {
     private:
      int _bypass;
      int _amp;
      double _esn0;
      int _scale;
      int mettab[2][256];

     public:
      viterbi_impl(int bypass = 0, int amp = 100, double esn0 = 5, int scale = 4);
      ~viterbi_impl();

      // Where all the action really happens
      int work(int noutput_items,
	       gr_vector_const_void_star &input_items,
	       gr_vector_void_star &output_items);
    };

  } // namespace ieee802_15_4a
} // namespace gr

#endif /* INCLUDED_IEEE802_15_4A_VITERBI_IMPL_H */


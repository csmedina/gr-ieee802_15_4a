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

#ifndef INCLUDED_IEEE802_15_4A_BPSK_BPM_MODULATOR_IMPL_H
#define INCLUDED_IEEE802_15_4A_BPSK_BPM_MODULATOR_IMPL_H

#include <ieee802_15_4a/bpsk_bpm_modulator.h>
#include <ieee802_15_4a/lfsr.h>

namespace gr {
  namespace ieee802_15_4a {

    class bpsk_bpm_modulator_impl : public bpsk_bpm_modulator
    {
     private:
      int _Nburst;
      int _Ncpb;
      int _code_index;
      lfsr _lfsr;

     public:
      bpsk_bpm_modulator_impl(int code_index, int Nburst = 32, int Ncpb = 16);
      ~bpsk_bpm_modulator_impl();

      // Where all the action really happens
      void forecast (int noutput_items, gr_vector_int &ninput_items_required);

      int general_work(int noutput_items,
		       gr_vector_int &ninput_items,
		       gr_vector_const_void_star &input_items,
		       gr_vector_void_star &output_items);
    };

  } // namespace ieee802_15_4a
} // namespace gr

#endif /* INCLUDED_IEEE802_15_4A_BPSK_BPM_MODULATOR_IMPL_H */


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
#include "bpsk_bpm_modulator_impl.h"
#include <stdio.h>

namespace gr {
  namespace ieee802_15_4a {

// The ternary codes as defined in page 78 of the standard
	static uint32_t ternary_codes[8] = {
		23930, //101110101111010
		25307, //110001011011011
		20398, //100111110101110
		28145, //110110111110001
		24250, //101111010111010
		23367, //101101101000111
		8699, //010000111111011
		28313, //110111010011001
		};
		
    bpsk_bpm_modulator::sptr
    bpsk_bpm_modulator::make(int code_index, int Nburst, int Ncpb)
    {
      return gnuradio::get_initial_sptr
        (new bpsk_bpm_modulator_impl(code_index, Nburst, Ncpb));
    }

    /*
     * The private constructor
     */
    bpsk_bpm_modulator_impl::bpsk_bpm_modulator_impl(int code_index, int Nburst, int Ncpb)
      : gr::block("bpsk_bpm_modulator",
              gr::io_signature::make(1, 1, sizeof(char)),
              gr::io_signature::make(1, 1, sizeof(char))),
              _code_index(code_index - 1), _Nburst(Nburst), _Ncpb(Ncpb), _lfsr(0x3, ternary_codes[_code_index], 15)
    {
		set_relative_rate ((float)(_Nburst*_Ncpb)/2.);
		set_output_multiple (_Nburst*_Ncpb);
	}

    /*
     * Our virtual destructor.
     */
    bpsk_bpm_modulator_impl::~bpsk_bpm_modulator_impl()
    {
	}
	
	void bpsk_bpm_modulator_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
	{
		ninput_items_required[0] = 2*noutput_items/(_Nburst*_Ncpb);
	}

    int bpsk_bpm_modulator_impl::general_work(int noutput_items,
		       gr_vector_int &ninput_items,
		       gr_vector_const_void_star &input_items,
		       gr_vector_void_star &output_items)
    {
		const char *in = (const char *) input_items[0];
        char *out = (char *) output_items[0];
        int nHops;
        int m;
        int estimated_input_items;
        
        nHops = _Nburst / 4;
        m = log2 (nHops);
        
        char *out_symbol = (char*)malloc (_Nburst * _Ncpb * sizeof(char));
        char *code = (char*)malloc (_Ncpb * sizeof(char));
        int signal;
        int pos;
        
        estimated_input_items = 2*noutput_items/(_Nburst*_Ncpb);
        for (int i = 0; i < estimated_input_items; i += 2)
        {
			char g0 = in[i];
			char g1 = in[i + 1];
			consume_each(2);
			// the bits go and g1 
			if (g0 == 0 && g1 == 0) { signal = 1; pos = 0; }
			if (g0 == 0 && g1 == 1) { signal = -1; pos = 0; }
			if (g0 == 1 && g1 == 0) { signal = 1; pos = 1; }
			if (g0 == 1 && g1 == 1) { signal = -1; pos = 1; }
			
			for (int j = 0; j < _Ncpb; ++j){
				code[j] = signal * (1-2*_lfsr.next_bit());
			}
			int hop = _lfsr.shift_register() >> (15-m);
						
			memset (out_symbol, 0x00, _Nburst * _Ncpb * sizeof(char));
			int idx = (hop + 2 * pos * nHops) * _Ncpb;
			
			for (int j = 0; j < _Ncpb; ++j)
				out_symbol [idx + j] = code[j];
				
			memcpy (out + _Nburst * _Ncpb * (i/2), out_symbol, _Nburst * _Ncpb * sizeof(char));
		}

		free (code);
		free (out_symbol);

        // Tell runtime system how many output items we produced.
        return (estimated_input_items/2)*_Nburst*_Ncpb;
    }

  } /* namespace ieee802_15_4a */
} /* namespace gr */


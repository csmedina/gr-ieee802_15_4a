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
#include "viterbi_impl.h"

#ifdef __cplusplus
extern "C" {
#endif
int
gen_met(
int mettab[2][256],	/* Metric table, [sent sym][rx symbol] */
int amp,		/* Signal amplitude, units */
double esn0,		/* Es/N0 ratio in dB */
double bias,		/* Metric bias; 0 for viterbi, rate for sequential */
int scale		/* Scale factor */
);
int   
viterbi_802154a(   
unsigned long *metric,  /* Final path metric (returned value) */   
unsigned char *data,    /* Decoded output data */   
unsigned char *symbols, /* Raw deinterleaved input symbols */   
unsigned int nbits, /* Number of output bits */   
int mettab[2][256], /* Metric table, [sent sym][rx symbol] */   
unsigned int startstate,         /* Encoder starting state */   
unsigned int endstate            /* Encoder ending state */   
);
#ifdef __cplusplus
};
#endif

namespace gr {
  namespace ieee802_15_4a {

    viterbi::sptr
    viterbi::make(int bypass, int amp, double esn0, int scale)
    {
      return gnuradio::get_initial_sptr
        (new viterbi_impl(bypass, amp, esn0, scale));
    }

    /*
     * The private constructor
     */
    viterbi_impl::viterbi_impl(int bypass, int amp, double esn0, int scale)
      : gr::sync_decimator("viterbi",
              gr::io_signature::make(1, 1, sizeof(char)),
              gr::io_signature::make(1, 1, sizeof(char)), bypass == 1 ? 1 : 2*8),
              _bypass(bypass), _amp(amp), _esn0(esn0), _scale(scale)
    {
		/* Generate metrics analytically, with gaussian pdf */	
		double noise = sqrt(0.5/pow(10.,_esn0/10.));
		gen_met(mettab,_amp,noise, 0., _scale);
	}

    /*
     * Our virtual destructor.
     */
    viterbi_impl::~viterbi_impl()
    {
    }

    int
    viterbi_impl::work(int noutput_items,
			  gr_vector_const_void_star &input_items,
			  gr_vector_void_star &output_items)
    {
        const unsigned char *in = (const unsigned char *) input_items[0];
        unsigned char *out = (unsigned char *) output_items[0];
        unsigned long metric;
        
        if (_bypass)
        {
			memcpy (out, in, noutput_items);
			return noutput_items;
		}
		int ninput_items = noutput_items * 2 * 8;
        viterbi_802154a(&metric, out, (unsigned char*)in, ninput_items, mettab, 0, 0);
        
        // Tell runtime system how many output items we produced.
        return noutput_items;
    }

  } /* namespace ieee802_15_4a */
} /* namespace gr */


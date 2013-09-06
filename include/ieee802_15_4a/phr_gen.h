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


#ifndef INCLUDED_IEEE802_15_4A_SPREADING_H
#define INCLUDED_IEEE802_15_4A_SPREADING_H

#include <ieee802_15_4a/api.h>
#include <gnuradio/sync_interpolator.h>

namespace gr {
  namespace ieee802_15_4a {

    /*!
     * \brief Generates the PHR (Physical header for the standard) - only mandatory mode
     * \ingroup ieee802_15_4a
     *
     */
    class IEEE802_15_4A_API phr_gen
    {
	 private:
		int _frame_len;
     public:
		
		phr_gen ()
		{}
		
		~phr_gen ()
		{}
		
		// Generate the phr, the stream parameter must have the appropriate size (19 see page 75).
		// Receives as arguments the frame length, which is the only variable parameter, the others are the default (see page 75-76)
		void get_phr (char *stream, int frame_len)
		{
			char R1 = 0;
			char R0 = 1;
			char L6 = (frame_len & 0x40) >> 6;
			char L5 = (frame_len & 0x20) >> 5;
			char L4 = (frame_len & 0x10) >> 4;
			char L3 = (frame_len & 0x08) >> 3;
			char L2 = (frame_len & 0x04) >> 2;
			char L1 = (frame_len & 0x02) >> 1;
			char L0 = (frame_len & 0x01);
			char RNG = 0;
			char EXT = 0;
			char P1 = 0;
			char P0 = 1;
			char C5 = R0 ^ R1 ^ L5 ^ L6 ^ C3 ^ C4;
			char C4 = P0 ^ P1;
			char C3 = L0 ^ L1 ^ L2 ^ L3 ^ L4 ^ RNG ^ EXT;
			char C2 = R0 ^ L0 ^ L1 ^ L5 ^ L6 ^ RNG ^ EXT;
			char C1 = R1 ^ L2 ^ L3 ^ L5 ^ L6 ^ RNG ^ EXT ^ P0;
			char C0 = R0 ^ R1 ^ L0 ^ L2 ^ L4 ^ L5 ^ EXT ^ P1;
			
			stream[0] = R1;
			stream[1] = R0;
			stream[2] = L6;
			stream[3] = L5;
			stream[4] = L4;
			stream[5] = L3;
			stream[6] = L2;
			stream[7] = L1;
			stream[8] = L0;
			stream[9] = RNG;
			stream[10] = EXT;
			stream[11] = P1;
			stream[12] = P0;
			stream[13] = C5;
			stream[14] = C4;
			stream[15] = C3;
			stream[16] = C2;
			stream[17] = C1;
			stream[18] = C0;			
		}		
    };

  } // namespace ieee802_15_4a
} // namespace gr

#endif /* INCLUDED_IEEE802_15_4A_SPREADING_H */


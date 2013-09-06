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

	// The ternary codes as defined in page 72 of the standard
	static char ternary_codes[8][31] = {
		{-1,0,0,0,0,1,0,-1,0,1,1,1,0,1,-1,0,0,0,1,-1,1,1,1,0,0,-1,1,0,-1,0,0},
		{0,1,0,1,-1,0,1,0,1,0,0,0,-1,1,1,0,-1,1,-1,-1,-1,0,0,1,0,0,1,1,0,0,0},
		{-1,1,0,1,1,0,0,0,-1,1,-1,1,1,0,0,1,1,0,1,0,0,-1,0,0,0,0,-1,0,1,0,-1},
		{0,0,0,0,1,-1,0,0,-1,0,0,-1,1,1,1,1,0,1,-1,1,0,0,0,1,0,-1,0,1,1,0,-1},
		{-1,0,1,-1,0,0,1,1,1,-1,1,0,0,0,-1,1,0,1,1,1,0,-1,0,1,0,0,0,0,-1,0,0},
		{1,1,0,0,1,0,0,-1,-1,-1,1,-1,0,1,1,-1,0,0,0,1,0,1,0,-1,1,0,1,0,0,0,0},
		{1,0,0,0,0,1,-1,0,1,0,1,0,0,1,0,0,0,1,0,1,1,-1,-1,-1,0,-1,1,0,0,-1,1},
		{0,1,0,0,-1,0,-1,0,1,1,0,0,0,0,-1,-1,1,0,0,-1,1,0,1,1,-1,1,1,0,1,0,0}
		};
    /*!
     * \brief Generates the SHR (Syncronization header for the standard) - only mandatory mode
     * \ingroup ieee802_15_4a
     *
     */
    class IEEE802_15_4A_API shr_gen
    {
	 private:
		int _Nsync;
		int _L;
		int _code_size;
		int _code_index;
     public:
		// Receives as arguments the number of repetitions of Si (Nsync) and the length of delta_L, as explained in pages 73 and 74 of the norm.
		// The values must be according table 39c (page 71) and table 39b (page 70).
		// The parameter code_index is the selected code index in table 39d.
		// Nsync is 64 by standard default
		shr_gen (int Nsync, int L, int code_index)
		{
			_Nsync = Nsync;
			_L = L;
			_code_index = code_index;
			_code_size = 31;
		}
		
		~shr_gen () 
		{
		}
		
		// Generate the sync section of the shr, the stream parameter must have the appropriate size (Nsync * L * 31<the size of the ternary code>).
		void get_sync_section (unsigned char *stream)
		{
			int idx;
			
			memset (stream, 0x00, _Nsync * _L * _code_size);
			for (idx = 0; idx < _code_size; ++idx)
			{
				stream[idx * _L] = ternary_codes[_code_index][idx];
			}
			
			for (idx = 1; idx < _Nsync; ++idx)
			{
				memcpy (stream + idx * _L * _code_size, stream, _L * _code_size);
			}
		}
		
		// Generate the sfd section of the shr, the stream parameter must have the appropriate size (L * 8<the size of the SFD sequence (see page 74)>).
		void get_sfd_section (unsigned char *stream)
		{
			int idx;
			int idx2;
			float seq[8] = {0, 1, 0, -1, 1, 0, 0, -1};
			
			for (idx2 = 0; idx2 < 8; ++idx2)
			{
				for (idx = 0; idx < _code_size; ++idx)
				{
					stream[idx2 * _code_size * _L + idx * _L] = seq[idx2] * ternary_codes[_code_index][idx];
				}
			}
		}
    };

  } // namespace ieee802_15_4a
} // namespace gr

#endif /* INCLUDED_IEEE802_15_4A_SPREADING_H */


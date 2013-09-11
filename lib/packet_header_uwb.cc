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
#include <ieee802_15_4a/packet_header_uwb.h>
#include <stdlib.h>
#include <stdio.h>

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

		packet_header_uwb::packet_header_uwb(int Nsync, int L, int code_index, int code_size, int Nsdf,
		  const std::string &len_tag_key,
		  const std::string &num_tag_key) 
		  : packet_header_default 
		    (
				(Nsync  + Nsdf) * L * code_size,
				len_tag_key,
				num_tag_key,
				1
			), _Nsync(Nsync), _L(L), _code_index(code_index - 1), _code_size(code_size), _Nsdf(Nsdf)
	{
	}
	
	packet_header_uwb::sptr
    packet_header_uwb::make(
        int Nsync, int L, int code_index, int code_size, int Nsdf,
	    const std::string &len_tag_key,
	    const std::string &num_tag_key
		)
    {
      return packet_header_uwb::sptr(
		new packet_header_uwb (Nsync, L, code_index, code_size, Nsdf, len_tag_key, num_tag_key)
      );
    }
    
		
		// Generate the sync section of the shr, the stream parameter must have the appropriate size (Nsync * L * _code_size (only 31 allowed)<the size of the ternary code>).
		void packet_header_uwb::get_sync_section (unsigned char *stream)
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
		
		// Generate the sfd section of the shr, the stream parameter must have the appropriate size (code_size * L * <the size (8-mandatory- or 64-optional-) of the SFD sequence (see page 74)>).
		void packet_header_uwb::get_sfd_section (unsigned char *stream)
		{
			int idx;
			int idx2;
			float seq_short[8] = {0, 1, 0, -1, 1, 0, 0, -1};
			float seq_long[64] = {0, 1, 0, -1, 1, 0, 0, -1,
							  0, 1, 0, -1, 1, 0, 0, -1,
							  -1, 0, 0, 1, 0, -1, 0, 1,
							  0, 1, 0, 0, 0, -1, 0, -1,
							  0, -1, 0, 0, 1, 0, -1, -1, 
							  0, -1, 1, 0, 0, 0, 0, 1,
							  1, 0, 0, -1, -1, -1, 1, -1,
							  1, 1, 0, 0, 0, 0, 1, 1};
			
			float *seq;
			if (_Nsdf == 8)
				seq = seq_short;
			else
				seq = seq_long;
			
			memset (stream, 0x00, _Nsdf * _L * _code_size);
			for (idx2 = 0; idx2 < _Nsdf; ++idx2)
			{
				for (idx = 0; idx < _code_size; ++idx)
				{
					stream[idx2 * _code_size * _L + idx * _L] = seq[idx2] * ternary_codes[_code_index][idx];
				}
			}
		}
		
	bool packet_header_uwb::header_formatter(
		long packet_len,
		unsigned char *out,
		const std::vector<tag_t> &tags
      )
    {
		get_sync_section (out);
		get_sfd_section (out + _Nsync * _L * _code_size);
		return true;
	}
      

    bool packet_header_uwb::header_parser(const unsigned char *header, std::vector<tag_t> &tags)
    {
		return true;
	}

  } /* namespace ieee802_15_4a */
} /* namespace gr */


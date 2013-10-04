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

#include <string.h>
#include <stdio.h>

#include <gnuradio/io_signature.h>
#include <ieee802_15_4a/msg_formatter.h>
#include <ieee802_15_4a/reedsolomon.h>

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
		
    msg_formatter::~msg_formatter()
    {
    }
    
    gr::message::sptr msg_formatter::make_from_string(const std::string s, int Nsync, int DataRate_Kbps, int MeanPRF_KHz, int isRangingPacket)
    {
		reedsolomon rs;
		int ncoded;
		int phy_size;
		
		phy_size = 19;
		ncoded = reedsolomon::outputSize (s.size(), 1);
		std::string msg_str(ncoded + phy_size, 0x00);
		
		rs.encode ((const unsigned char*)s.data(), s.size(), (unsigned char*)msg_str.data());
		
		set_phr (s.size()/8, Nsync, DataRate_Kbps, MeanPRF_KHz, isRangingPacket, (unsigned char *)(msg_str.data() + ncoded));
		
		gr::message::sptr msg_sptr = gr::message::make_from_string(msg_str);
		
		return msg_sptr;
	}
	  
	gr::message::sptr msg_formatter::make_from_string_Bb(const std::string s, int Nsync, int DataRate_Kbps, int MeanPRF_KHz, int isRangingPacket)
	{
		std::string bits (s.size() * 8, 0x00);
		char byte;
		for (int idx = 0; idx < s.size(); ++idx)
		{
			byte = s[idx];
			bits[8*idx] = byte & 0x01;
		    bits[8*idx + 1] = (byte>>1) & 0x01;
		    bits[8*idx + 2] = (byte>>2) & 0x01;
		    bits[8*idx + 3] = (byte>>3) & 0x01;
		    bits[8*idx + 4] = (byte>>4) & 0x01;
		    bits[8*idx + 5] = (byte>>5) & 0x01;
		    bits[8*idx + 6] = (byte>>6) & 0x01;
		    bits[8*idx + 7] = (byte>>7) & 0x01;
		}		
		return make_from_string(bits, Nsync, DataRate_Kbps, MeanPRF_KHz, isRangingPacket);
	}

	void msg_formatter::set_phr (int FrameLength_octal, int Nsync, int DataRate_Kbps, int MeanPRF_KHz, int isRangingPacket, unsigned char *stream)
	{
		char R1;
		char R0;
		char P1;
		char P0;
		
		if (MeanPRF_KHz == 15600 || MeanPRF_KHz == 62400)
		{
			if (DataRate_Kbps == 110){
				R1 = 0;
				R0 = 0;
			}else if (DataRate_Kbps == 850){
				R1 = 0;
				R0 = 1;
			}else if (DataRate_Kbps == 6810){
				R1 = 1;
				R0 = 0;
			}else { //if (DataRate_Kbps == 27240)
				R1 = 1;
				R0 = 1;
			}
		}else if (MeanPRF_KHz == 3900)
		{
			if (DataRate_Kbps == 110){
				R1 = 0;
				R0 = 0;
			}else if (DataRate_Kbps == 850){
				R1 = 0;
				R0 = 1;
			}else if (DataRate_Kbps == 1700){
				R1 = 1;
				R0 = 0;
			}else{ //if (DataRate == 6810)
				R1 = 1;
				R0 = 1;
			}
		}
		char L6 = (FrameLength_octal & 0x40) >> 6;
		char L5 = (FrameLength_octal & 0x20) >> 5;
		char L4 = (FrameLength_octal & 0x10) >> 4;
		char L3 = (FrameLength_octal & 0x08) >> 3;
		char L2 = (FrameLength_octal & 0x04) >> 2;
		char L1 = (FrameLength_octal & 0x02) >> 1;
		char L0 = (FrameLength_octal & 0x01);
		char RNG = isRangingPacket;
		char EXT = 0;
		if (Nsync == 16)
		{
			P1 = 0;
			P0 = 0;
		}else if (Nsync == 64)
		{
			P1 = 0;
			P0 = 1;
		}else if (Nsync == 1024)
		{
			P1 = 1;
			P0 = 0;
		}else //if (Nsync == 4096)
		{
			P1 = 1;
			P0 = 1;
		}
		char C0 = R0 ^ R1 ^ L0 ^ L2 ^ L4 ^ L5 ^ EXT ^ P1;
		char C1 = R1 ^ L2 ^ L3 ^ L5 ^ L6 ^ RNG ^ EXT ^ P0;
		char C2 = R0 ^ L0 ^ L1 ^ L5 ^ L6 ^ RNG ^ EXT;
		char C3 = L0 ^ L1 ^ L2 ^ L3 ^ L4 ^ RNG ^ EXT;
		char C4 = P0 ^ P1;
		char C5 = R0 ^ R1 ^ L5 ^ L6 ^ C3 ^ C4;
		
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

	void msg_formatter::set_sync_section (int code_size, int code_index, int Nsync, int L, unsigned char *stream)
		{
			int idx;
			
			memset (stream, 0x00,Nsync * L * code_size);
			for (idx = 0; idx < code_size; ++idx)
			{
				stream[idx * L] = ternary_codes[code_index][idx];
			}
			
			for (idx = 1; idx < Nsync; ++idx)
			{
				memcpy (stream + idx * L * code_size, stream, L * code_size);
			}
		}
		
		// Generate the sfd section of the shr, the stream parameter must have the appropriate size (code_size * L * <the size (8-mandatory- or 64-optional-) of the SFD sequence (see page 74)>).
		void msg_formatter::set_sfd_section (int code_size, int code_index, int L, int Nsdf, unsigned char *stream)
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
			if (Nsdf == 8)
				seq = seq_short;
			else
				seq = seq_long;
			
			memset (stream, 0x00, Nsdf * L * code_size);
			for (idx2 = 0; idx2 < Nsdf; ++idx2)
			{
				for (idx = 0; idx < code_size; ++idx)
				{
					stream[idx2 * code_size * L + idx * L] = seq[idx2] * ternary_codes[code_index][idx];
				}
			}
		}
  } /* namespace ieee802_15_4a */
} /* namespace gr */


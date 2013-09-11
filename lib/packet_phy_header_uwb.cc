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
#include <ieee802_15_4a/packet_phy_header_uwb.h>

namespace gr {
  namespace ieee802_15_4a {

    packet_phy_header_uwb::packet_phy_header_uwb(int FrameLength, int Nsync, int DataRate, int MeanPRF, int isRangingPacket,
				const std::string &len_tag_key,
				const std::string &num_tag_key)
    : packet_header_default 
		    (
				19, // the size of the header
				len_tag_key,
				num_tag_key,
				1
			), _Nsync(Nsync), _DataRate(DataRate), _FrameLength(FrameLength/8),/*The number of octects in the payload*/
			_isRangingPacket(isRangingPacket), _MeanPRF(MeanPRF)
    {
    }

    packet_phy_header_uwb::~packet_phy_header_uwb()
    {
    }
    
    packet_phy_header_uwb::sptr 
	packet_phy_header_uwb::make(
	    int FrameLength, int Nsync, int DataRate, int MeanPRF, int isRangingPacket,
	    const std::string &len_tag_key,
	    const std::string &num_tag_key
      )
      {
		  return packet_phy_header_uwb::sptr(
			new packet_phy_header_uwb (FrameLength, Nsync, DataRate, MeanPRF, isRangingPacket,
										len_tag_key,
										num_tag_key)
			);
	  }
      
    /*!
	* \brief Encodes the header information in the given tags into bits and places them into \p out
	*/
	bool packet_phy_header_uwb::header_formatter(
		long packet_len,
		unsigned char *out,
		const std::vector<tag_t> &tags
	)
	{
		get_phr (out);
		return true;
	}
  
	/*!
	* \brief Inverse function to header_formatter().
	*
	* Does the same as packet_header_default::header_parser(), but
	* adds another tags from the standard.
	*/
	bool packet_phy_header_uwb::header_parser(const unsigned char *header, std::vector<tag_t> &tags)
	{
		return true;
	}
	
	void packet_phy_header_uwb::get_phr (unsigned char *stream)
	{
		char R1;
		char R0;
		char P1;
		char P0;
		
		if (_MeanPRF == 15600 || _MeanPRF == 62400)
		{
			if (_DataRate == 110){
				R1 = 0;
				R0 = 0;
			}else if (_DataRate == 850){
				R1 = 0;
				R0 = 1;
			}else if (_DataRate == 6810){
				R1 = 1;
				R0 = 0;
			}else { //if (_DataRate == 27240)
				R1 = 1;
				R0 = 1;
			}
		}else if (_MeanPRF == 3900)
		{
			if (_DataRate == 110){
				R1 = 0;
				R0 = 0;
			}else if (_DataRate == 850){
				R1 = 0;
				R0 = 1;
			}else if (_DataRate == 1700){
				R1 = 1;
				R0 = 0;
			}else{ //if (_DataRate == 6810)
				R1 = 1;
				R0 = 1;
			}
		}
		char L6 = (_FrameLength & 0x40) >> 6;
		char L5 = (_FrameLength & 0x20) >> 5;
		char L4 = (_FrameLength & 0x10) >> 4;
		char L3 = (_FrameLength & 0x08) >> 3;
		char L2 = (_FrameLength & 0x04) >> 2;
		char L1 = (_FrameLength & 0x02) >> 1;
		char L0 = (_FrameLength & 0x01);
		char RNG = _isRangingPacket;
		char EXT = 0;
		if (_Nsync == 16)
		{
			P1 = 0;
			P0 = 0;
		}else if (_Nsync == 64)
		{
			P1 = 0;
			P0 = 1;
		}else if (_Nsync == 1024)
		{
			P1 = 1;
			P0 = 0;
		}else //if (_Nsync == 4096)
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

  } /* namespace ieee802_15_4a */
} /* namespace gr */


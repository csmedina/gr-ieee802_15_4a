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


#ifndef INCLUDED_IEEE802_15_4A_PACKET_PHY_HEADER_UWB_H
#define INCLUDED_IEEE802_15_4A_PACKET_PHY_HEADER_UWB_H

#include <vector>
#include <gnuradio/digital/packet_header_default.h>
#include <ieee802_15_4a/api.h>

namespace gr {
  namespace ieee802_15_4a {

    /*!
     * \brief <+description+>
     *
     */
    class IEEE802_15_4A_API packet_phy_header_uwb : public gr::digital::packet_header_default
    {
	public:
      typedef boost::shared_ptr<packet_phy_header_uwb> sptr;
      
    public:
      packet_phy_header_uwb(int FrameLength, int Nsync = 64, int DataRate = 850, int MeanPRF = 15600, int isRangingPacket = 0,
				const std::string &len_tag_key="packet_len",
				const std::string &num_tag_key="packet_num");
      ~packet_phy_header_uwb();
      
      static sptr make(
	    int FrameLength, int Nsync, int DataRate, int MeanPRF, int isRangingPacket,
	    const std::string &len_tag_key="packet_len",
	    const std::string &num_tag_key="packet_num"
      );
      
      /*!
       * \brief Encodes the header information in the given tags into bits and places them into \p out
       */
      virtual bool header_formatter(
		long packet_len,
		unsigned char *out,
		const std::vector<tag_t> &tags=std::vector<tag_t>()
      );
      
      /*!
       * \brief Inverse function to header_formatter().
       *
       * Does the same as packet_header_default::header_parser(), but
       * adds another tags from the standard.
       */
      virtual bool header_parser(const unsigned char *header, std::vector<tag_t> &tags);
      
    private:
		// Generate the phr, the stream parameter must have the appropriate size (19 bytes see page 75).
		// Receives as arguments the frame length, which is the only variable parameter, the others are the default (see page 75-76)
		void get_phr (unsigned char *stream);
		
		
	private:
	  int _Nsync;
	  int _DataRate;
	  int _FrameLength;
	  int _isRangingPacket;
	  int _MeanPRF;	  
    };

  } // namespace ieee802_15_4a
} // namespace gr

#endif /* INCLUDED_IEEE802_15_4A_PACKET_PHY_HEADER_UWB_H */


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


#ifndef INCLUDED_IEEE802_15_4A_PACKET_HEADER_UWB_H
#define INCLUDED_IEEE802_15_4A_PACKET_HEADER_UWB_H

#include <vector>
#include <gnuradio/digital/packet_header_default.h>
#include <ieee802_15_4a/api.h>

namespace gr {
  namespace ieee802_15_4a {

    /*!
     * \brief Header utility for UWB signals from IEEE 802.15.4a standard.
     * Receives as arguments the number of repetitions of Si (Nsync) and the length of delta_L, as explained in pages 73 and 74 of the norm.
     * The values must be according table 39c (page 71) and table 39b (page 70).
     * The parameter code_index is the selected code index in table 39d.
     * Nsync is 64 by standard default
     * \ingroup ofdm_blk
     */
    class IEEE802_15_4A_API packet_header_uwb : public gr::digital::packet_header_default
    {
	public:
      typedef boost::shared_ptr<packet_header_uwb> sptr;
      
    public:
		packet_header_uwb(int Nsync = 64, int L = 16, int code_index = 5, int code_size = 31, int Nsdf = 8,
		      const std::string &len_tag_key="packet_len",
		      const std::string &num_tag_key="packet_num");
		      
		~packet_header_uwb(){}
		
      static sptr make(
	    int Nsync, int L, int code_index, int code_size, int Nsdf,
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
		// Generate the sync section of the shr, the stream parameter must have the appropriate size (Nsync * L * 31<the size of the ternary code>).
		void get_sync_section (unsigned char *stream);		
		// Generate the sfd section of the shr, the stream parameter must have the appropriate size (L * 8<the size of the SFD sequence (see page 74)>).
		void get_sfd_section (unsigned char *stream);
		
	private:
	  int _Nsync;
	  int _L;
	  int _code_index;
	  int _code_size;
	  int _Nsdf;
    };

  } // namespace ieee802_15_4a
} // namespace gr

#endif /* INCLUDED_IEEE802_15_4A_PACKET_HEADER_UWB_H */


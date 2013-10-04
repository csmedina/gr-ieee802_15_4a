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


#ifndef INCLUDED_IEEE802_15_4A_MSG_FORMATTER_H
#define INCLUDED_IEEE802_15_4A_MSG_FORMATTER_H

#include <gnuradio/message.h>
#include <gnuradio/types.h>
#include <ieee802_15_4a/api.h>

namespace gr {
  namespace ieee802_15_4a {

    /*!
     * \brief <+description+>
     *
     */
    class IEEE802_15_4A_API msg_formatter
    {
		public:
			typedef boost::shared_ptr<msg_formatter> sptr;
      
	private:
      
      msg_formatter();      
      
   public:
      static void set_phr (int FrameLength_octal, int Nsync, int DataRate_Kbps, int MeanPRF_KHz, int isRangingPacket, unsigned char *stream);      
      // Generate the sync section of the shr, the stream parameter must have the appropriate size (Nsync * L * 31<the size of the ternary code>).
	  static void set_sync_section (int code_size, int code_index, int Nsync, int L, unsigned char *stream);		
	  // Generate the sfd section of the shr, the stream parameter must have the appropriate size (L * 8<the size of the SFD sequence (see page 74)>).
	  static void set_sfd_section (int code_size, int code_index, int L, int Nsdf, unsigned char *stream);
	  // return the size of the synch section
	  static int get_synch_section_len (int code_size, int L, int Nsdf, int Nsync)
	  {
		  return (Nsync  + Nsdf) * L * code_size;
	  }
	  static std::string set_synch(int code_size, int code_index, int Nsync, int L, int Nsdf)
	  {
		  int len = get_synch_section_len(code_size, L,  Nsdf, Nsync);
		  std::string header (len, 0x00);
		  
		set_sync_section (code_size, code_index, Nsync, L, (unsigned char*)header.data());
		set_sfd_section (code_size, code_index, L, Nsdf, (unsigned char*)header.data() + Nsync * L * code_size);
		
		return header;
	  }
      
    public:      
		static gr::message::sptr make_from_string(const std::string s, int Nsync, int DataRate_Kbps, int MeanPRF_KHz, int isRangingPacket);
		static gr::message::sptr make_from_string_Bb(const std::string s, int Nsync, int DataRate_Kbps, int MeanPRF_KHz, int isRangingPacket);
      
      ~msg_formatter();
    };

  } // namespace ieee802_15_4a
} // namespace gr

#endif /* INCLUDED_IEEE802_15_4A_MSG_FORMATTER_H */


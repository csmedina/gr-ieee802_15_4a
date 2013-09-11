/*
 * Copyright 2011 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifndef INCLUDED_IEEE802_15_4A_FEC_RS_H
#define INCLUDED_IEEE802_15_4A_FEC_RS_H

#include <ieee802_15_4a/api.h>

namespace gr {
  namespace ieee802_15_4a {
	  
	  class IEEE802_15_4A_API ReedSolomon 
	  {
		  public:
			ReedSolomon () {};
			~ReedSolomon () {};
			static void *OpenHandle ();
			static void CloseHandle (void *handle);
			static void encode (const unsigned char *msg, int len, unsigned char *codedMsg, void *handle);
			static void decode (const unsigned char *codedMsg, int len, unsigned char *msg, void *handle);
	  };
	  
  } /* namespace ieee802_15_4a */
} /* namespace gr */

#endif /* INCLUDED_IEEE802_15_4A_FEC_RS_H */

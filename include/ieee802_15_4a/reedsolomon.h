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


#ifndef INCLUDED_IEEE802_15_4A_REEDSOLOMON_H
#define INCLUDED_IEEE802_15_4A_REEDSOLOMON_H

#include <ieee802_15_4a/api.h>
#include <gnuradio/types.h>

namespace gr {
  namespace ieee802_15_4a {

    /*!
     * \brief <+description+>
     *
     */
    class IEEE802_15_4A_API reedsolomon
    {
    public:
      reedsolomon();
      ~reedsolomon();
      
      static int outputSize (int ninput, int encode);
      
      void encode (const unsigned char *msg, int len, unsigned char *codedMsg);	  
	  void decode (const unsigned char *codedMsg, int len, unsigned char *msg);
	  
    private:
	  void *handle;
      
      void int_encode (const unsigned char *msg, int len, unsigned char *codedMsg);	  
	  void int_decode (const unsigned char *codedMsg, int len, unsigned char *msg);
    };

  } // namespace ieee802_15_4a
} // namespace gr

#endif /* INCLUDED_IEEE802_15_4A_REEDSOLOMON_H */


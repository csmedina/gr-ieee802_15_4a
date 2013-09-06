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


#include "qa_fec_rs.h"
#include <cppunit/TestAssert.h>

#include <ieee802_15_4a/fec_rs.h>

namespace gr {
  namespace ieee802_15_4a {

    void
    qa_fec_rs::t1()
    {
		unsigned char msg[] = {1,0,1,0,1,0,1,0,1,1,1,0,1,0,1,0,0,1,0,0,0,0,1,0,0,0,0,0,0,1,0,0,1,1,1,0,1,1,1,0,1,0,1,0,0,1,1,0,0,0,1,1,0,1,1,0,1,1,0,0,0,1,1,0,1,1,1,1,0,1,1,0,1,0,1,1,0,1,1,0,1,0,1,0,0,1,1,0,1,1,0,0,1,1,1,0,0,0,0,0,0,1,0,0,1,0,0,1,0,0,1,0,1,0,1,0,0,0,1,0,1,0,1,0,0,0,1,0,1,0,1,0,0,0,1,0};
		unsigned char codedMsg[184];
		unsigned char decodedMsg[136];

		ReedSolomon fec;
		
		fprintf (stdout, "\n");
		fec.encode (msg, 136, codedMsg);
		//for (int i = 0; i < 184; ++i)
			//fprintf (stdout, "%d;", codedMsg[i]);
		
		fprintf (stdout, "\n");			
		fec.decode (codedMsg, 184, decodedMsg);
		//for (int i = 0; i < 136; ++i)
			//fprintf (stdout, "msg = %d; decoded = %d\n", msg[i], decodedMsg[i]);
		
		for (int i = 0; i < 136; ++i)
			CPPUNIT_ASSERT (decodedMsg[i] == msg[i]);
			
    }

  } /* namespace ieee802_15_4a */
} /* namespace gr */


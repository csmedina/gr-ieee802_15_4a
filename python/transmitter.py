#!/usr/bin/env python
# -*- coding: utf-8 -*-
# 
# Copyright 2013 <+YOU OR YOUR COMPANY+>.
# 
# This is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
# 
# This software is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this software; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.
# 

from gnuradio import gr
import ieee802_15_4a_swig as ieee802_15_4a
from gnuradio import blocks
from gnuradio import digital

class transmitter(gr.hier_block2):
    """
    IEEE 802.15.4a UWB modulator.

    Send packets by calling send_pkt.
    """
    def __init__(self, DataRate, bypass_conv_enc, code_index, code_size, MeanPRF, Nsync, deltaL, Nsdf, isRangingPacket, Nburst, Ncpb):
	    """
	    Hierarchical block for the 802-15-4a UWB  modulation.

        Packets to be sent are enqueued by calling send_pkt.
        The output is the complex modulated signal at baseband.

        @param msgq_limit: maximum number of messages in message queue
        @type msgq_limit: int
        
        See 802_15_4_mod for remaining parameters
        """
	    gr.hier_block2.__init__(self,
            "transmitter",
            gr.io_signature(1, 1, gr.sizeof_char),  # Input signature
            gr.io_signature(1, 1, gr.sizeof_char))  # Output signature
        
        ####### Define blocks 
        # reed solomon encoder
	    rs_encoder = ieee802_15_4a.reedsolomon_encoder ();
        
	    # reed solomon encoder
	    self.connect (self, rs_encoder, self)

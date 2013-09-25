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

from gnuradio import gr, gr_unittest
from transmitter import transmitter
from gnuradio import blocks
from gnuradio.digital.utils import tagged_streams

class qa_transmitter (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()

    def tearDown (self):
        self.tb = None

    def test_001_t (self):
        DataRate = 850;
        bypass_conv_enc = 0;
        code_index = 5;
        code_size = 31;  # the default code size (codes of size 127 are not supported)
        MeanPRF = 15600;
        Nsync = 64;
        deltaL = 16;
        Nsdf = 8;
        isRangingPacket = 0; # Only supported non-ranging packets
        Nburst = 32;
        Ncpb = 16;
        
        # define the blocks
        #bytes = bytearray(msg)
        #unpack_bytes = blocks.packed_to_unpacked_bb (1, gr.GR_LSB_FIRST);
        bytes = (1,0,1,0,0,1,0,1,0,1,0,1,0,1,0,1);
        FrameLength = len(bytes);
        blocks_vector_source = blocks.vector_source_b(bytes, False, 1, tagged_streams.make_lengthtags((FrameLength,), (0,), "packet_len"));
        tx = transmitter (FrameLength, DataRate, bypass_conv_enc, code_index, code_size, MeanPRF, Nsync, deltaL, Nsdf, isRangingPacket, Nburst, Ncpb)
        sink = blocks.file_sink (1, "output_tx.txt")
        
        # connect them
        self.tb.connect (blocks_vector_source, tx)
        self.tb.connect (tx, sink)
        
        # set up fg
        self.tb.run ()
        # check data
        

if __name__ == '__main__':
    gr_unittest.run(qa_transmitter, "qa_transmitter.xml")

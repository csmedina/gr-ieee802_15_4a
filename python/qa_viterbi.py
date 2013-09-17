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
import ieee802_15_4a_swig as ieee802_15_4a
from gnuradio import blocks
from gnuradio.digital.utils import tagged_streams
from random import *

randBinList = lambda n: [randint(0,1) for b in range(1,n+1)]

class qa_viterbi (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()

    def tearDown (self):
        self.tb = None

    def test_001_t (self):
        raw_data = (0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 0, 0, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 0, 1, 0, 1, 1, 0, 1, 1, 0, 1, 0, 1, 0, 0, 1, 1, 0, 1, 1, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 1, 0, 1, 1, 1, 1, 0, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 0, 1, 1, 1, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0);
        encoded =  (28,28,28,228,228,28,28,228,28,228,228,28,28,228,28,28,28,228,228,28,28,228,28,28,28,228,228,228,228,28,228,228,28,228,28,228,228,228,228,28,228,228,28,28,228,28,28,28,228,28,28,28,228,28,28,28,228,228,228,28,228,228,28,28,228,28,28,28,228,28,28,228,28,228,228,28,28,228,28,28,28,28,28,228,228,28,28,228,28,28,28,28,28,28,28,28,28,228,228,28,28,228,28,228,228,228,228,28,228,228,28,28,228,228,228,28,228,228,28,28,228,28,28,28,228,28,28,228,28,228,228,228,228,228,28,228,28,28,28,228,228,228,228,228,28,28,228,228,228,228,28,28,228,228,228,228,28,228,28,28,28,228,228,228,228,228,28,28,228,228,228,28,228,28,228,228,28,28,228,228,228,228,28,28,228,28,28,28,228,228,228,228,28,28,228,228,228,228,28,28,228,28,28,28,228,28,28,228,28,228,228,228,228,228,28,28,228,228,228,228,28,228,28,228,228,228,228,28,228,228,28,228,28,28,28,28,28,28,28,28,28,228,228,28,28,228,28,228,228,28,28,228,28,228,228,28,28,228,28,228,228,28,28,28,228,28,28,28,228,28,28,228,28,28,28,228,228,28,28,28,228,28,28,28,228,28,28,228,28,28,28,228,228,28,28,28,228,28,28,28,228,28,28,228,28,28,28,228,228,28,28,228,28,28,28,228,228,228,228,228,28,28,228,228,228,228,28,228,28,28,28,228,228,228,228,28,228,228,28,228,28,228,228,28,28,28,228,228,228,28,228,28,228,228,28,28,228,28,28,228,28,228,228,228,228,28,228,28,228,28,228,28,228,28,228,228,28,228,28,228,228,228,228,28,228,228,28,28,228,228,228,28,228,228,28,228,28,228,228,28,28,28,228,28,28,228,28,28,28,28,28,28,28,28,28,28);
        sz = len(encoded)
        #print (sz)
                        
        self.blocks_vector_source = blocks.vector_source_b(encoded, False, 1, tagged_streams.make_lengthtags((sz,), (0,), "packet_len"));
        self.decoder = ieee802_15_4a.viterbi (0, 100, 5, 4);
        self.blocks_unpacket_bits = blocks.unpack_k_bits_bb(8);
        
        self.sink = blocks.vector_sink_b(1)
        self.sink_outfile = blocks.file_sink (1, "output_out_viterbi.txt")
        
        self.tb.connect (self.blocks_vector_source, self.decoder)
        self.tb.connect (self.decoder, self.blocks_unpacket_bits)
        self.tb.connect (self.blocks_unpacket_bits, self.sink)
        
        #self.tb.connect (self.blocks_vector_source, self.blocks_unpacket_bits)
        #self.tb.connect (self.decoder, self.sink)
        
        self.tb.connect (self.blocks_unpacket_bits, self.sink_outfile)
        
        # set up fg
        self.tb.run ()
        # check data
        decoded_data = self.sink.data()
        #print (len(raw_data))
        #print (len(decoded_data))
        # check data
        self.assertEqual(raw_data, decoded_data)


if __name__ == '__main__':
    gr_unittest.run(qa_viterbi, "qa_viterbi.xml")

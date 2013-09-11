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
from gnuradio import digital
from gnuradio import blocks
from gnuradio.digital.utils import tagged_streams

class qa_packet_phy_header_uwb (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()

    def tearDown (self):
        self.tb = None

    def test_001_t (self):
        FrameLength = 96
        self.header_formatter = header_formatter = ieee802_15_4a.packet_phy_header_uwb(FrameLength, 64, 850, 15600,0)
        
        self.blocks_vector_source = blocks.vector_source_b(range(FrameLength), False, 1, tagged_streams.make_lengthtags((FrameLength,), (0,), "packet_len"))        
        self.blocks_tagged_stream_mux = blocks.tagged_stream_mux(1, "packet_len")        
        self.digital_packet_headergenerator_uwb = digital.packet_headergenerator_bb(header_formatter.formatter())
        self.sink = blocks.file_sink (1, "output_phy_uwb.txt")
        
        self.tb.connect((self.blocks_vector_source, 0), (self.digital_packet_headergenerator_uwb, 0))
        self.tb.connect((self.blocks_vector_source, 0), (self.blocks_tagged_stream_mux, 1))
        self.tb.connect((self.digital_packet_headergenerator_uwb, 0), (self.blocks_tagged_stream_mux, 0))
        self.tb.connect((self.blocks_tagged_stream_mux, 0), (self.sink, 0))
        
        self.tb.run ()


if __name__ == '__main__':
    gr_unittest.run(qa_packet_phy_header_uwb, "qa_packet_phy_header_uwb.xml")

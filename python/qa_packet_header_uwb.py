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

from gnuradio import gr, gr_unittest, blocks
import ieee802_15_4a_swig as ieee802_15_4a
from gnuradio import digital
        
class qa_packet_header_uwb (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()

    def tearDown (self):
        self.tb = None

    def test_001_t (self):
        self.header_formatter = header_formatter = ieee802_15_4a.packet_header_uwb(16, 16, 5, 31, 8)
        #self.header_formatter = digital.packet_header_default (1)
        # set up fg
        #----------self.length_tag_name = length_tag_name = "packet_len"
        #----------self.num_tag_key = num_tag_key = "packet_num"
        #----------, length_tag_name, num_tag_key)
        
        #self.occupied_carriers = occupied_carriers = (range(-26, -21) + range(-20, -7) + range(-6, 0) + range(1, 7) + range(8, 21) + range(22, 27),)
        #self.header_formatter = header_formatter = digital.packet_header_ofdm(occupied_carriers, 1)
        
        self.digital_packet_headergenerator_uwb = digital.packet_headergenerator_bb(header_formatter.formatter())        


if __name__ == '__main__':
    gr_unittest.run(qa_packet_header_uwb, "qa_packet_header_uwb.xml")

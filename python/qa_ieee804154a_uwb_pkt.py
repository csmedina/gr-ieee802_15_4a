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

from random import *

from gnuradio import gr, gr_unittest
from gnuradio import blocks

import ieee802_15_4a_swig as ieee802_15_4a
from ieee804154a_uwb_pkt import ieee804154a_uwb_mod_pkt

randBinList = lambda n: [randint(0,1) for b in range(1,n+1)]

class qa_ieee804154a_uwb_pkt (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()

    def tearDown (self):
        self.tb = None

    def test_001_t (self):
        self.modulator = ieee804154a_uwb_mod_pkt (5);
        self.sink = blocks.file_sink (1, "output_tx.txt")
        
        self.tb.connect (self.modulator, self.sink)
        self.modulator.send_pkt('esta é minha menssagem')
        # set up fg
        self.tb.start ()
        

if __name__ == '__main__':
    gr_unittest.run(qa_ieee804154a_uwb_pkt, "qa_ieee804154a_uwb_pkt.xml")

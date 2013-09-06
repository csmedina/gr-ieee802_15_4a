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

class qa_spreading (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()

    def tearDown (self):
        self.tb = None

    def test_001_t (self):
        # set up fg
        data = [complex(1, 1), complex(-1, -1)]
        code = [complex(1, 0), complex(-1, 0), complex(-1, 0), complex(1, 0), complex(1, 0)]
        expected_result = [complex(1, 1), complex(-1, -1), complex(-1, -1), complex(1, 1), complex(1, 1), complex(-1, -1), complex(1, 1), complex(1, 1), complex(-1, -1), complex(-1, -1)]
        
        src = blocks.vector_source_c(data)
        spread = ieee802_15_4a.spreading(code)
        dst = blocks.vector_sink_c()
        
        self.tb.connect(src, spread)
        self.tb.connect(spread, dst)
        self.tb.run ()
        result_data = dst.data()
        # check data
        self.assertFloatTuplesAlmostEqual(expected_result, result_data, 10)

if __name__ == '__main__':
    gr_unittest.run(qa_spreading, "qa_spreading.xml")

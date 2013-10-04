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

from gnuradio import gr
from gnuradio import eng_notation
from gnuradio import blocks
from gnuradio import digital

import copy
import sys

import ieee802_15_4a_swig as ieee802_15_4a
from ieee804154a_uwb_pkt import ieee804154a_uwb_mod_pkt

randBinList = lambda n: [randint(0,1) for b in range(1,n+1)]

class transmit_path (gr.top_block):
    def __init__(self, code_index, code_size, DataRate_Kbps, Nburst, Ncpb, MeanPRF_KHz, Nsync, deltaL, Nsdf, bypass_conv_enc, isRangingPacket):
        gr.top_block.__init__(self)
        
        self.code_index = code_index
        self.code_size = code_size
        self.DataRate_Kbps = DataRate_Kbps
        self.Nburst = Nburst
        self.Ncpb = Ncpb
        self.MeanPRF_KHz = MeanPRF_KHz
        self.Nsync = Nsync
        self.deltaL = deltaL
        self.Nsdf = Nsdf
        self.bypass_conv_enc = bypass_conv_enc
        self.isRangingPacket = isRangingPacket
        
        self.synch_seq = ieee802_15_4a.msg_formatter.set_synch(code_size, code_index, Nsync, deltaL, Nsdf)
      
        self.modulator = ieee804154a_uwb_mod_pkt (5)
        
        self.sink = blocks.file_sink (1, "output_tx.txt")
        
        self.connect (self.modulator, self.sink)
        
    def send_pkt(self, payload='', eof=False):
        """
        Calls the transmitter method to send a packet
        """
        return self.modulator.send_pkt(payload, self.Nsync, self.DataRate_Kbps, self.MeanPRF_KHz, self.isRangingPacket, eof)
        
def main():
    DataRate_Kbps = 850;
    bypass_conv_enc = 0;
    code_index = 5;
    code_size = 31;  # the default code size (codes of size 127 are not supported)
    MeanPRF_KHz = 15600;
    Nsync = 64;
    deltaL = 16;
    Nsdf = 8;
    isRangingPacket = 0; # Only supported non-ranging packets
    Nburst = 32;
    Ncpb = 16;
    
    put = transmit_path(code_index, code_size, DataRate_Kbps, Nburst, Ncpb, MeanPRF_KHz, Nsync, deltaL, Nsdf, bypass_conv_enc, isRangingPacket )
    put.start()
    put.send_pkt ('esta é minha menssagem')
    put.wait()

    
if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        pass

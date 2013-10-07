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

import struct
from gnuradio import gr
from gnuradio import blocks
import gnuradio.gr.gr_threading as _threading

import ieee802_15_4a_swig as ieee802_15_4a

# /////////////////////////////////////////////////////////////////////////////
#                   mod/demod with packets as i/o
# /////////////////////////////////////////////////////////////////////////////

class ieee804154a_uwb_mod_pkt(gr.hier_block2):
    """
    IEEE 802.15.4a UWB modulator that is a GNU Radio source.

    Send packets by calling send_pkt
    """
    def __init__(self, code_index, code_size=31, DataRate_Kbps=850, Nburst=32, Ncpb=16, MeanPRF_KHz=15600, Nsync=64, deltaL=16, Nsdf=8, bypass_conv_enc=0, isRangingPacket=0, msgq_limit=2):
        """
	Hierarchical block for the IEEE 802.15.4a UWB modulation.

        Packets to be sent are enqueued by calling send_pkt.
        The output is the complex modulated signal at baseband.

	@param fg: flow graph
	@type fg: flow graph
        @param msgq_limit: maximum number of messages in message queue
        @type msgq_limit: int
        @param pad_for_usrp: If true, packets are padded such that they end up a multiple of 128 samples
        @param bypass_conv_enc: bypass the convolutional encoding (1) or not (0)[default]
        @type bypass_conv_enc: int
        
        """
        gr.hier_block2.__init__(self, "ieee804154a_uwb_mod_pkt",
				gr.io_signature(0, 0, 0),                    # Input signature
				gr.io_signature(1, 1, gr.sizeof_char)) # Output signature
				        
        # setting parameters
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
        self.isRangingPacket = bypass_conv_enc
        
        ############ THE SYNCH PATH
        # synchronization header
        self.synch_seq = ieee802_15_4a.msg_formatter.set_synch(self.code_size, self.code_index, self.Nsync, self.deltaL, self.Nsdf)
        self.synch_msg = gr.message_from_string(self.synch_seq)
        self._synch_input = blocks.message_source(gr.sizeof_char, msgq_limit)
        
        ############ THE MAIN PATH
        # accepts messages from the outside world
        self._pkt_input = blocks.message_source(gr.sizeof_char, msgq_limit)
        
        # convolutional encoding
        self._convolutional_encoder = ieee802_15_4a.conv_encoder (self.bypass_conv_enc);
        
        # BPSK BPM modulator
        self._modulator = ieee802_15_4a.bpsk_bpm_modulator(self.code_index, self.Nburst, self.Ncpb)
        
        # connect the blocks
        self.connect(self._pkt_input, self._convolutional_encoder, self._modulator)
        
        # THE CONNECTION BETWEEN THE PATHS
        self.delay = blocks.delay (gr.sizeof_char, len(self.synch_seq))
        self.sum2 = blocks.or_bb(1);
        
        self.connect (self._synch_input, (self.sum2, 0))
        self.connect (self._modulator, self.delay)
        self.connect (self.delay, (self.sum2, 1))
        self.connect (self.sum2, self)
        
    def send_pkt(self, payload='', eof=False):
        """
        Send the payload.

        Args:
            payload: data to send (string)
        """

        if eof:
            msg = gr.message(1) # tell self._pkt_input we're not sending any more packets
        else:
            msg = ieee802_15_4a.make_from_string_Bb(payload, self.Nsync, self.DataRate_Kbps, self.MeanPRF_KHz, self.isRangingPacket)
        
        # insert the synch header
        self._synch_input.msgq().insert_tail(self.synch_msg)
        # insert the tail to complete the transmission
        sizepayload = len(msg.to_string()) * self.Nburst * self.Ncpb;
        t = struct.pack ('B', 0)
        tail = gr.message_from_string(t * sizepayload)
        self._synch_input.msgq().insert_tail(tail)
        
        # insert the payload
        self._pkt_input.msgq().insert_tail(msg)

class _queue_watcher_thread(_threading.Thread):
    def __init__(self, rcvd_pktq, callback):
        _threading.Thread.__init__(self)
        self.setDaemon(1)
        self.rcvd_pktq = rcvd_pktq
        self.callback = callback
        self.keep_running = True
        self.start()


    def run(self):
        while self.keep_running:
            msg = self.rcvd_pktq.delete_head()
            ok, payload = packet_utils.unmake_packet(msg.to_string(), int(msg.arg1()))
            if self.callback:
                self.callback(ok, payload)



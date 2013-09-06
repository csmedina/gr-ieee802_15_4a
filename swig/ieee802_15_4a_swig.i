/* -*- c++ -*- */

#define IEEE802_15_4A_API

%include "gnuradio.i"			// the common stuff

//load generated python docstrings
%include "ieee802_15_4a_swig_doc.i"

%{
#include "ieee802_15_4a/spreading.h"
#include "ieee802_15_4a/despreading.h"
#include "ieee802_15_4a/packet_header_uwb.h"
%}


%include "ieee802_15_4a/spreading.h"
GR_SWIG_BLOCK_MAGIC2(ieee802_15_4a, spreading);
%include "ieee802_15_4a/despreading.h"
GR_SWIG_BLOCK_MAGIC2(ieee802_15_4a, despreading);
%include "ieee802_15_4a/packet_header_uwb.h"
GR_SWIG_BLOCK_MAGIC2(ieee802_15_4a, packet_header_uwb);


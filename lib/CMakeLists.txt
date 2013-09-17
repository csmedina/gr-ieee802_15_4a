# Copyright 2011,2012 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
#
# GNU Radio is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with GNU Radio; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.

########################################################################
# Setup library
########################################################################
include(GrPlatform) #define LIB_SUFFIX

include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIRS})
list(APPEND ieee802_15_4a_sources
    spreading_impl.cc
    despreading_impl.cc
    packet_header_uwb.cc
    packet_phy_header_uwb.cc
    fec_rs.cc
    reedsolomon_encoder_impl.cc
    reedsolomon_decoder_impl.cc
    viterbi/metrics.c viterbi/viterbi.c viterbi/tab.c
    conv_encoder_impl.cc
    viterbi_impl.cc )

add_library(gnuradio-ieee802_15_4a SHARED ${ieee802_15_4a_sources})
target_link_libraries(gnuradio-ieee802_15_4a ${Boost_LIBRARIES} ${GNURADIO_RUNTIME_LIBRARIES} gnuradio-fec gnuradio-digital gnuradio-runtime gnuradio-pmt)
set_target_properties(gnuradio-ieee802_15_4a PROPERTIES DEFINE_SYMBOL "gnuradio_ieee802_15_4a_EXPORTS")

########################################################################
# Install built library files
########################################################################
install(TARGETS gnuradio-ieee802_15_4a
    LIBRARY DESTINATION lib${LIB_SUFFIX} # .so/.dylib file
    ARCHIVE DESTINATION lib${LIB_SUFFIX} # .lib file
    RUNTIME DESTINATION bin              # .dll file
)

########################################################################
# Build and register unit test
########################################################################
include(GrTest)

include_directories(${CPPUNIT_INCLUDE_DIRS})
list(APPEND test_ieee802_15_4a_sources
    ${CMAKE_CURRENT_SOURCE_DIR}/test_ieee802_15_4a.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/qa_ieee802_15_4a.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/qa_packet_phy_header_uwb.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/qa_packet_header_uwb.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/qa_despreading.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/qa_spreading.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/qa_fec_rs.cc
)

add_executable(test-ieee802_15_4a ${test_ieee802_15_4a_sources})

target_link_libraries(
  test-ieee802_15_4a
  ${GNURADIO_RUNTIME_LIBRARIES}
  ${Boost_LIBRARIES}
  ${CPPUNIT_LIBRARIES}
  gnuradio-ieee802_15_4a
  gnuradio-fec 
  gnuradio-digital
  gnuradio-runtime 
  gnuradio-pmt
)

GR_ADD_TEST(test_ieee802_15_4a test-ieee802_15_4a)

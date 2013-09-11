/*
 * Copyright 2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

/*
 * This class gathers together all the test cases for the gr-filter
 * directory into a single test suite.  As you create new test cases,
 * add them here.
 */

#include "qa_ieee802_15_4a.h"
#include "qa_spreading.h"
#include "qa_despreading.h"
#include "qa_fec_rs.h"
#include "qa_packet_header_uwb.h"
#include "qa_packet_phy_header_uwb.h"

CppUnit::TestSuite *
qa_ieee802_15_4a::suite()
{
  CppUnit::TestSuite *s = new CppUnit::TestSuite("ieee802_15_4a");
  s->addTest(gr::ieee802_15_4a::qa_spreading::suite());
  s->addTest(gr::ieee802_15_4a::qa_despreading::suite());
  s->addTest(gr::ieee802_15_4a::qa_fec_rs::suite());
  s->addTest(gr::ieee802_15_4a::qa_packet_header_uwb::suite());
  s->addTest(gr::ieee802_15_4a::qa_packet_phy_header_uwb::suite());

  return s;
}

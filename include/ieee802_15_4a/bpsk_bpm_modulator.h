/* -*- c++ -*- */
/* 
 * Copyright 2013 <+YOU OR YOUR COMPANY+>.
 * 
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */


#ifndef INCLUDED_IEEE802_15_4A_BPSK_BPM_MODULATOR_H
#define INCLUDED_IEEE802_15_4A_BPSK_BPM_MODULATOR_H

#include <ieee802_15_4a/api.h>
#include <gnuradio/tagged_stream_block.h>

namespace gr {
  namespace ieee802_15_4a {

    /*!
     * \brief <+description of block+>
     * \ingroup ieee802_15_4a
     *
     */
    class IEEE802_15_4A_API bpsk_bpm_modulator : virtual public gr::tagged_stream_block
    {
     public:
      typedef boost::shared_ptr<bpsk_bpm_modulator> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of ieee802_15_4a::bpsk_bpm_modulator.
       *
       * To avoid accidental use of raw pointers, ieee802_15_4a::bpsk_bpm_modulator's
       * constructor is in a private implementation
       * class. ieee802_15_4a::bpsk_bpm_modulator::make is the public interface for
       * creating new instances.
       */
      static sptr make(int code_index, int Nburst = 32, int Ncpb = 16, const std::string& len_tag_key="packet_len");
    };

  } // namespace ieee802_15_4a
} // namespace gr

#endif /* INCLUDED_IEEE802_15_4A_BPSK_BPM_MODULATOR_H */


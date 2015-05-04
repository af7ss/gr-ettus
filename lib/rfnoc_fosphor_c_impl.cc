/* -*- c++ -*- */
/* Copyright 2015 Ettus Research
 * 
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * gr-ettus is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with gr-ettus; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include "rfnoc_fosphor_c_impl.h"
#include <uhd/usrp/rfnoc/fosphor_block_ctrl.hpp>

namespace gr {
  namespace ettus {

    rfnoc_fosphor_c::sptr
    rfnoc_fosphor_c::make(
        const int fft_size,
        const device3::sptr &dev,
        const int block_select,
        const int device_select
    ) {
      return gnuradio::get_initial_sptr(
          new rfnoc_fosphor_c_impl(
            fft_size, dev, block_select, device_select
          )
      );
    }

    static ::uhd::stream_args_t
    _make_stream_args(const char *host_type, const char *otw_type, size_t spp)
    {
      ::uhd::stream_args_t stream_args(host_type, otw_type);
      stream_args.args["spp"] = str(boost::format("%s") % spp);
      return stream_args;
    }

    rfnoc_fosphor_c_impl::rfnoc_fosphor_c_impl(
        const int fft_size,
        const device3::sptr &dev,
        const int block_select,
        const int device_select
    ) : rfnoc_block("rfnoc_fosphor_c"),
        rfnoc_block_impl(
            dev,
            rfnoc_block_impl::make_block_id("fosphor", block_select, device_select),
            _make_stream_args("fc32", "sc16", fft_size),
            _make_stream_args("u8",   "u8",   fft_size)
        )
    {
      message_port_register_in(pmt::mp("cfg"));
      set_msg_handler(
        pmt::mp("cfg"),
        boost::bind(&rfnoc_fosphor_c_impl::handle_cfg_message, this, _1)
      );
    }

    rfnoc_fosphor_c_impl::~rfnoc_fosphor_c_impl()
    {
      /* nop */
    }


    void rfnoc_fosphor_c_impl::clear()
    {
      get_block_ctrl_throw< ::uhd::rfnoc::fosphor_block_ctrl >()->clear();
    }

    void rfnoc_fosphor_c_impl::set_decim(const int decim)
    {
      get_block_ctrl_throw< ::uhd::rfnoc::fosphor_block_ctrl >()->set_decim(decim);
    }

    void rfnoc_fosphor_c_impl::set_offset(const int offset)
    {
      get_block_ctrl_throw< ::uhd::rfnoc::fosphor_block_ctrl >()->set_offset(offset);
    }

    void rfnoc_fosphor_c_impl::set_scale(const int scale)
    {
      get_block_ctrl_throw< ::uhd::rfnoc::fosphor_block_ctrl >()->set_scale(scale);
    }

    void rfnoc_fosphor_c_impl::set_trise(const int trise)
    {
      get_block_ctrl_throw< ::uhd::rfnoc::fosphor_block_ctrl >()->set_trise(trise);
    }

    void rfnoc_fosphor_c_impl::set_tdecay(const int tdecay)
    {
      get_block_ctrl_throw< ::uhd::rfnoc::fosphor_block_ctrl >()->set_tdecay(tdecay);
    }

    void rfnoc_fosphor_c_impl::set_alpha(const int alpha)
    {
      get_block_ctrl_throw< ::uhd::rfnoc::fosphor_block_ctrl >()->set_alpha(alpha);
    }

    void rfnoc_fosphor_c_impl::set_epsilon(const int epsilon)
    {
      get_block_ctrl_throw< ::uhd::rfnoc::fosphor_block_ctrl >()->set_epsilon(epsilon);
    }


    void rfnoc_fosphor_c_impl::handle_cfg_message(pmt::pmt_t msg)
    {
      /* If the PMT is a list, assume it's a list of pairs and recurse for each */
      /* Works for dict too */
      try {
        /* Because of PMT is just broken you and can't distinguish between
         * pair and dict, we have to call length() and see if it will throw
         * or not ... */
        if (pmt::length(msg) > 0)
        {
          for (int i=0; i<pmt::length(msg); i++)
            this->handle_cfg_message(pmt::nth(i, msg));
          return;
        }
      } catch(...) { }

      /* Handle the pairs */
      if (pmt::is_pair(msg))
      {
        pmt::pmt_t key(pmt::car(msg));
        pmt::pmt_t val(pmt::cdr(msg));
        int val_i;

        if (!pmt::is_symbol(key) || !pmt::is_integer(val))
          return;

        val_i = pmt::to_long(val);

        if (pmt::eqv(key, pmt::string_to_symbol("clear")))
          this->clear();
        else if (pmt::eqv(key, pmt::string_to_symbol("decim")))
          this->set_decim(val_i);
        else if (pmt::eqv(key, pmt::string_to_symbol("offset")))
          this->set_offset(val_i);
        else if (pmt::eqv(key, pmt::string_to_symbol("scale")))
          this->set_scale(val_i);
        else if (pmt::eqv(key, pmt::string_to_symbol("trise")))
          this->set_trise(val_i);
        else if (pmt::eqv(key, pmt::string_to_symbol("tdecay")))
          this->set_tdecay(val_i);
        else if (pmt::eqv(key, pmt::string_to_symbol("alpha")))
          this->set_alpha(val_i);
        else if (pmt::eqv(key, pmt::string_to_symbol("epsilon")))
          this->set_epsilon(val_i);
      }
    }

  } /* namespace ettus */
} /* namespace gr */

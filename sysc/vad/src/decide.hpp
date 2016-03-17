/**********************************************************************
    * decide.hpp                                                 *
    *                                                                 *
    * Author:  Hosein Attarzadeh (shan2@kth.se)                       *
    *          adapted from KisTA: https://github.com/nandohca/kista  *
    *                                                                 *
    * Purpose: The VAD Decision task                                  *
    *                                                                 *
    * Usage:   The VAD example                                        *
    *                                                                 *
    * License: BSD3                                                   *
    *******************************************************************/

#ifndef VADDECISION_HPP
#define VADDECISION_HPP

#include <forsyde.hpp>
#include "includes/vad.h"

using namespace ForSyDe::SDF;

void decide_func(token_t<short>& out,
		 token_t<Pfloat> inp1,
		 token_t<Pfloat> inp2)
{
  // Resize all the vectors to contain 1 element
  out.resize(1);
    
  Pfloat* in_pvad = &inp1[0];
  Pfloat* in_thvad = &inp2[0];
  short out_vvad;
#pragma ForSyDe begin decide_func
    
  out_vvad = vad_decision(
		  *in_pvad,  // filtered signal energy (mantissa+exponent)
		  *in_thvad  // decision threshold (mantissa+exponent)
		  );
    
#pragma ForSyDe end
  out[0] = out_vvad;
}


#endif

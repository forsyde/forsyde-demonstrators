/**********************************************************************
 * engComp.hpp                                           *
 *                                                                 *
 * Author:  Hosein Attarzadeh (shan2@kth.se)                       *
 *          adapted from KisTA: https://github.com/nandohca/kista  *
 *                                                                 *
 * Purpose: The Energy Computation task                            *
 *                                                                 *
 * Usage:   The VAD example                                        *
 *                                                                 *
 * License: BSD3                                                   *
 *******************************************************************/

#ifndef ENERGYCOMPUTATION_HPP
#define ENERGYCOMPUTATION_HPP

#include <forsyde.hpp>
#include "includes/vad.h"

using namespace ForSyDe;
using namespace ForSyDe::SDF;

void engComp_func(tokens<token_tuple<pvad_acf0_t,Pfloat>>& out,
		  tokens<rvad_t> inp1,
		  tokens<r_t> inp2,
		  tokens<short> inp3)
{
  // Resize all the vectors to contain 1 element
  out = init<pvad_acf0_t,Pfloat>(1, {1, 1});
    
  short* in_rvad_buff = &get<0,0>(inp1);
  short in_rvad_scal  =  get<0,1>(inp1);
  short* in_r_h       = &get<0>(inp2);
  short in_scal_acf   =  get<0>(inp3);
    
  Pfloat* val_pvad = &get<0,0,0,0>(out);
  Pfloat* val_acf0 = &get<0,0,0,1>(out);  

#pragma ForSyDe begin acfAvg_func
    
  energy_computation(
		     in_r_h,         // autocorrelation of input  signal frame (msb)
		     in_scal_acf,    // scaling factor for the autocorrelations       
		     in_rvad_buff,   // autocorrelated adaptive filter coefficients
		     in_rvad_scal,   // scaling factor for rvad[]
		     val_acf0,       // signal frame energy (mantissa+exponent)
		     val_pvad        // filtered signal energy (mantissa+exponent)
		     );
    
#pragma ForSyDe end
  get<0,1,0>(out) = *val_pvad;
}


#endif

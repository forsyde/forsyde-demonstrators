/**********************************************************************
 * thrAdp.hpp                                         *
 *                                                                 *
 * Author:  Hosein Attarzadeh (shan2@kth.se)                       *
 *          adapted from KisTA: https://github.com/nandohca/kista  *
 *                                                                 *
 * Purpose: The Threshold Adaptation.hpp task                      *
 *                                                                 *
 * Usage:   The VAD example                                        *
 *                                                                 *
 * License: BSD3                                                   *
 *******************************************************************/

#ifndef THRESHOLDADAPTATION_HPP
#define THRESHOLDADAPTATION_HPP

#include <forsyde.hpp>
#include "includes/vad.h"

using namespace ForSyDe;
using namespace ForSyDe::SDF;

void thrAdp_func(tokens<token_tuple<rvad_t,Pfloat>>& out,
		 tokens<token_tuple<rav1_t,short,pvad_acf0_t,short,short>> inp1)
{
  // Resize all the output vectors to contain 1 element
  out = init<rvad_t,Pfloat>(1, {1, 1});
    
  short* in_rav_buff  = &get<0,0,0,0>(inp1);
  short in_rav_scal   =  get<0,0,0,1>(inp1);
  short in_stat       =  get<0,1,0  >(inp1);
  Pfloat in_pvad_pvad =  get<0,2,0,0>(inp1);
  Pfloat in_pvad_acf0 =  get<0,2,0,1>(inp1);
  short in_tone       =  get<0,3,0  >(inp1);
  short in_ptch       =  get<0,4,0  >(inp1);
    
  short* out_rvad_buff = &get<0,0,0,0>(out);
  short* out_rvad_scal = &get<0,0,0,1>(out);
  Pfloat* out_thvad    = &get<0,1    >(out);
    
#pragma ForSyDe begin thrAdp_func

  threshold_adaptation(
		       in_stat,        // flag to indicate spectral stationarity
		       in_ptch,        // flag to indicate a periodic signal component
		       in_tone,        // flag to indicate a tone signal component
		       in_rav_buff,    // ACF obtained from L_av1
		       in_rav_scal,    // rav1[] scaling factor
		       in_pvad_pvad,   // filtered signal energy (mantissa+exponent)
		       in_pvad_acf0,   // signal frame energy (mantissa+exponent)
		       out_rvad_buff,  // autocorrelated adaptive filter coefficients
		       out_rvad_scal,  // rvad[] scaling factor
		       out_thvad       // decision threshold (mantissa+exponent)
		       );
    
#pragma ForSyDe end
}


#endif

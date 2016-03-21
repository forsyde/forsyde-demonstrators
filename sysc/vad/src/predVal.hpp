/**********************************************************************
 * predVal.hpp                                             *
 *                                                                 *
 * Author:  Hosein Attarzadeh (shan2@kth.se)                       *
 *          adapted from KisTA: https://github.com/nandohca/kista  *
 *                                                                 *
 * Purpose: The Predictor Values task                              *
 *                                                                 *
 * Usage:   The VAD example                                        *
 *                                                                 *
 * License: BSD3                                                   *
 *******************************************************************/

#ifndef PREDICTORVALUES_HPP
#define PREDICTORVALUES_HPP

#include <forsyde.hpp>
#include <src/includes/vad.h>

using namespace ForSyDe::SDF;

void predVal_func(tokens<rav1_t>& out,
		  tokens<L_av_t> inp1)
{
  // Resize all the vectors to contain 1 element
  out = init<rav1_t>(1);

  short* out_rav_buff = std::get<0>(out[0]).data();
  short* out_rav_scal = &std::get<1>(out[0]);
#pragma ForSyDe begin predVal_func
    
  int in_L_av[9];
  for (auto i=0;i<9;i++) in_L_av[i] = inp1[0][i];
    
  predictor_values(
		   in_L_av,        // ACF averaged over previous four frames
		   out_rav_buff,   // ACF obtained from in_buff
		   out_rav_scal    // out scaling factor
		   );
    
#pragma ForSyDe end
}


#endif

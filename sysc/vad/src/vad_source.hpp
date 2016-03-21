/**********************************************************************
 * vad_source.hpp                                                  *
 *                                                                 *
 * Author:  Hosein Attarzadeh (shan2@kth.se)                       *
 *          adapted from KisTA: https://github.com/nandohca/kista  *
 *                                                                 *
 * Purpose: Provide input stimuli                                  *
 *                                                                 *
 * Usage:   The VAD example                                        *
 *                                                                 *
 * License: BSD3                                                   *
 *******************************************************************/

#ifndef VADSOURCE_HPP
#define VADSOURCE_HPP

#include <forsyde.hpp>
#include <iostream>

using namespace ForSyDe::SDF;

void vadSource_func(token_tuple<r_t,r_t,r_t,short,short,rc_t,short>& out,
		    const std::string& line)
{
  // Resize all the vectors to contain 1 element
  out = token_tuple<r_t,r_t,r_t,short,short,rc_t,short>({1,1,1,1,1,1,1});

#pragma ForSyDe begin vadSource_func

  std::stringstream ss(line);
    
  // read r_h_1
  for (short& val: get<0,0>(out))
    ss >> val;
  ss.ignore(1);   //ignore comma
    
  // read r_h_2
  get<1,0>(out) = get<0,0>(out);
  //~ for (short& val: (std::get<1>(out))[0])
  //~ ss >> val;
  //~ ss.ignore(1);   //ignore comma
    
  // read r_l
  for (short& val: get<2,0>(out))
    ss >> val;
  ss.ignore(1);   //ignore comma
    
  // read scal_acf_1
  ss >> get<3,0>(out);
  ss.ignore(1);   //ignore comma
    
  // read scal_acf_1
  get<4,0>(out) = get<3,0>(out);
  //~ ss >> (std::get<4>(out))[0];
  //~ ss.ignore(1);   //ignore comma
    
  // read rc
  for (short& val: get<5,0>(out))
    ss >> val;
  ss.ignore(1);   //ignore comma
    
  // read pitch
  ss >> get<6,0>(out);
    
#pragma ForSyDe end
}


#endif

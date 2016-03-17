/**********************************************************************
    * vad_types.hpp -- type definitions                               *
    *                                                                 *
    * Author:  Hosein Attarzadeh (shan2@kth.se)                       *
    *          adapted from KisTA: https://github.com/nandohca/kista  *
    *                                                                 *
    * Purpose: Definition of types used in the model                  *
    *                                                                 *
    * Usage:   The VAD example                                        *
    *                                                                 *
    * License: BSD3                                                   *
    *******************************************************************/

#ifndef VADTYPES_HPP
#define VADTYPES_HPP

#include <forsyde.hpp>
#include "src/includes/vad.h"
#include <array>
#include <cmath>

typedef std::array<short,9> r_t;
typedef std::array<short,9> L_av_t;
typedef std::array<short,4> rc_t;
typedef std::tuple<std::array<short,9> , short> rav1_t;
typedef std::tuple<std::array<short,9> , short> rvad_t;
typedef std::tuple<Pfloat,Pfloat> pvad_acf0_t;

DEFINE_TYPE(r_t);
DEFINE_TYPE(rc_t);
DEFINE_TYPE(rav1_t);
DEFINE_TYPE(pvad_acf0_t);
DEFINE_TYPE_STREAM(Pfloat) {os << obj.m * pow(10,obj.e);}

DEFINE_TYPE(token_tuple_t<L_av_t,L_av_t>);
DEFINE_TYPE(token_tuple_t<rav1_t,short,pvad_acf0_t,short,short>);
DEFINE_TYPE(token_tuple_t<pvad_acf0_t,Pfloat>);
DEFINE_TYPE(token_tuple_t<rvad_t,Pfloat>);
DEFINE_TYPE(token_tuple_t<r_t,r_t,r_t,short,short,rc_t,short>);

#endif

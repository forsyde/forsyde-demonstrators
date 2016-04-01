
#ifndef Merge_HPP
#define Merge_HPP

#include "include/MP3Decoder.h"
#include "include/MP3Decoder_types.hpp"

using namespace ForSyDe;
using namespace ForSyDe::SDF;

void Merge_func(const MergeType& inp)
{

  ChanuleSamples* inp1 = &get<0,0>(inp);  
  ChanuleSamples* inp2 = &get<1,0>(inp);  
  char*           inp3 = &get<2,0>(inp);  
  FrameHeader*    inp4 = &get<3,0>(inp);  
  ChanuleSamples* inp5 = &get<4,0>(inp);  
  ChanuleSamples* inp6 = &get<5,0>(inp);  


#pragma ForSyDe begin Merge_func

        /* Main actor code */
        mergeChanules(&inp4[0], &inp1[0], &inp2[0], &inp5[0], &inp6[0], "output");

#pragma ForSyDe end
        if (inp3[0]) sc_stop();
}

#endif

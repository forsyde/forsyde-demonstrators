
#ifndef ProcessChanuleOneLeft_HPP
#define ProcessChanuleOneLeft_HPP

#include "include/MP3Decoder.h"
#include "include/MP3Decoder_types.hpp"

using namespace ForSyDe;
using namespace ForSyDe::SDF;

void ProcessChanuleOneLeft_func(
    tokens<ChanuleType>&            outs, // headerChanuleLeft
    const tokens<FrameHeader>&        inp1, // headerGranule
    const tokens<FrameSideInfo>&      inp2, // sideInfoGranule
    const tokens<ChanuleData>&        inp3, // chanuleData
    const tokens<VecType>&            inp4  // sync
)
{ 
  outs = init<ChanuleSamples,VecType>(1,{1,1});
  ChanuleSamples* out1 = &get<0,0,0>(outs);  
  VecType*        out2 = &get<0,1,0>(outs);  

#pragma ForSyDe begin ProcessChanuleOneLeft_func

    /* User-defined local variables */
    GranuleData processedMainData;
    
    /* Main actor code */
    out2[0] = inp4[0];
    processChanule(1, 0, &out1[0], &inp1[0], &inp2[0], &inp3[0], out2[0].v_vec);
            
#pragma ForSyDe end    
}

#endif

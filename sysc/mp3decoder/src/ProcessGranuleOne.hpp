
#ifndef ProcessGranuleOne_HPP
#define ProcessGranuleOne_HPP

#include "include/MP3Decoder.h"
#include "include/MP3Decoder_types.hpp"

using namespace ForSyDe;
using namespace ForSyDe::SDF;

void ProcessGranuleOne_func(
    tokens<GranuleType>&              outs,
    const tokens<FrameHeader>&        inp1, // headerGranule
    const tokens<FrameSideInfo>&      inp2, // sideInfoGranule
    const tokens<GranuleData>&        inp3  // granuleData
)
{
  outs = init<FrameHeader, FrameSideInfo, ChanuleData, FrameHeader, FrameSideInfo, ChanuleData> 
    (1, {1,1,1,1,1,1});

  FrameHeader*   out1 = &get<0,0,0>(outs);    // headerChanuleLeft
  FrameSideInfo* out2 = &get<0,1,0>(outs);    // sideInfoChanuleLeft
  ChanuleData*   out3 = &get<0,2,0>(outs);    // chanuleDataLeft
  FrameHeader*   out4 = &get<0,3,0>(outs);    // headerChanuleRight
  FrameSideInfo* out5 = &get<0,4,0>(outs);    // sideInfoChanuleRight
  ChanuleData*   out6 = &get<0,5,0>(outs);    // chanuleDataRight
    
#pragma ForSyDe begin ProcessGranuleOne_func

    /* User-defined local variables */
    GranuleData processedMainData;
    
    /* Main actor code */
    processGranule(1, &inp1[0], &inp2[0], &inp3[0], &processedMainData);

    out1[0] = inp1[0];
    out2[0] = inp2[0];
    copyChanuleData(processedMainData, 0, out3[0]);

    out4[0] = inp1[0];
    out5[0] = inp2[0];
    copyChanuleData(processedMainData, 1, out6[0]);
            
#pragma ForSyDe end
}

#endif

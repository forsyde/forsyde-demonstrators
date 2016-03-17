
#ifndef ProcessGranuleZero_HPP
#define ProcessGranuleZero_HPP

#include "include/MP3Decoder.h"
#include "include/MP3Decoder_types.hpp"

using namespace std;

void ProcessGranuleZero_func(
    token_t<GranuleType>&              outs,
    const token_t<FrameHeader>&        inp1, // headerGranule
    const token_t<FrameSideInfo>&      inp2, // sideInfoGranule
    const token_t<GranuleData>&        inp3  // granuleData
)
{
    token_t<FrameHeader>         out1(1);    // headerChanuleLeft
    token_t<FrameSideInfo>       out2(1);    // sideInfoChanuleLeft
    token_t<ChanuleData>         out3(1);    // chanuleDataLeft
    token_t<FrameHeader>         out4(1);    // headerChanuleRight
    token_t<FrameSideInfo>       out5(1);    // sideInfoChanuleRight
    token_t<ChanuleData>         out6(1);    // chanuleDataRight

#pragma ForSyDe begin ProcessGranuleZero_func

    /* User-defined local variables */
    GranuleData processedMainData;

    /* Main actor code */
    processGranule(0, &inp1[0], &inp2[0], &inp3[0], &processedMainData);

    out1[0] = inp1[0];
    out2[0] = inp2[0];
    copyChanuleData(processedMainData, 0, out3[0]);

    out4[0] = inp1[0];
    out5[0] = inp2[0];
    copyChanuleData(processedMainData, 1, out6[0]);

#pragma ForSyDe end
    outs[0] = make_tuple(out1,out2,out3,out4,out5,out6);
}

#endif

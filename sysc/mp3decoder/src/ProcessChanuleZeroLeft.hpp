
#ifndef ProcessChanuleZeroLeft_HPP
#define ProcessChanuleZeroLeft_HPP

#include "include/MP3Decoder.h"
#include "include/MP3Decoder_types.hpp"

using namespace std;

void ProcessChanuleZeroLeft_func(
    token_t<ChanuleType>&              outs, // headerChanuleLeft
    const token_t<FrameHeader>&        inp1, // headerGranule
    const token_t<FrameSideInfo>&      inp2, // sideInfoGranule
    const token_t<ChanuleData>&        inp3, // chanuleData
    const token_t<VecType>&            inp4  // sync
)
{
    token_t<ChanuleSamples>         out1(1);
    token_t<VecType>                out2(1);
#pragma ForSyDe begin ProcessChanuleZeroLeft_func

    /* User-defined local variables */
    GranuleData processedMainData;

    /* Main actor code */
    out2[0] = inp4[0];
    processChanule(0, 0, &out1[0], &inp1[0], &inp2[0], &inp3[0], out2[0].v_vec);


#pragma ForSyDe end
    outs[0] = make_tuple(out1,out2);
}

#endif


#ifndef ReadBitstreamAndExtractFrames_HPP
#define ReadBitstreamAndExtractFrames_HPP

#include "include/MP3Decoder.h"
#include "include/MP3Decoder_types.hpp"

using namespace std;

void ReadBitstreamAndExtractFrames_func(
    token_t<InputType>& outs,
    const token_t<float>& inp1)
{
    token_t<float>               out1(1);    // dummyCounter
    token_t<bool>                out2(1);    // lastFrame
    token_t<FrameHeader>         out3(1);    // headerMerge
    token_t<FrameHeader>         out4(1);    // headerGranule0
    token_t<FrameSideInfo>       out5(1);    // sideInfoGranule0
    token_t<GranuleData>         out6(1);    // granuleData0
    token_t<FrameHeader>         out7(1);    // headerGranule1
    token_t<FrameSideInfo>       out8(1);    // sideInfoGranule1
    token_t<GranuleData>         out9(1);    // granuleData1

#pragma ForSyDe begin ReadBitstreamAndExtractFrames_func
    /* User-defined local variables */
    char* file_name = "files/test.mp3";
    static FrameHeader header;
    static FrameSideInfo sideInfo = {0};
    static FrameMainData frameMainData;
    bool moreFrames = true;

    /* Main actor code */

    #ifdef FORSYDE_HPP
    moreFrames = readBitstreamAndExtractFrames(file_name, &header, &sideInfo, &frameMainData);
    #endif

    out1[0] = 1;
    out2[0] = !moreFrames;
    out3[0] = header;
    out4[0] = header;
    out5[0] = sideInfo;
    copyGranuleData(frameMainData, 0, out6[0]);
    out7[0] = header;
    out8[0] = sideInfo;
    copyGranuleData(frameMainData, 1, out9[0]);

#pragma ForSyDe end
    outs[0] = make_tuple(out1,out2,out3,out4,out5,out6,out7,out8,out9);
}

#endif

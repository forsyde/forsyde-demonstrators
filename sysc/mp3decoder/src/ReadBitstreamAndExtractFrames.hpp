
#ifndef ReadBitstreamAndExtractFrames_HPP
#define ReadBitstreamAndExtractFrames_HPP

#include "include/MP3Decoder.h"
#include "include/MP3Decoder_types.hpp"

using namespace ForSyDe;
using namespace ForSyDe::SDF;

void ReadBitstreamAndExtractFrames_func(
    tokens<InputType>& outs,
    const tokens<float>& inp)
{
  outs = init<float,char,FrameHeader,FrameHeader,FrameSideInfo,GranuleData,FrameHeader,FrameSideInfo,GranuleData>
    (1, {1,1,1,1,1,1,1,1,1});
    
    float*         out1 = &get<0,0,0>(outs);    // dummyCounter
    char*          out2 = &get<0,1,0>(outs);    // lastFrame
    FrameHeader*   out3 = &get<0,2,0>(outs);    // headerMerge
    FrameHeader*   out4 = &get<0,3,0>(outs);    // headerGranule0
    FrameSideInfo* out5 = &get<0,4,0>(outs);    // sideInfoGranule0
    GranuleData*   out6 = &get<0,5,0>(outs);    // granuleData0
    FrameHeader*   out7 = &get<0,6,0>(outs);    // headerGranule1
    FrameSideInfo* out8 = &get<0,7,0>(outs);    // sideInfoGranule1
    GranuleData*   out9 = &get<0,8,0>(outs);    // granuleData1


#pragma ForSyDe begin ReadBitstreamAndExtractFrames_func
    /* User-defined local variables */
    char* file_name = "files/test.mp3";
    static FrameHeader header;
    static FrameSideInfo sideInfo = {0};
    static FrameMainData frameMainData;
    char moreFrames = true;

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
}

#endif

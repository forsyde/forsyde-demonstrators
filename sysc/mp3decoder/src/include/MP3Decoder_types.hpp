/**********************************************************************
    * MP3Decoder_types.hpp -- type definitions                        *
    *                                                                 *
    * Author:  George Ungureanu                                       *
    *                                                                 *
    * Purpose: Definition of types used in the model                  *
    *                                                                 *
    * Usage:   The MP3Decoder example                                 *
    *                                                                 *
    * License: BSD3                                                   *
    *******************************************************************/
#ifndef MP3DECODERTYPES_HPP
#define MP3DECODERTYPES_HPP

#include "MP3Decoder.h"
#include <forsyde.hpp>

using namespace ForSyDe::SDF;

typedef token_tuple_t<
        float,              // dummyCounter
        bool,               // lastFrame
        FrameHeader,        // headerMerge
        FrameHeader,        // headerGranule0
        FrameSideInfo,      // sideInfoGranule0
        GranuleData,        // granuleData0
        FrameHeader,        // headerGranule1
        FrameSideInfo,      // sideInfoGranule1
        GranuleData         // granuleData1
    > InputType;

typedef unzipN<
        float,
        bool,
        FrameHeader,
        FrameHeader,
        FrameSideInfo,
        GranuleData,
        FrameHeader,
        FrameSideInfo,
        GranuleData
    > InputUnzipper;


typedef token_tuple_t<
        ChanuleSamples,
        ChanuleSamples,
        bool,
        FrameHeader,
        ChanuleSamples,
        ChanuleSamples
    > MergeType;

typedef zipN<
    ChanuleSamples,
    ChanuleSamples,
    bool,
    FrameHeader,
    ChanuleSamples,
    ChanuleSamples
    > MergeZipper;


typedef token_tuple_t<
        ChanuleSamples,
        VecType
    > ChanuleType;

typedef unzipN<
        ChanuleSamples,
        VecType
    > ChanuleUnzipper;

typedef token_tuple_t<
        FrameHeader,            // headerChanuleLeft
        FrameSideInfo,          // sideInfoChanuleLeft
        ChanuleData,            // chanuleDataLeft
        FrameHeader,            // headerChanuleRight
        FrameSideInfo,          // sideInfoChanuleRight
        ChanuleData             // chanuleDataRight
    > GranuleType;

typedef unzipN<
        FrameHeader,
        FrameSideInfo,
        ChanuleData,
        FrameHeader,
        FrameSideInfo,
        ChanuleData
    > GranuelUnzipper;


typedef comb<InputType,float>                                            ReadBitstreamAndExtractFrames;
typedef comb4<ChanuleType,FrameHeader,FrameSideInfo,ChanuleData,VecType> ProcessChanule;
typedef comb3<GranuleType,FrameHeader,FrameSideInfo,GranuleData>         ProcessGranule;
typedef sink<MergeType> Merge;


DEFINE_TYPE(InputType);
DEFINE_TYPE(MergeType);
DEFINE_TYPE(ChanuleType);
DEFINE_TYPE(GranuleType);


DEFINE_TYPE_STREAM(ChanuleSamples){}
DEFINE_TYPE_STREAM(ChanuleData){}
DEFINE_TYPE_STREAM(GranuleData){}
DEFINE_TYPE_STREAM(FrameSideInfo){}
DEFINE_TYPE_STREAM(FrameHeader){}
DEFINE_TYPE_STREAM(VecType){}

#endif

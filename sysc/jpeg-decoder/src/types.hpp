#ifndef _JPEG_TYPES_H_INCLUDED
#define _JPEG_TYPES_H_INCLUDED

#include "structures.h"
#include <forsyde.hpp>

using namespace ForSyDe::SDF;

typedef unzipN<FValue, int, SubHeader1, SubHeader2> VldUnzipper;
typedef zipN<SubHeader2, ColorBuffer> RasterZipper;

TYPEDEF(VldOutputType,   token_tuple<FValue, int, SubHeader1, SubHeader2>)
TYPEDEF(RasterInputType, token_tuple<SubHeader2, ColorBuffer>)

DEFINE_TYPE_STREAM(PBlock){}
DEFINE_TYPE_STREAM(FBlock){}
DEFINE_TYPE_STREAM(FValue){}
DEFINE_TYPE_STREAM(ColorBuffer){}
DEFINE_TYPE_STREAM(cd_t){}
DEFINE_TYPE_STREAM(SubHeader1){}
DEFINE_TYPE_STREAM(SubHeader2){}

#endif

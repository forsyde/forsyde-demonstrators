#ifndef TYPES_H
#define TYPES_H

#include <forsyde.hpp>


#include "conf.h"
#include "susan.h"

using namespace ForSyDe;
using namespace ForSyDe::SDF;

typedef unzipN<MCU_BLOCK, EdgeStrength, EdgeDirection> SusanDirectionOutUnzipper;

TYPEDEF (SusanUsanOutputType,      token_tuple<MCU_BLOCK,EdgeStrength>);
TYPEDEF (SusanDirectionOutputType, token_tuple<MCU_BLOCK,EdgeStrength,EdgeDirection>) ;
TYPEDEF (SusanThinOutputType,      token_tuple<MCU_BLOCK,EdgeDirection>) ;

DEFINE_TYPE_STREAM(MCU_BLOCK) {}
DEFINE_TYPE_STREAM(EdgeStrength) {}
DEFINE_TYPE_STREAM(EdgeDirection) {}

#endif

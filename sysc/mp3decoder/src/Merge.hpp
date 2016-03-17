
#ifndef Merge_HPP
#define Merge_HPP

#include "include/MP3Decoder.h"
#include "include/MP3Decoder_types.hpp"

using namespace ForSyDe::SDF;
using namespace std;

void Merge_func(const MergeType& inp)
{
        token_t<ChanuleSamples>  inp1 = get<0>(inp);
        token_t<ChanuleSamples>  inp2 = get<1>(inp);
        token_t<bool>            inp3 = get<2>(inp);
        token_t<FrameHeader>     inp4 = get<3>(inp);
        token_t<ChanuleSamples>  inp5 = get<4>(inp);
        token_t<ChanuleSamples>  inp6 = get<5>(inp);

#pragma ForSyDe begin Merge_func

        /* Main actor code */
        mergeChanules(&inp4[0], &inp1[0], &inp2[0], &inp5[0], &inp6[0], "output");

#pragma ForSyDe end
        if (inp3[0]) sc_stop();
}

#endif

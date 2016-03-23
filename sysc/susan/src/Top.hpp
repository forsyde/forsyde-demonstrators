#ifndef TOP_H_
#define TOP_H_

#include <forsyde.hpp>
#include "conf.h"
#include "susan.h"
#include "types.hpp"
#include "imageio.hpp"

using namespace ForSyDe;
using namespace ForSyDe::SDF;


void splitMcuBlocks_func(tokens<MCU_BLOCK> &out, tokens<Image> in) {
  std::cout << "splitImage" << std::endl;
  out = init<MCU_BLOCK>(1);

  MCU_BLOCK*  o = &get<0>(out);
  Image* i = &in[0];

#pragma ForSyDe begin getImage_func
  splitIntoMCUs(o, i);
#pragma ForSyDe end
}

void usan_func(tokens<SusanUsanOutputType> &out, const tokens<MCU_BLOCK> &inp1) {
  std::cout << "usan" << std::endl;
  out = init<MCU_BLOCK, EdgeStrength>(1, {1,1});

  MCU_BLOCK     i  =  get<0>(inp1);
  MCU_BLOCK*    o1 = &get<0,0,0>(out);
  EdgeStrength* o2 = &get<0,1,0>(out);

#pragma ForSyDe begin usan_func
  susanUsan(&i, o1, o2);
#pragma ForSyDe end
}

void direction_func(tokens<SusanDirectionOutputType> &out, const tokens<MCU_BLOCK> &inp1, const tokens<EdgeStrength> &inp2) {
  std::cout << "direction" << std::endl;
  out = init<MCU_BLOCK, EdgeStrength, EdgeDirection>(1, {1,1,1});

  MCU_BLOCK      i1 =  get<0>(inp1);
  EdgeStrength   i2 =  get<0>(inp2);
  MCU_BLOCK*     o1 = &get<0,0,0>(out);
  EdgeStrength*  o2 = &get<0,1,0>(out);
  EdgeDirection* o3 = &get<0,2,0>(out);

#pragma ForSyDe begin direction_func
  susanDirection(&i1, &i2, o1, o2, o3);
#pragma ForSyDe end
}

void thin_func(tokens<SusanThinOutputType> &out, const tokens<MCU_BLOCK> &inp1, const tokens<EdgeStrength> &inp2, const tokens<EdgeDirection> &inp3) {
  std::cout << "thin" << std::endl;
  out = init<MCU_BLOCK, EdgeDirection>(1, {1,1});

  MCU_BLOCK      i1 =  get<0>(inp1);
  EdgeStrength   i2 =  get<0>(inp2);
  EdgeDirection  i3 =  get<0>(inp3);
  MCU_BLOCK*     o1 = &get<0,0,0>(out);
  EdgeDirection* o2 = &get<0,1,0>(out);

#pragma ForSyDe begin thin_func	
  susanThin(&i1, &i2 , &i3, o1, o2);
#pragma ForSyDe end
}

void stitchImage_func(tokens<SusanImageOutputType> &out, 
		   const tokens<MCU_BLOCK> &inp1, const tokens<EdgeDirection> &inp2) {
  std::cout << "stitchImage" << std::endl;
  out = init<uchar, uchar, Image>(1, {1,1,1});

  MCU_BLOCK      i1 =  get<0>(inp1);
  EdgeDirection  i2 =  get<0>(inp2);
  uchar*         o1 = &get<0,0,0>(out);
  uchar*         o2 = &get<0,1,0>(out);
  Image*         o3 = &get<0,2,0>(out);

#pragma ForSyDe begin putImage_func
  wrapUp(&i1, &i2, o1, o2, o3);
#pragma ForSyDe end
}

void report_func(const SusanImageOutputType inp1) {

  uchar first = get<0,0>(inp1);
  uchar last  = get<1,0>(inp1);
  Image image = get<2,0>(inp1);

  if (first) initOutFile(image.xSize, image.ySize);
  if (last) {
    writeToFile(image.xSize, image.ySize, &image.imageBuffer[0]);
    sc_stop();
  }

}

SC_MODULE(Top) {
 public:
  // Signals
  SDF2SDF<Image>                inputImage;
  SDF2SDF<MCU_BLOCK>            in_McuBlock;
  SDF2SDF<SusanUsanOutputType>  susanUsanOut;
  SDF2SDF<MCU_BLOCK>		susanUsanOut_McuBlock;
  SDF2SDF<EdgeStrength>		susanUsanOut_EdgeStrength;
  SDF2SDF<SusanDirectionOutputType> susanDirectionOut;
  SDF2SDF<MCU_BLOCK>            susanDirectionOut_McuBlock;
  SDF2SDF<EdgeStrength>         susanDirectionOut_EdgeStrength;
  SDF2SDF<EdgeDirection>        susanDirectionOut_EdgeDirection;
  SDF2SDF<SusanThinOutputType>  susanThinOut;
  SDF2SDF<MCU_BLOCK>            susanThinOut_McuBlock;
  SDF2SDF<EdgeDirection>	susanThinOut_EdgeDirection;
  SDF2SDF<SusanImageOutputType> outputImage;

  // Triple Unzippers
  SusanDirectionOutUnzipper *susanDirectionOutUnzipper;


  SC_CTOR(Top) {
    SDF::make_constant("ReadImage", getImage(), 0, inputImage);

    SDF::make_comb("splitMcuBlocks", splitMcuBlocks_func, 1, 1, in_McuBlock, inputImage);

    make_comb("usan", usan_func, 1, 1, susanUsanOut, in_McuBlock);

    make_unzip("susanUsanOutUnzipper", susanUsanOut, 1, 1, susanUsanOut_McuBlock, susanUsanOut_EdgeStrength);
   
    make_comb2("direction", direction_func, 1, 1, 1, susanDirectionOut, susanUsanOut_McuBlock, susanUsanOut_EdgeStrength);

    susanDirectionOutUnzipper = new SusanDirectionOutUnzipper("susanDirectionOutUnzipper",{1, 1, 1});
    susanDirectionOutUnzipper->iport1(susanDirectionOut);
    std::get<0>(susanDirectionOutUnzipper->oport)(susanDirectionOut_McuBlock);
    std::get<1>(susanDirectionOutUnzipper->oport)(susanDirectionOut_EdgeStrength);
    std::get<2>(susanDirectionOutUnzipper->oport)(susanDirectionOut_EdgeDirection);

    make_comb3("thin", thin_func, 1, 1, 1, 1, susanThinOut, susanDirectionOut_McuBlock, susanDirectionOut_EdgeStrength, susanDirectionOut_EdgeDirection);

    make_unzip("susanThinOutUnzipper", susanThinOut, 1, 1, susanThinOut_McuBlock, susanThinOut_EdgeDirection);

    make_comb2("stitchImage", stitchImage_func, 1, 1, 1, outputImage, susanThinOut_McuBlock, susanThinOut_EdgeDirection);

    SDF::make_sink("report", report_func, outputImage);
  }

#ifdef FORSYDE_INTROSPECTION

  void start_of_simulation() {
    ForSyDe::XMLExport dumper("ir/");
    dumper.traverse(this);
  }
#endif
};

#endif /* TOP_H_ */

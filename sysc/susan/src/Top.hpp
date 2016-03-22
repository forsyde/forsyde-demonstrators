#ifndef TOP_H_
#define TOP_H_

#include <forsyde.hpp>
#include "conf.h"
#include "susan.h"
#include "types.hpp"

using namespace ForSyDe;
using namespace ForSyDe::SDF;


void getImage_func(tokens<MCU_BLOCK> &out, const tokens<int> &inp1) {
  std::cout << "getImage" << std::endl;
  
  out = init<MCU_BLOCK>(1);
  MCU_BLOCK* o = &get<0>(out);

#pragma ForSyDe begin getImage_func
  splitIntoMCUs(o);
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

void putImage_func(tokens<int> &out, const tokens<MCU_BLOCK> &inp1, const tokens<EdgeDirection> &inp2) {
  std::cout << "putImage" << std::endl;
  out = init<int>(1);

  MCU_BLOCK      i1 =  get<0>(inp1);
  EdgeDirection  i2 =  get<0>(inp2);
  int*           o1 = &get<0>(out);

#pragma ForSyDe begin putImage_func
  wrapUp(&i1, &i2);	
  *o1 = 1; //dummy output
#pragma ForSyDe end
}

void report_func(const int inp1) {
 #pragma ForSyDe begin report_func
  ;
  ;
#pragma ForSyDe end
}

SC_MODULE(Top) {
 public:
  // Signals
  SDF2SDF<int>                  dummyInput;
  SDF2SDF<MCU_BLOCK>            getImageOut_McuBlock;
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
  SDF2SDF<int>                  dummyOutput;

  // Triple Unzippers
  SusanDirectionOutUnzipper *susanDirectionOutUnzipper;

  SC_CTOR(Top) {
    SDF::make_constant("dummyInputConst", 0, 0, dummyInput);

    make_comb("getImage", getImage_func, 1, 1, getImageOut_McuBlock, dummyInput);

    make_comb("usan", usan_func, 1, 1, susanUsanOut, getImageOut_McuBlock);

    make_unzip("susanUsanOutUnzipper", susanUsanOut, 1, 1, susanUsanOut_McuBlock, susanUsanOut_EdgeStrength);
   
    make_comb2("direction", direction_func, 1, 1, 1, susanDirectionOut, susanUsanOut_McuBlock, susanUsanOut_EdgeStrength);

    susanDirectionOutUnzipper = new SusanDirectionOutUnzipper("susanDirectionOutUnzipper",{1, 1, 1});
    susanDirectionOutUnzipper->iport1(susanDirectionOut);
    std::get<0>(susanDirectionOutUnzipper->oport)(susanDirectionOut_McuBlock);
    std::get<1>(susanDirectionOutUnzipper->oport)(susanDirectionOut_EdgeStrength);
    std::get<2>(susanDirectionOutUnzipper->oport)(susanDirectionOut_EdgeDirection);

    make_comb3("thin", thin_func, 1, 1, 1, 1, susanThinOut, susanDirectionOut_McuBlock, susanDirectionOut_EdgeStrength, susanDirectionOut_EdgeDirection);

    make_unzip("susanThinOutUnzipper", susanThinOut, 1, 1, susanThinOut_McuBlock, susanThinOut_EdgeDirection);

    make_comb2("putImage", putImage_func, 1, 1, 1, dummyOutput, susanThinOut_McuBlock, susanThinOut_EdgeDirection);

    SDF::make_sink("report", report_func, dummyOutput);
  }

#ifdef FORSYDE_INTROSPECTION

  void start_of_simulation() {
    ForSyDe::XMLExport dumper("ir/");
    dumper.traverse(this);
  }
#endif
};

#endif /* TOP_H_ */

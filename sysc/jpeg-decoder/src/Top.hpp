/* 
 * File:   top.hpp
 * Author: ekrem
 *
 * Created on March 29, 2013, 5:44 PM
 */

#ifndef TOP_H_
#define TOP_H_

#include "conf.h"
#include "structures.h"
#include "types.hpp"
#include "actors.h"

#include <forsyde.hpp>


using namespace ForSyDe;
using namespace ForSyDe::SDF;


void vld_func(tokens<VldOutputType> &out, const tokens<int> &inp1) {
  out = init <FValue,int,SubHeader1,SubHeader2>(1, {10, 1, 1, 1});
    
  int         i1 =  get<0>(inp1);
  FValue*     o1 = &get <0,0,0>(out);
  int*        o2 = &get <0,1,0>(out);
  SubHeader1* o3 = &get <0,2,0>(out);
  SubHeader2* o4 = &get <0,3,0>(out);

#pragma ForSyDe begin vld_func
  if (i1 == 1) init_header_vld(NULL, NULL, NULL);
  header_vld(o1, o3, o4);
  *o2 = 0;
#pragma ForSyDe end
}

void iqzz_func(tokens<FBlock> &out, const tokens<FValue> &inp1) {
  out = init<FBlock>(1);

  FValue  i1 =  get<0>(inp1);
  FBlock* o1 = &get<0>(out);

#pragma ForSyDe begin iqzz_func
  iqzz(&i1, o1);
#pragma ForSyDe end
}

void idct_func(tokens<PBlock> &out, const tokens<FBlock> &inp1) {
  out = init<PBlock>(1);

  FBlock  i1 =  get<0>(inp1);
  PBlock* o1 = &get<0>(out);
  
#pragma ForSyDe begin idct_func
  idct(&i1, o1);
#pragma ForSyDe end
}

void cc_func(tokens<ColorBuffer> &out, const tokens<SubHeader1> &inp1,
	const tokens<PBlock> &inp2) {

  SubHeader1   i1 =  get<0>(inp1);
  PBlock       i2 =  get<0>(inp2);
  ColorBuffer* o1 = &get<0>(out);

#pragma ForSyDe begin cc_func
  cc(&i1, &i2, o1);
#pragma ForSyDe end
}

void raster_func(const RasterInputType &inp) {
 
  SubHeader2  i1 =  get<0,0>(inp);
  ColorBuffer i2 =  get<1,0>(inp);

#pragma ForSyDe begin raster_func
  raster(&i1, &i2);
#pragma ForSyDe end
}

SC_MODULE(Top){

  // Signals
  SDF2SDF<int> init_vld_state_in;
  SDF2SDF<int> init_vld_state_out;
  SDF2SDF<VldOutputType> vldOut;
  SDF2SDF<FValue> vld2iqzz;
  SDF2SDF<SubHeader1> subHeader1;
  SDF2SDF<SubHeader2> subHeader2;
  SDF2SDF<FBlock> iqzz2idct;
  SDF2SDF<PBlock> idct2cc;
  SDF2SDF<ColorBuffer> cc2raster;
  SDF2SDF<RasterInputType> rasterInput;

  // Unzippers
  VldUnzipper *vldUnzipper;
  RasterZipper *rasterInputZipper;

  SC_CTOR(Top){

    SDF::make_delay("init_vld_state", 1, init_vld_state_out, init_vld_state_in);

    make_comb("vld", vld_func, 1, 1, vldOut, init_vld_state_out);

    vldUnzipper = new VldUnzipper("vldCombOutUnzipper", { 10, 1, 1, 1 }) ;
    vldUnzipper->iport1(vldOut);
    std::get < 0 > (vldUnzipper->oport)(vld2iqzz);
    std::get < 1 > (vldUnzipper->oport)(init_vld_state_in);
    std::get < 2 > (vldUnzipper->oport)(subHeader1);
    std::get < 3 > (vldUnzipper->oport)(subHeader2);

    make_comb("iqzz", iqzz_func, 1, 1, iqzz2idct, vld2iqzz);

    make_comb("idct", idct_func,  1,  1, idct2cc, iqzz2idct);

    make_comb2("cc", cc_func, 1, 1, 10, cc2raster, subHeader1, idct2cc);

    rasterInputZipper = new RasterZipper("rasterInputZipper", { 1, 1 });
    std::get < 0 > (rasterInputZipper->iport)(subHeader2);
    std::get < 1 > (rasterInputZipper->iport)(cc2raster);
    rasterInputZipper->oport1(rasterInput);

    SDF::make_sink("raster", raster_func, rasterInput );
  }

#ifdef FORSYDE_INTROSPECTION

  void
    start_of_simulation()
  {
    ForSyDe::XMLExport dumper("ir/");
    dumper.traverse(this);
  }
#endif
};

#endif /* TOP_H_ */

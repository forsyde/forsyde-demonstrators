/**********************************************************************           
 * top.hpp -- the Top process and testbench for the VAD example    *
 *                                                                 *
 * Author:  Hosein Attarzadeh (shan2@kth.se)                       *
 *          adapted from KisTA: https://github.com/nandohca/kista  *
 *                                                                 *
 * Purpose:                                                        *
 *                                                                 *
 * Usage:   The Voice Activity Detection (VAD) example             *
 *                                                                 *
 * License: BSD3                                                   *
 *******************************************************************/

#include <forsyde.hpp>

#include "includes/vad_types.hpp"
#include "vad_source.hpp"
#include "toneDetect.hpp"
#include "engComp.hpp"
#include "acfAvg.hpp"
#include "predVal.hpp"
#include "specComp.hpp"
#include "thrAdp.hpp"
#include "decide.hpp"
#include "hangover.hpp"
#include "vadSink.hpp"

using namespace ForSyDe;

tokens<int> test;

SC_MODULE(top)
{
  SDF::signal<short> e5, e9, e11, e15, e16, e18, e19;
  SDF::signal<L_av_t> e1, e2;
  SDF::signal<rav1_t> e3, e4;
  SDF::signal<pvad_acf0_t> e6;
  SDF::signal<rvad_t> e7, e7d;
  SDF::signal<Pfloat> e8, e10;
  SDF::signal<r_t> e12, e13, e14;
  SDF::signal<rc_t> e17;
  SDF::signal<token_tuple<L_av_t,L_av_t>> e1_2;
  SDF::signal<token_tuple<rav1_t,short,pvad_acf0_t,short,short>> e4_5_6_9_18;
  SDF::signal<token_tuple<pvad_acf0_t,Pfloat>> e6_8;
  SDF::signal<token_tuple<rvad_t,Pfloat>> e7_10;
  SDF::signal<token_tuple<r_t,r_t,r_t,short,short,rc_t,short>> e12_13_14_15_16_17_18;
    
  SC_CTOR(top)
  {
    SDF::make_file_source("vadSource1", vadSource_func, "files/source_data.txt", e12_13_14_15_16_17_18);
        
    auto vadSource1_unzip = new SDF::unzipN<r_t,r_t,r_t,short,short,rc_t,short>("vadSource1_unzip", {1,1,1,1,1,1,1});
    vadSource1_unzip->iport1(e12_13_14_15_16_17_18);
    std::get<0>(vadSource1_unzip->oport)(e12);
    std::get<1>(vadSource1_unzip->oport)(e13);
    std::get<2>(vadSource1_unzip->oport)(e14);
    std::get<3>(vadSource1_unzip->oport)(e15);
    std::get<4>(vadSource1_unzip->oport)(e16);
    std::get<5>(vadSource1_unzip->oport)(e17);
    std::get<6>(vadSource1_unzip->oport)(e18);        
        
    SDF::make_comb("toneDetect1", toneDetect_func, 1, 1, e9, e17);
        
    SDF::make_comb3("engComp1", engComp_func, 1, 1, 1, 1, e6_8, e7d, e13, e16);
    SDF::make_unzip("engComp1_unzip", e6_8, 1, 1, e6, e8);
        
    SDF::make_comb3("acfAvg1", acfAvg_func, 1, 1, 1, 1, e1_2, e12, e14, e15);
    SDF::make_unzip("acfAvg1_unzip", e1_2, 1, 1, e1, e2);
        
    auto predVal1 = SDF::make_comb("predVal1", predVal_func, 1, 1, e3, e2);
    predVal1->oport1(e4);
        
    SDF::make_comb2("specComp1", specComp_func, 1, 1, 1, e5, e1, e3);    
    //~ SDF::make_comb5("thrAdp1", thrAdp_func, , , , e7_10, e4, e5, e6, e9, e18);

    auto thrAdp1_zip = new SDF::zipN<rav1_t,short,pvad_acf0_t,short,short>("thrAdp1_zip", {1,1,1,1,1});
    thrAdp1_zip->oport1(e4_5_6_9_18);
    std::get<0>(thrAdp1_zip->iport)(e4);
    std::get<1>(thrAdp1_zip->iport)(e5);
    std::get<2>(thrAdp1_zip->iport)(e6);
    std::get<3>(thrAdp1_zip->iport)(e9);
    std::get<4>(thrAdp1_zip->iport)(e18);
	
    SDF::make_comb("thrAdp1", thrAdp_func, 1, 1, e7_10, e4_5_6_9_18);
	
    SDF::make_unzip("thrAdp1_unzip", e7_10, 1, 1, e7, e10);
        
    std::array<short,9> rvad_init = {{0x6000,0,0,0,0,0,0,0,0}}; short scal_init = 7;
    SDF::make_delay("e7_init", std::make_tuple(rvad_init,scal_init), e7d, e7);
        
    SDF::make_comb2("decide1", decide_func, 1, 1, 1, e11, e8, e10);
        
    SDF::make_comb("hangover1", hangover_func, 1, 1, e19, e11);
        
    SDF::make_file_sink("vadSink1", vadSink_func, "files/sink_data.txt", e19);
  }

#ifdef FORSYDE_INTROSPECTION
  void start_of_simulation()
  {
    system("mkdir -p ir");
    ForSyDe::XMLExport dumper("ir/");
    dumper.traverse(this);
#ifdef FORSYDE_TYPE_INTROSPECTION
    TypeContainer::get().printXML("ir/types.xml");
#endif  
  }
#endif
};

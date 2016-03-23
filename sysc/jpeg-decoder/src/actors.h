#ifndef _JPEG_ACTORS_H_INCLUDED
#define _JPEG_ACTORS_H_INCLUDED

#include "structures.h"

//#ifdef __cplusplus
//extern "C" {
//#endif
	void init_header_vld(FValue * mcu_after_vld, SubHeader1 * SH1, SubHeader2 * SH2);
	void header_vld(FValue * mcu_after_vld, SubHeader1 * SH1, SubHeader2 * SH2);
	void iqzz(const FValue * V, FBlock * B);
	void idct(const FBlock * input, PBlock * output);
	void cc(const SubHeader1 * SH1, const PBlock * PB, ColorBuffer * CB);
	void raster(const SubHeader2 * SH2, const ColorBuffer * CB);
//#ifdef __cplusplus
//}
//#endif
#endif							/* _JPEG_ACTORS_H_INCLUDED */

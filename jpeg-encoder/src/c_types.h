/* 
 * File:   c_types.h
 * Author: marcus
 *
 * Created on March 28, 2013, 6:39 PM
 */

#ifndef TYPES_H
#define	TYPES_H

#include <forsyde.hpp>

// C types

typedef unsigned char color;

extern "C" STRUCT( huffman_t,
	  const unsigned char (*haclen)[12];
	  const unsigned short (*hacbit)[12];
	  const unsigned char *hdclen;
	  const unsigned short *hdcbit;
	  const unsigned char *qtable;
	  short dc;
	  ) {} ;

//extern huffman_t huffman_ctx[3];

// the out of the Huffman encoding step
// packed into a 256 bit array, of which not all bits will be used

extern "C" STRUCT( huffman_encoding_out,
	  unsigned int content[8];
	  unsigned int bit_position;
	  unsigned int height;
	  unsigned int width;
	  int block_id;
	  ) {} ;

extern "C" STRUCT( block_out,
	  unsigned int content[48];
	  unsigned int bit_position;
	  ) {} ;

extern "C" STRUCT( smaller_block,
	 unsigned int height;
	 unsigned int width;
	 int block_id;
	 short content[8][8];
	 short is_cr, is_cb;
	 ) {} ;


// bitmap types

extern "C" STRUCT( BITMAP_HEADER,
	  unsigned int HeaderSize;
	  int Width;
	  int Height;
	  unsigned short int Planes;
	  unsigned short int BitCount;
	  unsigned int Compression;
	  unsigned int SizeImage;
	  int PelsPerMeterX;
	  int PelsPerMeterY;
	  unsigned int ClrUsed;
	  unsigned int ClrImportant;
	  unsigned int RedMask;
	  unsigned int GreenMask;
	  unsigned int BlueMask;
	  unsigned int AlphaMask;
	  unsigned int CsType;
	  unsigned int Endpoints[9]; // see http://msdn2.microsoft.com/en-us/library/ms536569.aspx
	  unsigned int GammaRed;
	  unsigned int GammaGreen;
	  unsigned int GammaBlue;
	  ) {} ;

extern "C" STRUCT( RGBA,
	  color Red;
	  color Green;
	  color Blue;
	  color Alpha;
	  ) {} ;

extern "C" STRUCT( BGRA,
	  color Blue;
	  color Green;
	  color Red;
	  color Alpha;
	  ) {} ;

extern "C" STRUCT( BGR,
	  color Blue;
	  color Green;
	  color Red;
	  ) {} ;

extern "C" STRUCT( colorRGB,
	  unsigned char B, G, R;
	  ) {} ;

extern "C" STRUCT( BGR16,
	  unsigned short int Blue :5;
	  unsigned short int Green :5;
	  unsigned short int Red :5;
	  unsigned short int Reserved :1;
	  ) {} ;

extern "C" STRUCT( bitmap_reader_out,
	  unsigned int height;
	  unsigned int width;
	  int block_id;
	  BGR color_block[16][16];
	  ) {} ;

extern "C" STRUCT( encoded_block,
	  unsigned int height;
	  unsigned int width;
	  int block_id;
	  block_out current_block_data;
	  ) {} ;

#endif	/* TYPES_H */


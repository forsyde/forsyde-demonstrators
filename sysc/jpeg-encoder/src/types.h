/* 
 * File:   types.h
 * Author: marcus
 *
 * Created on March 28, 2013, 6:39 PM
 */

#ifndef TYPES_H
#define	TYPES_H

#include <forsyde.hpp>

// C types

typedef unsigned char color;

C_STRUCT( huffman_t,
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

C_STRUCT( huffman_encoding_out,
	  unsigned int content[8];
	  unsigned int bit_position;
	  unsigned int height;
	  unsigned int width;
	  int block_id;
	  ) {} ;

C_STRUCT( block_out,
	  unsigned int content[48];
	  unsigned int bit_position;
	  ) {} ;

C_STRUCT( smaller_block,
	 unsigned int height;
	 unsigned int width;
	 int block_id;
	 short content[8][8];
	 short is_cr, is_cb;
	 ) {} ;


// bitmap types

C_STRUCT( BITMAP_HEADER,
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

C_STRUCT( RGBA,
	  color Red;
	  color Green;
	  color Blue;
	  color Alpha;
	  ) {} ;

C_STRUCT( BGRA,
	  color Blue;
	  color Green;
	  color Red;
	  color Alpha;
	  ) {} ;

C_STRUCT( BGR,
	  color Blue;
	  color Green;
	  color Red;
	  ) {} ;

C_STRUCT( colorRGB,
	  unsigned char B, G, R;
	  ) {} ;

C_STRUCT( BGR16,
	  unsigned short int Blue :5;
	  unsigned short int Green :5;
	  unsigned short int Red :5;
	  unsigned short int Reserved :1;
	  ) {} ;

C_STRUCT( bitmap_reader_out,
	  unsigned int height;
	  unsigned int width;
	  int block_id;
	  BGR color_block[16][16];
	  ) {} ;

C_STRUCT( encoded_block,
	  unsigned int height;
	  unsigned int width;
	  int block_id;
	  block_out current_block_data;
	  ) {} ;



// ForSyDe-SystemC types

typedef ForSyDe::SDF::zipN <encoded_block, encoded_block, encoded_block, encoded_block> merge_blocks_zipper;
typedef ForSyDe::SDF::unzipN <bitmap_reader_out, bitmap_reader_out, bitmap_reader_out, bitmap_reader_out> rgb_block_unzipper;
typedef ForSyDe::SDF::unzipN <smaller_block, smaller_block, smaller_block, smaller_block, smaller_block, smaller_block> block_unzipper;
typedef ForSyDe::SDF::zipN <huffman_encoding_out, huffman_encoding_out, huffman_encoding_out, huffman_encoding_out, huffman_encoding_out, huffman_encoding_out> encoded_blocks_zipper;

typedef token_tuple_t <encoded_block,encoded_block,encoded_block,encoded_block> merge_blocks_in_t;
typedef token_tuple_t <bitmap_reader_out,bitmap_reader_out,bitmap_reader_out,bitmap_reader_out> rgb_collector_out_t;
typedef token_tuple_t <smaller_block,smaller_block,smaller_block,smaller_block,smaller_block,smaller_block> color_conversion_out_t;
typedef token_tuple_t <huffman_encoding_out,huffman_encoding_out,huffman_encoding_out,huffman_encoding_out, huffman_encoding_out,huffman_encoding_out> concatenate_smaller_blocks_in_t;

DEFINE_TYPE(merge_blocks_in_t)
DEFINE_TYPE(rgb_collector_out_t)
DEFINE_TYPE(color_conversion_out_t)
DEFINE_TYPE(concatenate_smaller_blocks_in_t)

#endif	/* TYPES_H */


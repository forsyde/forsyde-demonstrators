/*
 * block_processor.hpp

 *
 *  Created on: May 11, 2013
 *      Author: marcus
 */

// turn off the reorder warning when using zipN and unzipN
// TODO: fix the warning instead of ignoring it
#pragma GCC diagnostic ignored "-Wreorder"

#ifndef BLOCK_PROCESSOR_HPP_
#define BLOCK_PROCESSOR_HPP_

#include "c_types.h"
#include "tables.h"
#include "forsyde_types.hpp"
#include "process_block.hpp"

#include <forsyde.hpp>

using namespace ForSyDe;
using namespace ForSyDe::SDF;

void color_conversion_func(tokens<color_conversion_out_t> &out, const tokens<bitmap_reader_out> &inp1)
{
  out = init<smaller_block,smaller_block,smaller_block,smaller_block,smaller_block,smaller_block>(1, {1, 1, 1, 1, 1, 1});
  auto current_color_block = get<bitmap_reader_out, 0>(inp1)->color_block;
  auto current_width       = get<bitmap_reader_out, 0>(inp1)->width;
  auto current_height      = get<bitmap_reader_out, 0>(inp1)->height;
  auto current_block_id    = get<bitmap_reader_out, 0>(inp1)->block_id;

  std::cout << "Processing block " << current_block_id << std::endl;

#pragma ForSyDe begin color_conversion_func

  // four blocks of 8x8 Y
  smaller_block y_blocks[2][2];

  // Cb8x8[8][8] block
  smaller_block cb_block;
  cb_block.is_cb = 1;
  cb_block.is_cr = 0;
  cb_block.width = current_width;
  cb_block.height = current_height;
  cb_block.block_id = current_block_id;

  // Cr8x8[8][8] block
  smaller_block cr_block;
  cr_block.is_cb = 0;
  cr_block.is_cr = 1;
  cr_block.width = current_width;
  cr_block.height = current_height;
  cr_block.block_id = current_block_id;

  unsigned int i, j, r, c;

  // getting four 8x8 Y-blocks
  for (i = 0; i < 2; i++)
    {
      for (j = 0; j < 2; j++)
	{
	  for (r = 0; r < 8; r++)
	    {
	      for (c = 0; c < 8; c++)
		{
		  const unsigned rr = (i << 3) + r;
		  const unsigned cc = (j << 3) + c;

		  const color R = current_color_block[rr][cc].Red;
		  const color G = current_color_block[rr][cc].Green;
		  const color B = current_color_block[rr][cc].Blue;

		  // converting RGB into Y (luminance)
		  y_blocks[i][j].content[r][c] = RGB2Y(R, G, B) - 128;

		  y_blocks[i][j].is_cb = 0;
		  y_blocks[i][j].is_cr = 0;
		  y_blocks[i][j].width = current_width;
		  y_blocks[i][j].height = current_height;
		  y_blocks[i][j].block_id = current_block_id;
		}
	    }
	}
    }

  // one block of 8x8 Cr
  short Cb8x8[8][8];
  // one block of 8x8 Cb
  short Cr8x8[8][8];

  // create subsampled Cb and Cr blocks
  subsample(current_color_block, Cb8x8, Cr8x8);

  for (r = 0; r < 8; r++)
    {
      for (c = 0; c < 8; c++)
	{
	  cb_block.content[r][c] = Cb8x8[r][c];
	}
    }
  for (r = 0; r < 8; r++)
    {
      for (c = 0; c < 8; c++)
	{
	  cr_block.content[r][c] = Cr8x8[r][c];
	}
    }
#pragma ForSyDe end

  *get<smaller_block, 0,0,0>(out) = y_blocks[0][0];
  *get<smaller_block, 0,1,0>(out) = y_blocks[0][1];
  *get<smaller_block, 0,2,0>(out) = y_blocks[1][0];
  *get<smaller_block, 0,3,0>(out) = y_blocks[1][1];
  *get<smaller_block, 0,4,0>(out) = cb_block;
  *get<smaller_block, 0,5,0>(out) = cr_block;
}

void dct_func(tokens<smaller_block> &out, const tokens<smaller_block> &inp1)
{
  smaller_block out_block;
  // pass through all picture information to avoid having to create an extra channel for it
  out_block.is_cb = inp1[0].is_cb;
  out_block.is_cr = inp1[0].is_cr;
  out_block.block_id = inp1[0].block_id;
  out_block.width = inp1[0].width;
  out_block.height = inp1[0].height;

#pragma ForSyDe begin dct_func
  dct3(inp1[0].content, out_block.content);
#pragma ForSyDe end

  out[0] = out_block;
}

void huffman_encode_func(tokens<huffman_encoding_out> &out, const tokens<smaller_block> &inp1)
{
  huffman_encoding_out huffman_step_out;

  huffman_step_out.width = inp1[0].width;
  huffman_step_out.height = inp1[0].height;
  huffman_step_out.block_id = inp1[0].block_id;

#pragma ForSyDe begin huffman_encode_func

  if (inp1[0].is_cb == 1)
    {
      huffman_encode(HUFFMAN_CTX_Cb, (short*) inp1[0].content, &huffman_step_out);
      //printf("Cb: Got %u bits form Huffman step\n", huffman_step_out.bit_position);
    }
  else if (inp1[0].is_cr == 1)
    {
      huffman_encode(HUFFMAN_CTX_Cr, (short*) inp1[0].content, &huffman_step_out);
      //printf("Cr: Got %u bits form Huffman step\n", huffman_step_out.bit_position);
    }
  else if ((inp1[0].is_cr == 0) && (inp1[0].is_cb == 0))
    {
      huffman_encode(HUFFMAN_CTX_Y, (short*) inp1[0].content, &huffman_step_out);
      //printf("Y: Got %u bits form Huffman step\n", huffman_step_out.bit_position);
    }
#pragma ForSyDe end

  out[0] = huffman_step_out;
}

void concatenate_steps_func(tokens<encoded_block> &out, const tokens<concatenate_smaller_blocks_in_t> &inp1)
{
  out = tokens<encoded_block>(1);
  huffman_encoding_out in_tup0 = *get<huffman_encoding_out, 0,0,0>(inp1);
  huffman_encoding_out in_tup1 = *get<huffman_encoding_out, 0,1,0>(inp1);
  huffman_encoding_out in_tup2 = *get<huffman_encoding_out, 0,2,0>(inp1);
  huffman_encoding_out in_tup3 = *get<huffman_encoding_out, 0,3,0>(inp1);
  huffman_encoding_out in_tup4 = *get<huffman_encoding_out, 0,4,0>(inp1);
  huffman_encoding_out in_tup5 = *get<huffman_encoding_out, 0,5,0>(inp1);

#pragma ForSyDe begin concatenate_steps_func

  encoded_block out_block;
  block_out* current_block_out = &(out_block.current_block_data);
  init_block_out(current_block_out);

  // collect all six huffman steps and concatenate them to a complete block out
  // Y[0][0]
  write_block_out(current_block_out, &in_tup0);
  // Y[0][1]
  write_block_out(current_block_out, &in_tup1);
  // Y[1][0]
  write_block_out(current_block_out, &in_tup2);
  // Y[1][1]
  write_block_out(current_block_out, &in_tup3);
  // Cb
  write_block_out(current_block_out, &in_tup4);
  // Cr
  write_block_out(current_block_out, &in_tup5);

  // just get the information through to the JPEG writer
  out_block.block_id = in_tup5.block_id;
  out_block.height = in_tup5.height;
  out_block.width = in_tup5.width;
  
#pragma ForSyDe end

  out[0] = out_block;
  //std::cout << get<0>(out).block_id << ", " << get<0>(out).height << ", " << get<0>(out).width << std::endl;

}

SC_MODULE(block_processor)
{
 public:

  SDF_in<bitmap_reader_out> incoming_bitmap_block;
  SDF_out<encoded_block> outgoing_encoded_block;

  SDF2SDF<color_conversion_out_t> color_conversion_out;

  SDF2SDF<concatenate_smaller_blocks_in_t> concatenated_block;

  SDF2SDF<smaller_block> cc_to_dct_1;
  SDF2SDF<smaller_block> cc_to_dct_2;
  SDF2SDF<smaller_block> cc_to_dct_3;
  SDF2SDF<smaller_block> cc_to_dct_4;
  SDF2SDF<smaller_block> cc_to_dct_5;
  SDF2SDF<smaller_block> cc_to_dct_6;

  SDF2SDF<smaller_block> dct_to_huffman_1;
  SDF2SDF<smaller_block> dct_to_huffman_2;
  SDF2SDF<smaller_block> dct_to_huffman_3;
  SDF2SDF<smaller_block> dct_to_huffman_4;
  SDF2SDF<smaller_block> dct_to_huffman_5;
  SDF2SDF<smaller_block> dct_to_huffman_6;

  SDF2SDF<huffman_encoding_out> huffman_to_conc_1;
  SDF2SDF<huffman_encoding_out> huffman_to_conc_2;
  SDF2SDF<huffman_encoding_out> huffman_to_conc_3;
  SDF2SDF<huffman_encoding_out> huffman_to_conc_4;
  SDF2SDF<huffman_encoding_out> huffman_to_conc_5;
  SDF2SDF<huffman_encoding_out> huffman_to_conc_6;

  SC_CTOR(block_processor)
  {
    make_comb("color_conversion", color_conversion_func, 1, 1, color_conversion_out, incoming_bitmap_block);

    auto unzip_blocks = new block_unzipper("block_unzipper", { 1, 1, 1, 1, 1, 1 });
    unzip_blocks->iport1(color_conversion_out);
    std::get < 0 > (unzip_blocks->oport)(cc_to_dct_1);
    std::get < 1 > (unzip_blocks->oport)(cc_to_dct_2);
    std::get < 2 > (unzip_blocks->oport)(cc_to_dct_3);
    std::get < 3 > (unzip_blocks->oport)(cc_to_dct_4);
    std::get < 4 > (unzip_blocks->oport)(cc_to_dct_5);
    std::get < 5 > (unzip_blocks->oport)(cc_to_dct_6);

    make_comb("dct0", dct_func, 1, 1, dct_to_huffman_1, cc_to_dct_1);
    make_comb("dct1", dct_func, 1, 1, dct_to_huffman_2, cc_to_dct_2);
    make_comb("dct00", dct_func, 1, 1, dct_to_huffman_3, cc_to_dct_3);
    make_comb("dct01", dct_func, 1, 1, dct_to_huffman_4, cc_to_dct_4);
    make_comb("dct10", dct_func, 1, 1, dct_to_huffman_5, cc_to_dct_5);
    make_comb("dct11", dct_func, 1, 1, dct_to_huffman_6, cc_to_dct_6);

    make_comb("huffman_encode0",  huffman_encode_func, 1, 1, huffman_to_conc_1, dct_to_huffman_1);
    make_comb("huffman_encode1",  huffman_encode_func, 1, 1, huffman_to_conc_2, dct_to_huffman_2);
    make_comb("huffman_encode00", huffman_encode_func, 1, 1, huffman_to_conc_3, dct_to_huffman_3);
    make_comb("huffman_encode01", huffman_encode_func, 1, 1, huffman_to_conc_4, dct_to_huffman_4);
    make_comb("huffman_encode10", huffman_encode_func, 1, 1, huffman_to_conc_5, dct_to_huffman_5);
    make_comb("huffman_encode11", huffman_encode_func, 1, 1, huffman_to_conc_6, dct_to_huffman_6);

    auto zip_blocks = new encoded_blocks_zipper("block_zipper", { 1, 1, 1, 1, 1, 1 });
    std::get < 0 > (zip_blocks->iport)(huffman_to_conc_1);
    std::get < 1 > (zip_blocks->iport)(huffman_to_conc_2);
    std::get < 2 > (zip_blocks->iport)(huffman_to_conc_3);
    std::get < 3 > (zip_blocks->iport)(huffman_to_conc_4);
    std::get < 4 > (zip_blocks->iport)(huffman_to_conc_5);
    std::get < 5 > (zip_blocks->iport)(huffman_to_conc_6);
    zip_blocks->oport1(concatenated_block);

    make_comb("concatenate_steps", concatenate_steps_func, 1, 1, outgoing_encoded_block, concatenated_block);
  }

};
#endif /* BLOCK_PROCESSOR_HPP_ */

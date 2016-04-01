/* 
 * File:   process_block.hpp
 * Author: marcus
 *
 * Created on March 28, 2013, 5:50 PM
 */

#ifndef PROCESS_BLOCK_HPP
#define	PROCESS_BLOCK_HPP

#include "subsample.hpp"
#include "dct.hpp"
#include "huffman_encode.hpp"

//extern huffman_t huffman_ctx[3];

#define	HUFFMAN_CTX_Y	&huffman_ctx[0]
#define	HUFFMAN_CTX_Cb	&huffman_ctx[1]
#define	HUFFMAN_CTX_Cr	&huffman_ctx[2]

void print_block_out(block_out* block)
{
	printf("\t%u\n", block->bit_position);
	unsigned int i;
	for (i = 0; i < 48; i++)
	{
		if ((block->content[i] == 0) & (((i + 1) * 32) < block->bit_position))
		{
			printf("error\n");
		}
		printf("\t%u ... %u: %x\n", ((i + 1) * 32) - 1, i * 32, block->content[i]);
	}
}

void write_block_out(block_out* out, huffman_encoding_out* huffman_out)
{
#ifdef VERBOSE
	printf("Adding %u bits to the buffer of block %u\n", huffman_out->bit_position, huffman_out->block_id);
#endif
	//	print_block_out(out);

	unsigned int i, current_position, test_count = 0;

	current_position = huffman_out->bit_position;

	// first shift whole words to the left
	while (current_position >= 32)
	{
		for (i = 47; i > 0; i--)
		{
			out->content[i] = out->content[i - 1];
		}
		current_position -= 32;
		test_count++;
	}

	for (i = 0; i < test_count; i++)
	{
		out->content[i] = 0;
	}
	if (current_position != 0)
	{
		// then shift the remaining individual bits
		unsigned int shift_amount_inversed = 32 - current_position;

		for (i = 47; i > 0; i--)
		{
			out->content[i] = (out->content[i - 1] >> shift_amount_inversed) | (out->content[i] << current_position);
		}

		out->content[0] = out->content[0] << current_position;
	}

	// now that all bits are shifted, concatenate both arrays
	for (i = 0; i < 8; i++)
	{
		out->content[i] = out->content[i] | huffman_out->content[i];
	}

	out->bit_position += huffman_out->bit_position;

//	print_block_out(out);
}

void init_block_out(block_out* out)
{
	out->bit_position = 0;
	memset(out->content, 0, 48 * sizeof(int));
}

//block_out current_block_out;

// changes the block_out variable declared above
// after this function is finished and before it gets called next,
// current_block_out needs to be processed

void process_current_block(const BGR current_color_block[16][16], block_out* current_block_out)
//void process_current_block(const bitmap_reader_out* in_data, block_out* current_block_out)

//void process_current_block(const bitmap_reader_out* in_data, encoded_block* out_data)
{
//	BGR current_color_block[16][16] = in_data->color_block;

//	block_out* current_block_out = &out_data->current_block_data;

// route all incoming information about the picture through this block
//	out_data->block_id = in_data->block_id;
//	out_data->height = in_data->height;
//	out_data->width = in_data->width;

	init_block_out(current_block_out);

	// four blocks of 8x8 Y
	short Y8x8[2][2][8][8];
	// one block of 8x8 Cr
	short Cb8x8[8][8];
	// one block of 8x8 Cb
	short Cr8x8[8][8];

	unsigned int i, j, r, c;

	// the out of the individual Huffman encoding steps
	// will be overwritten with every new step
	// but written to the block out before that
	huffman_encoding_out huffman_step_out;
	//	init_huffman_out(&huffman_step_out);

	// getting four 8x8 Y-blocks
	for (i = 0; i < 2; i++)
		for (j = 0; j < 2; j++)
		{
			for (r = 0; r < 8; r++)
				for (c = 0; c < 8; c++)
				{
					const unsigned rr = (i << 3) + r;
					const unsigned cc = (j << 3) + c;

					const color R = current_color_block[rr][cc].Red;
					const color G = current_color_block[rr][cc].Green;
					const color B = current_color_block[rr][cc].Blue;
					//					printf("R: %u G: %u B: %u\n", R, G, B);
					// converting RGB into Y (luminance)
					Y8x8[i][j][r][c] = RGB2Y(R, G, B) - 128;
				}
		}

	// getting subsampled Cb and Cr
	subsample(current_color_block, Cb8x8, Cr8x8);

	//	for (i = 0; i < 8; i++)
	//	{
	//		for (j = 0; j < 8; j++)
	//		{
	//			printf("%i ", Y8x8[0][0][i][j]);
	//		}
	//		printf("\n");
	//	}

	// 1 Y-compression
	dct3(Y8x8[0][0], Y8x8[0][0]);
	huffman_encode(HUFFMAN_CTX_Y, (short*) Y8x8[0][0], &huffman_step_out);

//	printf("Got %u bits form Huffman step\n", huffman_step_out.bit_position);
	write_block_out(current_block_out, &huffman_step_out);

//	printf("\n");

	// 2 Y-compression
	dct3(Y8x8[0][1], Y8x8[0][1]);
	huffman_encode(HUFFMAN_CTX_Y, (short*) Y8x8[0][1], &huffman_step_out);
//	printf("Got %u bits form Huffman step\n", huffman_step_out.bit_position);
	write_block_out(current_block_out, &huffman_step_out);
//	printf("\n");

	// 3 Y-compression
	dct3(Y8x8[1][0], Y8x8[1][0]);
	huffman_encode(HUFFMAN_CTX_Y, (short*) Y8x8[1][0], &huffman_step_out);
//	printf("Got %u bits form Huffman step\n", huffman_step_out.bit_position);
	write_block_out(current_block_out, &huffman_step_out);
//	printf("\n");

	// 4 Y-compression
	dct3(Y8x8[1][1], Y8x8[1][1]);
	huffman_encode(HUFFMAN_CTX_Y, (short*) Y8x8[1][1], &huffman_step_out);
//	printf("Got %u bits form Huffman step\n", huffman_step_out.bit_position);
	write_block_out(current_block_out, &huffman_step_out);
//	printf("\n");

	// Cb-compression
	dct3(Cb8x8, Cb8x8);
	huffman_encode(HUFFMAN_CTX_Cb, (short*) Cb8x8, &huffman_step_out);
//	printf("Got %u bits form Huffman step\n", huffman_step_out.bit_position);
	write_block_out(current_block_out, &huffman_step_out);
//	printf("\n");

	// Cr-compression
	dct3(Cr8x8, Cr8x8);
	huffman_encode(HUFFMAN_CTX_Cr, (short*) Cr8x8, &huffman_step_out);
//	printf("Got %u bits form Huffman step\n", huffman_step_out.bit_position);
	write_block_out(current_block_out, &huffman_step_out);
//	printf("\n");

}

#endif	/* PROCESS_BLOCK_HPP */


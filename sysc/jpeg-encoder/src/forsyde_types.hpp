#ifndef JPEG_FORSYDE_TYPES
#define JPEG_FORSYDE_TYPES


// ForSyDe-SystemC types

typedef ForSyDe::SDF::zipN <encoded_block, encoded_block, encoded_block, encoded_block> merge_blocks_zipper;
typedef ForSyDe::SDF::unzipN <bitmap_reader_out, bitmap_reader_out, bitmap_reader_out, bitmap_reader_out> rgb_block_unzipper;
typedef ForSyDe::SDF::unzipN <smaller_block, smaller_block, smaller_block, smaller_block, smaller_block, smaller_block> block_unzipper;
typedef ForSyDe::SDF::zipN <huffman_encoding_out, huffman_encoding_out, huffman_encoding_out, 
  huffman_encoding_out, huffman_encoding_out, huffman_encoding_out> encoded_blocks_zipper;

typedef ForSyDe::token_tuple <encoded_block,encoded_block,encoded_block,encoded_block> merge_blocks_in_t;
typedef ForSyDe::token_tuple <bitmap_reader_out,bitmap_reader_out,bitmap_reader_out,bitmap_reader_out> rgb_collector_out_t;
typedef ForSyDe::token_tuple <smaller_block,smaller_block,smaller_block,smaller_block,smaller_block,smaller_block>
  color_conversion_out_t;
typedef ForSyDe::token_tuple <huffman_encoding_out,huffman_encoding_out,huffman_encoding_out,huffman_encoding_out,
  huffman_encoding_out,huffman_encoding_out> concatenate_smaller_blocks_in_t;

DEFINE_TYPE(merge_blocks_in_t)
DEFINE_TYPE(rgb_collector_out_t)
DEFINE_TYPE(color_conversion_out_t)
DEFINE_TYPE(concatenate_smaller_blocks_in_t)


#endif

/* 
 * File:   top.hpp
 * Author: marcus
 *
 * Created on March 29, 2013, 5:44 PM
 */

#ifndef TOP_HPP
#define	TOP_HPP

#include "c_types.h"
#include "forsyde_types.hpp"

#include "read_bitmap.hpp"
#include "jpeg_writer.hpp"
#include "block_processor.hpp"

#include "forsyde.hpp"
#include <tuple>

using namespace ForSyDe;
using namespace ForSyDe::SDF;


void read_bitmap_func(tokens<bitmap_reader_out> &out, const tokens<bitmap_reader_out> &inp1)
{
  //	std::cout << "read_bitmap_func" << std::endl;
  int current_block_id = inp1[0].block_id;

#pragma ForSyDe begin READ_BITMAP
  if (current_block_id == -2)
    {
      //std::cout << "Initialization bitmap file reading, properties: ";
      init_bitmap(&out[0]);
    }
  else
    {
      read_bitmap(&inp1[0], &out[0]);
      //std::cout << "Reading block " << out[0].block_id << std::endl;
    }
#pragma ForSyDe end
}

void rgb_block_collector_func(tokens<rgb_collector_out_t> &out, const tokens<bitmap_reader_out> &inp1)
{
  //std::cout << "Block collector function" << std::endl;
  out = init<bitmap_reader_out,bitmap_reader_out,bitmap_reader_out,bitmap_reader_out>(1, { 1 , 1 , 1, 1 }); 
  
  *get<bitmap_reader_out, 0,0,0>(out) = *get<bitmap_reader_out, 0>(inp1);
  *get<bitmap_reader_out, 0,1,0>(out) = *get<bitmap_reader_out, 1>(inp1);
  *get<bitmap_reader_out, 0,2,0>(out) = *get<bitmap_reader_out, 2>(inp1);
  *get<bitmap_reader_out, 0,3,0>(out) = *get<bitmap_reader_out, 3>(inp1);
}

void write_jpeg_func(const encoded_block inp)
{
  
  block_out current_block = inp.current_block_data;
  int current_block_id = inp.block_id;

  int width = inp.width;
  int height = inp.height;


#pragma ForSyDe begin WRITE_JPEG
  if (current_block_id == -1)
    {
      // initialize jpeg out file
      std::cout << "Initializating JPEG file" << std::endl;
      initialize_jpeg_file(width, height);
    }
  else if (current_block_id == ((width * height) >> 8))
    {
      // if the image is completely processed, write the EOI marker and stop the simulation
      //std::cout << "Writing EOI" << std::endl;
      finalize_jpeg_file();
      sc_stop();
    }
  else
    {
      //std::cout << "Writing block " << current_block_id << std::endl;
      write_jpeg_file(&current_block);
    }
#pragma ForSyDe end

}


void merge_blocks_func(tokens<encoded_block>& out, const tokens<merge_blocks_in_t>& inp)
{
  out.resize(4);
  *get<encoded_block, 0>(out) = *get<encoded_block, 0,0,0> (inp);
  *get<encoded_block, 1>(out) = *get<encoded_block, 0,1,0> (inp);
  *get<encoded_block, 2>(out) = *get<encoded_block, 0,2,0> (inp);
  *get<encoded_block, 3>(out) = *get<encoded_block, 0,3,0> (inp);
}

void write_blocks_func(const encoded_block inp)
{
  write_jpeg_func(inp);
}

SC_MODULE(Top)
{
 public:
  // Signals
  SDF2SDF<bitmap_reader_out> bitmap_reader_out_to_fanout;
  SDF2SDF<bitmap_reader_out> bitmap_reader_to_delay;
  SDF2SDF<bitmap_reader_out> bitmap_reader_to_processing;
  SDF2SDF<bitmap_reader_out> bitmap_reader_next_state;
  bitmap_reader_out bitmap_reader_initial_state;

  SDF2SDF<rgb_collector_out_t> rgb_collector_out;

  rgb_block_unzipper *unzip_blocks;
  tokens<unsigned int> rgb_block_unzipper_rates;

  SDF2SDF<bitmap_reader_out> processing_in_1;
  SDF2SDF<bitmap_reader_out> processing_in_2;
  SDF2SDF<bitmap_reader_out> processing_in_3;
  SDF2SDF<bitmap_reader_out> processing_in_4;

  SDF2SDF<encoded_block> processing_to_writer_1;
  SDF2SDF<encoded_block> processing_to_writer_2;
  SDF2SDF<encoded_block> processing_to_writer_3;
  SDF2SDF<encoded_block> processing_to_writer_4;

  SDF2SDF<merge_blocks_in_t> block_merger_in;
  SDF2SDF<encoded_block> block_merger_out;

  merge_blocks_zipper *zip_blocks;
  tokens<unsigned> zip_blocks_rates;

  SC_CTOR(Top)
  {
    bitmap_reader_initial_state.block_id = -2;
    bitmap_reader_initial_state.height = 0;
    bitmap_reader_initial_state.width = 0;

    ForSyDe::SDF::make_delay("bitmap_reader_delay", // Name
			     bitmap_reader_initial_state, // Initial state
			     bitmap_reader_next_state, bitmap_reader_to_delay);

    ForSyDe::SDF::make_comb("read_bitmap", // Name
			    read_bitmap_func, // Function to invoke
			    1, // Production rate
			    1, // Consumption rate
			    bitmap_reader_out_to_fanout, // Out signal to drive
			    bitmap_reader_next_state // In signal
			    );

    auto bitmap_reader_fanout = ForSyDe::SDF::make_fanout("bitmap_reader_fanout", bitmap_reader_to_delay, bitmap_reader_out_to_fanout);
    bitmap_reader_fanout->oport1(bitmap_reader_to_processing);

    ForSyDe::SDF::make_comb("rgb_block_collector", rgb_block_collector_func, 1, 4, rgb_collector_out, bitmap_reader_to_processing);

    unzip_blocks = new rgb_block_unzipper("rgb_blocks_unzipper", { 1, 1, 1, 1 });
    unzip_blocks->iport1(rgb_collector_out);
    std::get < 0 > (unzip_blocks->oport)(processing_in_1);
    std::get < 1 > (unzip_blocks->oport)(processing_in_2);
    std::get < 2 > (unzip_blocks->oport)(processing_in_3);
    std::get < 3 > (unzip_blocks->oport)(processing_in_4);

    auto block_processor_1 = new block_processor("block_processor_1");
    block_processor_1->incoming_bitmap_block(processing_in_1);
    block_processor_1->outgoing_encoded_block(processing_to_writer_1);

    auto block_processor_2 = new block_processor("block_processor_2");
    block_processor_2->incoming_bitmap_block(processing_in_2);
    block_processor_2->outgoing_encoded_block(processing_to_writer_2);

    auto block_processor_3 = new block_processor("block_processor_3");
    block_processor_3->incoming_bitmap_block(processing_in_3);
    block_processor_3->outgoing_encoded_block(processing_to_writer_3);

    auto block_processor_4 = new block_processor("block_processor_4");
    block_processor_4->incoming_bitmap_block(processing_in_4);
    block_processor_4->outgoing_encoded_block(processing_to_writer_4);

    zip_blocks = new merge_blocks_zipper("block_zipper", { 1, 1, 1, 1 });
    std::get < 0 > (zip_blocks->iport)(processing_to_writer_1);
    std::get < 1 > (zip_blocks->iport)(processing_to_writer_2);
    std::get < 2 > (zip_blocks->iport)(processing_to_writer_3);
    std::get < 3 > (zip_blocks->iport)(processing_to_writer_4);
    zip_blocks->oport1(block_merger_in);

    ForSyDe::SDF::make_comb("merge_blocks", merge_blocks_func, 4, 1, block_merger_out, block_merger_in);

    ForSyDe::SDF::make_sink("write_blocks", // Name
			    write_blocks_func, // Function to invoke
			    block_merger_out // Signal to report
			    );

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

  void end_of_simulation()
  {
    std::cout << "Conversion finished..." << std::endl;
  }
#endif
  };

#endif	/* TOP_HPP */


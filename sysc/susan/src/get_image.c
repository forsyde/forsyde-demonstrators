#include "susan.h"
#include <string.h>
#include <stdio.h>


/**
 * Split the image into MCU blocks.
 * @param fileName
 *  Name of the input file
 * @param imgAfterGet
 *  The image to split. This is read from a *.pgm file.
 * @param block
 *  The MCU block that hold the current resulting split.
 */
void splitIntoMCUs(MCU_BLOCK* block, Image* imgAfterGet) {

  /* The image's size that is actually
   * taken into account during edge detection */
  static int new_x_size;
  new_x_size = imgAfterGet->xSize - 2 * OUTER_PIXEL;
  static int new_y_size;
  new_y_size = imgAfterGet->ySize - 2 * OUTER_PIXEL;

  /* Count the rows and columns of MCU blocks for the given image */
  static int remainder_last_column;
  remainder_last_column = new_x_size % BLOCK_SIZE;
  static int remainder_last_row;
  remainder_last_row = new_y_size % BLOCK_SIZE;
  static int n_rows;
  n_rows = new_y_size / BLOCK_SIZE + ((remainder_last_row > 0) ? 1
      : 0);
  static int n_columns;
  n_columns = new_x_size / BLOCK_SIZE + ((remainder_last_column > 0)
      ? 1 : 0);

  /* Variables to facilitate iteration of MCUs */
  static int row = 1;
  static int column = 1;

  /* Starting positions for the blocks of interest */
  static int f_i = 0;
  static int f_j = 0;
  f_i = 4 + (column - 1) * BLOCK_SIZE;
  f_j = 4 + (row - 1) * BLOCK_SIZE;

  /*We initialize the static data of the block.
   * That is the width and the height of the whole image*/
  block->IMAGE_WIDTH = imgAfterGet->xSize;
  block->IMAGE_HEIGHT = imgAfterGet->ySize;
  block->ROW = row;
  block->COLUMN = column;

  /*We copy the corresponding data from the whole image into the block*/
  takeMCU(block, f_i, f_j, n_columns, n_rows, remainder_last_row,
      remainder_last_column, imgAfterGet->xSize, row, column,
      imgAfterGet->imageBuffer);

  /*increment the row and column accordingly*/
  if (column < n_columns) {
    column++;
  }
  else if (row < n_rows) {
    column = 1;
    row++;
  }
  else {
  }
}

/**
 *  Function for reading a block of data from the whole data.
 *
 * @param block
 *  Block as a structure in which we are going to save the data for the block
 * @param f_i
 *  Index of the first pixel of the block of interest(without the overhead)
 *  regarding the x-axis of the picture
 * @param f_j
 *  Index of the first pixel of the block of interest(without the overhead)
 *  regarding the y-axis of the picture
 * @param n_columns
 *  Total number of columns of MCU blocks
 * @param n_rows
 *  Total number of rows of MCU blocks
 * @param remainder_last_row
 *  The remainding height of the last row
 * @param remainder_last_column
 *  The remainding width of the last column
 * @param x_size
 *  The width of the whole picture
 * @param row
 *  The row index of the current block we are reading
 * @param column
 *  The column index of the current block we are reading
 * @param tmp
 *  Pointer to the first pixel of the intensity array of the image
 */
void takeMCU(MCU_BLOCK *block, int f_i, int f_j, int n_columns, int n_rows,
    int remainder_last_row, int remainder_last_column, int x_size, int row,
    int column, uchar* tmp) {

  int b_start_x, b_start_y;

  int icr_1, icr_2;

  int i = 0;

  if (row == 1) {

    if (column == 1) {

      //TYPE 1
      b_start_x = f_i - OUTER_PIXEL;
      b_start_y = f_j - OUTER_PIXEL;

      tmp += b_start_y * x_size + b_start_x;

      icr_1 = 0;
      icr_2 = 0;
      /*We are coping the data of the block row by row from the input image*/
      for (i = 0; i < BLOCK_SIZE + OUTER_PIXEL + OVERLAP_PIXEL; i++) {
        memcpy(block->IN + icr_1, tmp + icr_2, BLOCK_SIZE + OUTER_PIXEL
            + OVERLAP_PIXEL);
        icr_1 += BLOCK_SIZE + OUTER_PIXEL + OVERLAP_PIXEL;
        icr_2 += x_size;
      }

      /*We write the dimensions of the block size into the block*/
      block->BL_HEIGHT = BLOCK_SIZE;
      block->BL_WIDTH = BLOCK_SIZE;

      /*We write data for the overhead we have added*/
      block->TOP = OUTER_PIXEL;
      block->BOTTOM = OVERLAP_PIXEL;
      block->LEFT = OUTER_PIXEL;
      block->RIGHT = OVERLAP_PIXEL;

    }
    else if (column == n_columns) {

      //TYPE 2
      b_start_x = f_i - OVERLAP_PIXEL;
      b_start_y = f_j - OUTER_PIXEL;

      tmp += b_start_y * x_size + b_start_x;

      int NEW_BLOCK_WIDTH = (remainder_last_column > 0) ? remainder_last_column
          : BLOCK_SIZE;

      icr_1 = 0;
      icr_2 = 0;

      for (i = 0; i < BLOCK_SIZE + OUTER_PIXEL + OVERLAP_PIXEL; i++) {
        memcpy(block->IN + icr_1, tmp + icr_2, NEW_BLOCK_WIDTH + OUTER_PIXEL
            + OVERLAP_PIXEL);
        icr_1 += NEW_BLOCK_WIDTH + OUTER_PIXEL + OVERLAP_PIXEL;
        icr_2 += x_size;
      }

      block->BL_HEIGHT = BLOCK_SIZE;
      block->BL_WIDTH = NEW_BLOCK_WIDTH;

      block->TOP = OUTER_PIXEL;
      block->BOTTOM = OVERLAP_PIXEL;
      block->LEFT = OVERLAP_PIXEL;
      block->RIGHT = OUTER_PIXEL;

    }
    else {
      //TYPE 6
      b_start_x = f_i - OVERLAP_PIXEL;
      b_start_y = f_j - OUTER_PIXEL;

      tmp += b_start_y * x_size + b_start_x;

      icr_1 = 0;
      icr_2 = 0;

      for (i = 0; i < BLOCK_SIZE + OUTER_PIXEL + OVERLAP_PIXEL; i++) {
        memcpy(block->IN + icr_1, tmp + icr_2, BLOCK_SIZE + 2 * OVERLAP_PIXEL);
        icr_1 += BLOCK_SIZE + 2 * OVERLAP_PIXEL;
        icr_2 += x_size;
      }

      block->BL_HEIGHT = BLOCK_SIZE;
      block->BL_WIDTH = BLOCK_SIZE;

      block->TOP = OUTER_PIXEL;
      block->BOTTOM = OVERLAP_PIXEL;
      block->LEFT = OVERLAP_PIXEL;
      block->RIGHT = OVERLAP_PIXEL;
    }

  }
  else if (row == n_rows) {

    if (column == 1) {
      //TYPE 4
      b_start_x = f_i - OUTER_PIXEL;
      b_start_y = f_j - OVERLAP_PIXEL;

      tmp += b_start_y * x_size + b_start_x;

      int NEW_BLOCK_HEIGHT = (remainder_last_row > 0) ? remainder_last_row
          : BLOCK_SIZE;

      icr_1 = 0;
      icr_2 = 0;

      for (i = 0; i < NEW_BLOCK_HEIGHT + OUTER_PIXEL + OVERLAP_PIXEL; i++) {
        memcpy(block->IN + icr_1, tmp + icr_2, BLOCK_SIZE + OUTER_PIXEL
            + OVERLAP_PIXEL);
        icr_1 += BLOCK_SIZE + OUTER_PIXEL + OVERLAP_PIXEL;
        icr_2 += x_size;
      }

      block->BL_HEIGHT = NEW_BLOCK_HEIGHT;
      block->BL_WIDTH = BLOCK_SIZE;

      block->TOP = OVERLAP_PIXEL;
      block->BOTTOM = OUTER_PIXEL;
      block->LEFT = OUTER_PIXEL;
      block->RIGHT = OVERLAP_PIXEL;

    }
    else if (column == n_columns) {

      //TYPE 3
      b_start_x = f_i - OVERLAP_PIXEL;
      b_start_y = f_j - OVERLAP_PIXEL;

      tmp += b_start_y * x_size + b_start_x;

      int NEW_BLOCK_HEIGHT = (remainder_last_row > 0) ? remainder_last_row
          : BLOCK_SIZE;
      int NEW_BLOCK_WIDTH = (remainder_last_column > 0) ? remainder_last_column
          : BLOCK_SIZE;

      icr_1 = 0;
      icr_2 = 0;

      for (i = 0; i < NEW_BLOCK_HEIGHT + OUTER_PIXEL + OVERLAP_PIXEL; i++) {
        memcpy(block->IN + icr_1, tmp + icr_2, NEW_BLOCK_WIDTH + OUTER_PIXEL
            + OVERLAP_PIXEL);
        icr_1 += NEW_BLOCK_WIDTH + OUTER_PIXEL + OVERLAP_PIXEL;
        icr_2 += x_size;
      }

      block->BL_HEIGHT = NEW_BLOCK_HEIGHT;
      block->BL_WIDTH = NEW_BLOCK_WIDTH;

      block->TOP = OVERLAP_PIXEL;
      block->BOTTOM = OUTER_PIXEL;
      block->LEFT = OVERLAP_PIXEL;
      block->RIGHT = OUTER_PIXEL;

    }
    else {

      //TYPE 8
      b_start_x = f_i - OVERLAP_PIXEL;
      b_start_y = f_j - OVERLAP_PIXEL;

      tmp += b_start_y * x_size + b_start_x;
      int NEW_BLOCK_HEIGHT = (remainder_last_row > 0) ? remainder_last_row
          : BLOCK_SIZE;

      icr_1 = 0;
      icr_2 = 0;

      for (i = 0; i < NEW_BLOCK_HEIGHT + OUTER_PIXEL + OVERLAP_PIXEL; i++) {
        memcpy(block->IN + icr_1, tmp + icr_2, BLOCK_SIZE + 2 * OVERLAP_PIXEL);
        icr_1 += BLOCK_SIZE + 2 * OVERLAP_PIXEL;
        icr_2 += x_size;
      }

      block->BL_HEIGHT = NEW_BLOCK_HEIGHT;
      block->BL_WIDTH = BLOCK_SIZE;

      block->TOP = OVERLAP_PIXEL;
      block->BOTTOM = OUTER_PIXEL;
      block->LEFT = OVERLAP_PIXEL;
      block->RIGHT = OVERLAP_PIXEL;

    }

  }
  else {

    if (column == 1) {

      //TYPE 5
      b_start_x = f_i - OUTER_PIXEL;
      b_start_y = f_j - OVERLAP_PIXEL;

      tmp += b_start_y * x_size + b_start_x;

      icr_1 = 0;
      icr_2 = 0;

      for (i = 0; i < BLOCK_SIZE + 2 * OVERLAP_PIXEL; i++) {
        memcpy(block->IN + icr_1, tmp + icr_2, BLOCK_SIZE + OUTER_PIXEL
            + OVERLAP_PIXEL);
        icr_1 += BLOCK_SIZE + OUTER_PIXEL + OVERLAP_PIXEL;
        icr_2 += x_size;
      }

      block->BL_HEIGHT = BLOCK_SIZE;
      block->BL_WIDTH = BLOCK_SIZE;

      block->TOP = OVERLAP_PIXEL;
      block->BOTTOM = OVERLAP_PIXEL;
      block->LEFT = OUTER_PIXEL;
      block->RIGHT = OVERLAP_PIXEL;

    }
    else if (column == n_columns) {

      //TYPE 7
      b_start_x = f_i - OVERLAP_PIXEL;
      b_start_y = f_j - OVERLAP_PIXEL;

      tmp += b_start_y * x_size + b_start_x;

      int NEW_BLOCK_WIDTH = (remainder_last_column > 0) ? remainder_last_column
          : BLOCK_SIZE;

      icr_1 = 0;
      icr_2 = 0;

      for (i = 0; i < BLOCK_SIZE + 2 * OVERLAP_PIXEL; i++) {
        memcpy(block->IN + icr_1, tmp + icr_2, NEW_BLOCK_WIDTH + OUTER_PIXEL
            + OVERLAP_PIXEL);
        icr_1 += NEW_BLOCK_WIDTH + OUTER_PIXEL + OVERLAP_PIXEL;
        icr_2 += x_size;
      }

      block->BL_HEIGHT = BLOCK_SIZE;
      block->BL_WIDTH = NEW_BLOCK_WIDTH;

      block->TOP = OVERLAP_PIXEL;
      block->BOTTOM = OVERLAP_PIXEL;
      block->LEFT = OVERLAP_PIXEL;
      block->RIGHT = OUTER_PIXEL;

    }
    else {

      //TYPE 9
      b_start_x = f_i - OVERLAP_PIXEL;
      b_start_y = f_j - OVERLAP_PIXEL;

      tmp += b_start_y * x_size + b_start_x;

      icr_1 = 0;
      icr_2 = 0;

      for (i = 0; i < BLOCK_SIZE + 2 * OVERLAP_PIXEL; i++) {
        memcpy(block->IN + icr_1, tmp + icr_2, BLOCK_SIZE + 2 * OVERLAP_PIXEL);
        icr_1 += BLOCK_SIZE + 2 * OVERLAP_PIXEL;
        icr_2 += x_size;
      }

      block->BL_HEIGHT = BLOCK_SIZE;
      block->BL_WIDTH = BLOCK_SIZE;

      block->TOP = OVERLAP_PIXEL;
      block->BOTTOM = OVERLAP_PIXEL;
      block->LEFT = OVERLAP_PIXEL;
      block->RIGHT = OVERLAP_PIXEL;
    }
  }

  if ((row == n_rows) && (column == n_columns)) {
    block->ROW *= -1;
  }

}


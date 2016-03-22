#include "susan.h"
#include <stdio.h>

static uchar initialized=0;
static uchar last_MCU=0;
/**
 * Function to write the resulting image (with drawn edges) to a *.pgm file
 * @param fileName
 *  The *.pgm file to write to.
 * @param imgAfterThin
 *  Block having a part of the image. In this case, it is the last block.
 * @param outputImageBuffer
 *  Array that keeps the intensity data for every pixel of the whole image.
 */
void writeToFile(char* fileName, const MCU_BLOCK* imgAfterThin,
    uchar* outputImageBuffer) {
  /*We save the final result into an output image*/
	FILE *file;
	file = fopen("files/output.pgm","a+");

  int n;
  for(n=0; n < imgAfterThin->IMAGE_WIDTH*imgAfterThin->IMAGE_HEIGHT; n++)
  {
	  // send it to the output
	  fprintf(file,"%03d ",outputImageBuffer[n]);
  }
  fclose(file);

}

/**
 * Stitching the part within the central arrays for the brightness
 * and the strength of 'edginess' of every pixel, draw the edges,
 * then write MCUs to an output file
 * @param fileName
 *  The name of the output file
 * @param imgOutput
 *  MCU block that have to be stiched
 * @param edgeDir
 *  Block that keeps data about the level of 'edginess'
 */
void wrapUp(const MCU_BLOCK* imgOutput, const EdgeDirection *edgeDir) {

  static uchar outputImageBuffer[WIDTH*HEIGHT];
  static uchar outputMidBuffer[WIDTH*HEIGHT];

  if(initialized == 0){

    initialized=1;
    memset(outputMidBuffer, 100, imgOutput->IMAGE_WIDTH
        * imgOutput->IMAGE_HEIGHT);

  }

  stitch((MCU_BLOCK*)imgOutput, outputImageBuffer, outputMidBuffer, edgeDir);



  if (last_MCU == 1) {
    edgeDraw(outputMidBuffer, outputImageBuffer, imgOutput->IMAGE_WIDTH,
        imgOutput->IMAGE_HEIGHT);
    writeToFile(OUTPUT_FILE, imgOutput, outputImageBuffer);
    // Do nothing
  }
}

/**
 *Function for stitching the processed blocks as they arrive
 *
 * @param imgOutput
 *  Block having a part of the image
 * @param outputImageBuffer
 *  Array that keeps the intensity data for every pixel of the whole image.
 *  Data from imgOutput is copied into this array.
 * @param outputMidBuffer.
 *  Array that keeps the data for the strength of the 'edginess' of
 *  every pixel of the whole image. Data from edgeDir is copied into this array.
 * @param edgeDir
 *  Block that holds the data for the strength of the 'edginess' of
 *  every pixel of interest
 */
void stitch(MCU_BLOCK* imgOutput, uchar * outputImageBuffer,
    uchar * outputMidBuffer, const EdgeDirection *edgeDir) {

  /*If this is the final block, than this is the last loop*/
  if (imgOutput->ROW < 0){
    imgOutput->ROW *= -1;
    last_MCU=1;
  }

  /*--------------------STITCH IMAGE--------------------*/
  /*{{{ First we copy the central part(without the overhead) within the block
   * into the outputImageBuffer*/
  int positionOutput, positionInput;
  positionOutput = ((imgOutput->ROW - 1) * BLOCK_SIZE + OUTER_PIXEL)
      * imgOutput->IMAGE_WIDTH + (imgOutput->COLUMN - 1) * BLOCK_SIZE
      + OUTER_PIXEL;
  positionInput = imgOutput->TOP * (imgOutput->BL_WIDTH + imgOutput->RIGHT
      + imgOutput->LEFT) + imgOutput->LEFT;

  int i;
  for (i = 1; i <= imgOutput->BL_HEIGHT; i++) {
    memcpy(outputImageBuffer + positionOutput, imgOutput->IN + positionInput,
        imgOutput->BL_WIDTH);
    positionInput += imgOutput->BL_WIDTH + imgOutput->RIGHT + imgOutput->LEFT;
    positionOutput += imgOutput->IMAGE_WIDTH;
  }
  /*}}}*/

  /*{{{ Now we copy the OUTER_PIXELs only from those blocks that are stitched
   * to the borders of the image. There are 8 types of such blocks.
   * As a reference, see the Analysis document.*/
  if (imgOutput->TOP == OUTER_PIXEL) {

    if (imgOutput->LEFT == OUTER_PIXEL) {

      positionOutput = 0;
      positionInput = 0;

      for (i = 1; i <= imgOutput->TOP; i++) {

        memcpy(outputImageBuffer + positionOutput, imgOutput->IN
            + positionInput, imgOutput->BL_WIDTH + imgOutput->LEFT);
        positionOutput += imgOutput->IMAGE_WIDTH;
        positionInput += imgOutput->BL_WIDTH + imgOutput->RIGHT
            + imgOutput->LEFT;

      }

      for (i = 1; i <= imgOutput->BL_HEIGHT; i++) {

        memcpy(outputImageBuffer + positionOutput, imgOutput->IN
            + positionInput, imgOutput->LEFT);
        positionOutput += imgOutput->IMAGE_WIDTH;
        positionInput += imgOutput->BL_WIDTH + imgOutput->RIGHT
            + imgOutput->LEFT;

      }

    }
    else if (imgOutput->RIGHT == OUTER_PIXEL) {

      positionOutput = imgOutput->IMAGE_WIDTH - imgOutput->BL_WIDTH
          - imgOutput->RIGHT;
      positionInput = imgOutput->LEFT;

      for (i = 1; i <= imgOutput->TOP; i++) {

        memcpy(outputImageBuffer + positionOutput, imgOutput->IN
            + positionInput, imgOutput->BL_WIDTH + imgOutput->RIGHT);
        positionOutput += imgOutput->IMAGE_WIDTH;
        positionInput += imgOutput->BL_WIDTH + imgOutput->RIGHT
            + imgOutput->LEFT;

      }

      positionOutput += imgOutput->BL_WIDTH;
      positionInput += imgOutput->BL_WIDTH;

      for (i = 1; i <= imgOutput->BL_HEIGHT; i++) {

        memcpy(outputImageBuffer + positionOutput, imgOutput->IN
            + positionInput, imgOutput->RIGHT);
        positionOutput += imgOutput->IMAGE_WIDTH;
        positionInput += imgOutput->BL_WIDTH + imgOutput->RIGHT
            + imgOutput->LEFT;

      }

    }
    else {

      positionOutput = (imgOutput->COLUMN - 1) * BLOCK_SIZE + OUTER_PIXEL;
      positionInput = imgOutput->LEFT;

      for (i = 1; i <= imgOutput->TOP; i++) {

        memcpy(outputImageBuffer + positionOutput, imgOutput->IN
            + positionInput, imgOutput->BL_WIDTH);
        positionOutput += imgOutput->IMAGE_WIDTH;
        positionInput += imgOutput->BL_WIDTH + imgOutput->RIGHT
            + imgOutput->LEFT;

      }

    }

  }
  else if (imgOutput->BOTTOM == OUTER_PIXEL) {

    if (imgOutput->LEFT == OUTER_PIXEL) {

      positionOutput = (imgOutput->IMAGE_HEIGHT - imgOutput->BOTTOM)
          * imgOutput->IMAGE_WIDTH;
      positionInput = (imgOutput->BL_HEIGHT + imgOutput->TOP)
          * (imgOutput->BL_WIDTH + imgOutput->RIGHT + imgOutput->LEFT);

      for (i = 1; i <= imgOutput->BOTTOM; i++) {

        memcpy(outputImageBuffer + positionOutput, imgOutput->IN
            + positionInput, imgOutput->BL_WIDTH + imgOutput->LEFT);
        positionOutput += imgOutput->IMAGE_WIDTH;
        positionInput += imgOutput->BL_WIDTH + imgOutput->RIGHT
            + imgOutput->LEFT;

      }

      positionOutput = (imgOutput->IMAGE_HEIGHT - imgOutput->BOTTOM
          - imgOutput->BL_HEIGHT) * imgOutput->IMAGE_WIDTH;
      positionInput = imgOutput->TOP * (imgOutput->BL_WIDTH + imgOutput->RIGHT
          + imgOutput->LEFT);

      for (i = 1; i <= imgOutput->BL_HEIGHT; i++) {

        memcpy(outputImageBuffer + positionOutput, imgOutput->IN
            + positionInput, imgOutput->LEFT);
        positionOutput += imgOutput->IMAGE_WIDTH;
        positionInput += imgOutput->BL_WIDTH + imgOutput->RIGHT
            + imgOutput->LEFT;

      }

    }
    else if (imgOutput->RIGHT == OUTER_PIXEL) {

      positionOutput = (imgOutput->IMAGE_HEIGHT - imgOutput->BOTTOM + 1)
          * imgOutput->IMAGE_WIDTH - imgOutput->BL_WIDTH - imgOutput->RIGHT;
      positionInput = (imgOutput->BL_HEIGHT + imgOutput->TOP)
          * (imgOutput->BL_WIDTH + imgOutput->RIGHT + imgOutput->LEFT)
          + imgOutput->LEFT;

      for (i = 1; i <= imgOutput->BOTTOM; i++) {

        memcpy(outputImageBuffer + positionOutput, imgOutput->IN
            + positionInput, imgOutput->BL_WIDTH + imgOutput->RIGHT);
        positionOutput += imgOutput->IMAGE_WIDTH;
        positionInput += imgOutput->BL_WIDTH + imgOutput->RIGHT
            + imgOutput->LEFT;

      }

      positionOutput = (imgOutput->IMAGE_HEIGHT - imgOutput->BOTTOM
          - imgOutput->BL_HEIGHT + 1) * imgOutput->IMAGE_WIDTH
          - imgOutput->RIGHT;
      positionInput = (imgOutput->TOP + 1) * (imgOutput->BL_WIDTH
          + imgOutput->RIGHT + imgOutput->LEFT) - imgOutput->RIGHT;

      for (i = 1; i <= imgOutput->BL_HEIGHT; i++) {

        memcpy(outputImageBuffer + positionOutput, imgOutput->IN
            + positionInput, imgOutput->RIGHT);
        positionOutput += imgOutput->IMAGE_WIDTH;
        positionInput += imgOutput->BL_WIDTH + imgOutput->RIGHT
            + imgOutput->LEFT;

      }

    }
    else {

      positionOutput = (imgOutput->IMAGE_HEIGHT - imgOutput->BOTTOM)
          * imgOutput->IMAGE_WIDTH + OUTER_PIXEL + (imgOutput->COLUMN - 1)
          * BLOCK_SIZE;
      positionInput = (imgOutput->BL_HEIGHT + imgOutput->TOP)
          * (imgOutput->BL_WIDTH + imgOutput->RIGHT + imgOutput->LEFT)
          + imgOutput->LEFT;

      for (i = 1; i <= imgOutput->BOTTOM; i++) {

        memcpy(outputImageBuffer + positionOutput, imgOutput->IN
            + positionInput, imgOutput->BL_WIDTH);
        positionOutput += imgOutput->IMAGE_WIDTH;
        positionInput += imgOutput->BL_WIDTH + imgOutput->RIGHT
            + imgOutput->LEFT;

      }
    }

  }
  else {

    if (imgOutput->LEFT == OUTER_PIXEL) {

      positionOutput = ((imgOutput->ROW - 1) * BLOCK_SIZE + OUTER_PIXEL)
          * imgOutput->IMAGE_WIDTH;
      positionInput = imgOutput->TOP * (imgOutput->BL_WIDTH + imgOutput->RIGHT
          + imgOutput->LEFT);

      for (i = 1; i <= imgOutput->BL_HEIGHT; i++) {

        memcpy(outputImageBuffer + positionOutput, imgOutput->IN
            + positionInput, imgOutput->LEFT);
        positionOutput += imgOutput->IMAGE_WIDTH;
        positionInput += imgOutput->BL_WIDTH + imgOutput->RIGHT
            + imgOutput->LEFT;

      }

    }
    else if (imgOutput->RIGHT == OUTER_PIXEL) {

      positionOutput = ((imgOutput->ROW - 1) * BLOCK_SIZE + OUTER_PIXEL + 1)
          * imgOutput->IMAGE_WIDTH - imgOutput->RIGHT;
      positionInput = (imgOutput->TOP + 1) * (imgOutput->BL_WIDTH
          + imgOutput->RIGHT + imgOutput->LEFT) - imgOutput->RIGHT;

      for (i = 1; i <= imgOutput->BL_HEIGHT; i++) {

        memcpy(outputImageBuffer + positionOutput, imgOutput->IN
            + positionInput, imgOutput->RIGHT);
        positionOutput += imgOutput->IMAGE_WIDTH;
        positionInput += imgOutput->BL_WIDTH + imgOutput->RIGHT
            + imgOutput->LEFT;

      }

    }

  }
  /*}}}*/
  /*--------------------END: STITCH IMAGE--------------------*/

  /*--------------------STITCH MID--------------------*/
  /*{{{ We also stitch the data for the strength of the 'edginess' of
   *  every pixel of the whole image */



  /*{{{ Than we copy the central part of the edgeDir into the outputMidBuffer*/
  positionOutput = ((imgOutput->ROW - 1) * BLOCK_SIZE + OUTER_PIXEL)
      * imgOutput->IMAGE_WIDTH + (imgOutput->COLUMN - 1) * BLOCK_SIZE
      + OUTER_PIXEL;
  positionInput = imgOutput->TOP * (imgOutput->BL_WIDTH + imgOutput->RIGHT
      + imgOutput->LEFT) + imgOutput->LEFT;

  for (i = 1; i <= imgOutput->BL_HEIGHT; i++) {
    memcpy(outputMidBuffer + positionOutput,
        edgeDir->midBuffer + positionInput, imgOutput->BL_WIDTH);
    positionInput += imgOutput->BL_WIDTH + imgOutput->RIGHT + imgOutput->LEFT;
    positionOutput += imgOutput->IMAGE_WIDTH;
  }
  /*}}}*/
  /*}}}*/
  /*--------------------END: STITCH MID--------------------*/
}

/**
 * Function for composing the final output considering the original image
 * (outputImageBuffer) and the strength of 'edginess'
 * of the pixels (outputMidBuffer)
 *
 * @param outputMidBuffer
 *  Array that keeps the data for the strength of the 'edginess' of
 *  every pixel of the whole image. Data from edgeDir is copied into this array.
 * @param outputImageBuffer
 *  Array that keeps the intensity data for every pixel of the whole image.
 *  Data from imgOutput is copied into this array.
 * @param x_size
 *  The width of the whole image
 * @param y_size
 *  The height of the whole image
 */
void edgeDraw(uchar* outputMidBuffer, uchar* outputImageBuffer,
    const int x_size, const int y_size) {
  int i;
  uchar *inp, *midp;

  uchar *in = outputImageBuffer;
  uchar *mid = outputMidBuffer;

  if (DRAWING_MODE == 0) {
    /* mark 3x3 white block around each edrawingdge point */
    midp = mid;
    for (i = 0; i < x_size * y_size; i++) {
      if (*midp < 8) {
        inp = in + (midp - mid) - x_size - 1;
        *inp++ = 255;
        *inp++ = 255;
        *inp = 255;
        inp += x_size - 2;
        *inp++ = 255;
        //*inp++;
        *inp = 255;
        inp += x_size - 2;
        *inp++ = 255;
        *inp++ = 255;
        *inp = 255;
      }
      midp++;
    }
  }

  /* now mark 1 black pixel at each edge point */
  midp = mid;
  for (i = 0; i < x_size * y_size; i++) {
    if (*midp < 8)
      *(in + (midp - mid)) = 0;
    midp++;
  }
}


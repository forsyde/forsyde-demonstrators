#include "susan.h"

static uchar initilized = 0;
static BrightnessLUT brightLUT;

/**
 * Function for calculating the USAN for every pixel of interest
 * within the block
 *
 * @param imgInput
 *  Block having a part of the image
 * @param strength
 *  Block that holds the data for the USAN of every pixel of interest
 */
void susanUsan(const MCU_BLOCK *imgInput, MCU_BLOCK *imgOutput, EdgeStrength *strength) {
  int k;
  float temp;

  int i, j, n;
  uchar *p, *cp;

  int x_size, y_size;
  uchar *bp;
  uchar *in;
  int *r;

  /* relay the image from the input token to the output token */
  memcpy(imgOutput, imgInput, sizeof(MCU_BLOCK));

  if (initilized == 0) {
    /* setupBrightnessLut will be executed only once on
     * the first time this function is invoked. */
    for (k = -256; k < 257; k++) {
      if (k < -25 || k > 25) {
        brightLUT.bpBuffer[258 + k] = 0;
      } else {
        temp = ((float) k) / ((float) BRIGHT_THRESHOLD);
        temp = temp * temp;
        if (FORM_EDGE_CORNER == 6)
          temp = temp * temp * temp;
        temp = 100.0 * exp(-temp);
        brightLUT.bpBuffer[258 + k] = (uchar) temp;
      }
    }
    initilized = 1;
  }

  /* 
   Initialize x_size, y_size, bp, in, and r
   according to the new input arguments,
   in order to keep the rest of this function
   consistent with the original source code.
   */

  /*Total width of the block*/
  x_size = imgInput->BL_WIDTH + imgInput->LEFT + imgInput->RIGHT;

  /*Total height of the block*/
  y_size = imgInput->BL_HEIGHT + imgInput->TOP + imgInput->BOTTOM;

  /*We position to the center of the lookup table*/
  bp = (uchar*) (brightLUT.bpBuffer + 258);

  in = (uchar*) imgInput->IN;
  r = strength->rBuffer;

  memset(strength->rBuffer, 0, sizeof(EdgeStrength));

  /* Following is the same as the source code */
  for (i = 3; i < y_size - 3; i++)
    for (j = 3; j < x_size - 3; j++) {
      n = 100;
      p = in + (i - 3) * x_size + j - 1;
      cp = bp + in[i * x_size + j];

      n += *(cp - *p++);
      n += *(cp - *p++);
      n += *(cp - *p);
      p += x_size - 3;

      n += *(cp - *p++);
      n += *(cp - *p++);
      n += *(cp - *p++);
      n += *(cp - *p++);
      n += *(cp - *p);
      p += x_size - 5;

      n += *(cp - *p++);
      n += *(cp - *p++);
      n += *(cp - *p++);
      n += *(cp - *p++);
      n += *(cp - *p++);
      n += *(cp - *p++);
      n += *(cp - *p);
      p += x_size - 6;

      n += *(cp - *p++);
      n += *(cp - *p++);
      n += *(cp - *p);
      p += 2;
      n += *(cp - *p++);
      n += *(cp - *p++);
      n += *(cp - *p);
      p += x_size - 6;

      n += *(cp - *p++);
      n += *(cp - *p++);
      n += *(cp - *p++);
      n += *(cp - *p++);
      n += *(cp - *p++);
      n += *(cp - *p++);
      n += *(cp - *p);
      p += x_size - 5;

      n += *(cp - *p++);
      n += *(cp - *p++);
      n += *(cp - *p++);
      n += *(cp - *p++);
      n += *(cp - *p);
      p += x_size - 3;

      n += *(cp - *p++);
      n += *(cp - *p++);
      n += *(cp - *p);

      if (n <= MAX_NO_EDGES)
        r[i * x_size + j] = MAX_NO_EDGES - n;
    }
}


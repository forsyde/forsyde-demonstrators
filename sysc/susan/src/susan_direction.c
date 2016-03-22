#include "susan.h"

static uchar initilized = 0;
static BrightnessLUT brightLUTDir;

/**
 *  Function to determine whether the edge pixel assumed in the susan_usan
 *  method is really an edge point
 *
 * @param imgInput
 *  Block having a part of the image
 * @param strength
 *  Block that holds the data for the USAN of every pixel of interest
 * @param direction
 *  Block that holds the data for the strength of the 'edginess' of
 *  every pixel of interest
 */


void susanDirection(const MCU_BLOCK *imgInput, const EdgeStrength *strength,
       MCU_BLOCK *imgOutput, EdgeStrength *strengthOutput, EdgeDirection *direction) 
{
  int k;
  float temp;

  float z;
  int do_symmetry, i, j, m, n, a, b, x, y, w;
  uchar c, *p, *cp;

  int x_size, y_size;
  uchar *bp, *in, *mid;
  int *r;


  /* relay the MCU_BLOCK and EdgeStrength from the input token to the output token */
  memcpy(imgOutput, imgInput, sizeof(MCU_BLOCK));
  memcpy(strengthOutput, strength, sizeof(EdgeStrength));


  if (initilized == 0) {
    /* setupBrightnessLut will be executed only once on
     * the first time this function is invoked.
     * */
    for (k = -256; k < 257; k++) {
      if (k < -25 || k > 25) {
        brightLUTDir.bpBuffer[258 + k] = 0;
      } else {
        temp = ((float) k) / ((float) BRIGHT_THRESHOLD);
        temp = temp * temp;
        if (FORM_EDGE_CORNER == 6)
          temp = temp * temp * temp;
        temp = 100.0 * exp(-temp);
        brightLUTDir.bpBuffer[258 + k] = (uchar) temp;
      }
    }
    initilized = 1;
  }

  /* 
   Initialize x_size, y_size, bp, in, and r
   according to the new input arguments,
   in order to keep the rest of this
   function consistent with the original source code.
   */

  x_size = imgInput->BL_WIDTH + imgInput->LEFT + imgInput->RIGHT;
  y_size = imgInput->BL_HEIGHT + imgInput->TOP + imgInput->BOTTOM;
  bp = (uchar*) (brightLUTDir.bpBuffer + 258);
  in = (uchar*) imgInput->IN;
  r = (int*) strength->rBuffer;
  mid = direction->midBuffer;

  memset(direction, 100, sizeof(EdgeDirection));

  int left, right, bottom, top;
  left = (imgInput->LEFT == OVERLAP_PIXEL) ? OVERLAP_PIXEL - 2 : imgInput->LEFT;
  right = (imgInput->RIGHT == OVERLAP_PIXEL) ? OVERLAP_PIXEL - 2
      : imgInput->RIGHT;
  top = (imgInput->TOP == OVERLAP_PIXEL) ? OVERLAP_PIXEL - 2 : imgInput->TOP;
  bottom = (imgInput->BOTTOM == OVERLAP_PIXEL) ? OVERLAP_PIXEL - 2
      : imgInput->BOTTOM;

  /* following is the same as the origninal source code */
  for (i = top; i < y_size - bottom; i++)
    for (j = left; j < x_size - right; j++) {

      if (r[i * x_size + j] > 0) {

        m = r[i * x_size + j];
        n = MAX_NO_EDGES - m;
        cp = bp + in[i * x_size + j];

        if (n > 600) {

          p = in + (i - 3) * x_size + j - 1;
          x = 0;
          y = 0;

          c = *(cp - *p++);
          x -= c;
          y -= 3 * c;
          c = *(cp - *p++);
          y -= 3 * c;
          c = *(cp - *p);
          x += c;
          y -= 3 * c;
          p += x_size - 3;

          c = *(cp - *p++);
          x -= 2 * c;
          y -= 2 * c;
          c = *(cp - *p++);
          x -= c;
          y -= 2 * c;
          c = *(cp - *p++);
          y -= 2 * c;
          c = *(cp - *p++);
          x += c;
          y -= 2 * c;
          c = *(cp - *p);
          x += 2 * c;
          y -= 2 * c;
          p += x_size - 5;

          c = *(cp - *p++);
          x -= 3 * c;
          y -= c;
          c = *(cp - *p++);
          x -= 2 * c;
          y -= c;
          c = *(cp - *p++);
          x -= c;
          y -= c;
          c = *(cp - *p++);
          y -= c;
          c = *(cp - *p++);
          x += c;
          y -= c;
          c = *(cp - *p++);
          x += 2 * c;
          y -= c;
          c = *(cp - *p);
          x += 3 * c;
          y -= c;
          p += x_size - 6;

          c = *(cp - *p++);
          x -= 3 * c;
          c = *(cp - *p++);
          x -= 2 * c;
          c = *(cp - *p);
          x -= c;
          p += 2;
          c = *(cp - *p++);
          x += c;
          c = *(cp - *p++);
          x += 2 * c;
          c = *(cp - *p);
          x += 3 * c;
          p += x_size - 6;

          c = *(cp - *p++);
          x -= 3 * c;
          y += c;
          c = *(cp - *p++);
          x -= 2 * c;
          y += c;
          c = *(cp - *p++);
          x -= c;
          y += c;
          c = *(cp - *p++);
          y += c;
          c = *(cp - *p++);
          x += c;
          y += c;
          c = *(cp - *p++);
          x += 2 * c;
          y += c;
          c = *(cp - *p);
          x += 3 * c;
          y += c;
          p += x_size - 5;

          c = *(cp - *p++);
          x -= 2 * c;
          y += 2 * c;
          c = *(cp - *p++);
          x -= c;
          y += 2 * c;
          c = *(cp - *p++);
          y += 2 * c;
          c = *(cp - *p++);
          x += c;
          y += 2 * c;
          c = *(cp - *p);
          x += 2 * c;
          y += 2 * c;
          p += x_size - 3;

          c = *(cp - *p++);
          x -= c;
          y += 3 * c;
          c = *(cp - *p++);
          y += 3 * c;
          c = *(cp - *p);
          x += c;
          y += 3 * c;

          z = sqrt((float) ((x * x) + (y * y)));
          if (z > (0.9 * (float) n)) {

            do_symmetry = 0;

            if (x == 0)
              z = 1000000.0;
            else
              z = ((float) y) / ((float) x);

            if (z < 0) {
              z = -z;
              w = -1;
            }
            else
              w = 1;

            if (z < 0.5) {
              /* vert_edge */
              a = 0;
              b = 1;
            }
            else {
              if (z > 2.0) {
                /* hor_edge */
                a = 1;
                b = 0;
              }
              else {
                /* diag_edge */
                if (w > 0) {
                  a = 1;
                  b = 1;
                }
                else {
                  a = -1;
                  b = 1;
                }
              }
            }

            if ((m > r[(i + a) * x_size + j + b]) && (m >= r[(i - a) * x_size
                + j - b]) && (m > r[(i + (2 * a)) * x_size + j + (2 * b)])
                && (m >= r[(i - (2 * a)) * x_size + j - (2 * b)]))
              mid[i * x_size + j] = 1;
          }
          else
            do_symmetry = 1;
        }
        else
          do_symmetry = 1;

        if (do_symmetry == 1) {

          p = in + (i - 3) * x_size + j - 1;
          x = 0;
          y = 0;
          w = 0;

          /*   |      \
               y  -x-  w
           |        \   */

          c = *(cp - *p++);
          x += c;
          y += 9 * c;
          w += 3 * c;
          c = *(cp - *p++);
          y += 9 * c;
          c = *(cp - *p);
          x += c;
          y += 9 * c;
          w -= 3 * c;
          p += x_size - 3;

          c = *(cp - *p++);
          x += 4 * c;
          y += 4 * c;
          w += 4 * c;
          c = *(cp - *p++);
          x += c;
          y += 4 * c;
          w += 2 * c;
          c = *(cp - *p++);
          y += 4 * c;
          c = *(cp - *p++);
          x += c;
          y += 4 * c;
          w -= 2 * c;
          c = *(cp - *p);
          x += 4 * c;
          y += 4 * c;
          w -= 4 * c;
          p += x_size - 5;

          c = *(cp - *p++);
          x += 9 * c;
          y += c;
          w += 3 * c;
          c = *(cp - *p++);
          x += 4 * c;
          y += c;
          w += 2 * c;
          c = *(cp - *p++);
          x += c;
          y += c;
          w += c;
          c = *(cp - *p++);
          y += c;
          c = *(cp - *p++);
          x += c;
          y += c;
          w -= c;
          c = *(cp - *p++);
          x += 4 * c;
          y += c;
          w -= 2 * c;
          c = *(cp - *p);
          x += 9 * c;
          y += c;
          w -= 3 * c;
          p += x_size - 6;

          c = *(cp - *p++);
          x += 9 * c;
          c = *(cp - *p++);
          x += 4 * c;
          c = *(cp - *p);
          x += c;
          p += 2;
          c = *(cp - *p++);
          x += c;
          c = *(cp - *p++);
          x += 4 * c;
          c = *(cp - *p);
          x += 9 * c;
          p += x_size - 6;

          c = *(cp - *p++);
          x += 9 * c;
          y += c;
          w -= 3 * c;
          c = *(cp - *p++);
          x += 4 * c;
          y += c;
          w -= 2 * c;
          c = *(cp - *p++);
          x += c;
          y += c;
          w -= c;
          c = *(cp - *p++);
          y += c;
          c = *(cp - *p++);
          x += c;
          y += c;
          w += c;
          c = *(cp - *p++);
          x += 4 * c;
          y += c;
          w += 2 * c;
          c = *(cp - *p);
          x += 9 * c;
          y += c;
          w += 3 * c;
          p += x_size - 5;

          c = *(cp - *p++);
          x += 4 * c;
          y += 4 * c;
          w -= 4 * c;
          c = *(cp - *p++);
          x += c;
          y += 4 * c;
          w -= 2 * c;
          c = *(cp - *p++);
          y += 4 * c;
          c = *(cp - *p++);
          x += c;
          y += 4 * c;
          w += 2 * c;
          c = *(cp - *p);
          x += 4 * c;
          y += 4 * c;
          w += 4 * c;
          p += x_size - 3;

          c = *(cp - *p++);
          x += c;
          y += 9 * c;
          w -= 3 * c;
          c = *(cp - *p++);
          y += 9 * c;
          c = *(cp - *p);
          x += c;
          y += 9 * c;
          w += 3 * c;

          if (y == 0)
            z = 1000000.0;
          else
            z = ((float) x) / ((float) y);

          if (z < 0.5) { /* vertical */
            a = 0;
            b = 1;
          }
          else {
            if (z > 2.0) { /* horizontal */
              a = 1;
              b = 0;
            }
            else { /* diagonal */
              if (w > 0) {
                a = -1;
                b = 1;
              }
              else {
                a = 1;
                b = 1;
              }
            }
          }
          if ((m > r[(i + a) * x_size + j + b]) && (m >= r[(i - a) * x_size + j
              - b]) && (m > r[(i + (2 * a)) * x_size + j + (2 * b)]) && (m
              >= r[(i - (2 * a)) * x_size + j - (2 * b)]))
            mid[i * x_size + j] = 2;
        }
      }
    }
}


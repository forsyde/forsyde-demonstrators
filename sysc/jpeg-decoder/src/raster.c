#include "stdio-wrapper.h"
#include <string.h>
//#include <stdlib.h>
#include "structures.h"

#define SYSACE_FILENAME "output.bmp"
#define MAX_IMAGE_SIZE 3*128*128

//#undef VERBOSE
#ifndef VERBOSE
//#define NO_WRITE
#endif

static unsigned char FrameBuffer[MAX_IMAGE_SIZE];
#ifndef NO_WRITE
static unsigned char BMP_Header[54] = {
  66, 77, 0, 0, 0, 0, 0, 0, 0, 0, 54, 0, 0, 0, 40, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 24, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0
};
#endif

void raster(const SubHeader2 * SH2, const ColorBuffer * CB)
{
  const int leftover = SH2->leftover;
  const int goodrows = SH2->goodrows;
  const int goodcolumns = SH2->goodcolumns;
  const int MCU_sx = SH2->MCU_sx;
  const int n_comp = SH2->n_comp;
  int i;
#ifndef NO_WRITE
  const int x_size = SH2->x_size;
  const int y_size = SH2->y_size;
  const int offset = SH2->offset;
  int j;
#endif

  for (i = 0; i < goodrows; i++)
#ifdef NO_WRITE
    memmove(FrameBuffer, CB->data + n_comp * i * MCU_sx, n_comp * goodcolumns);
#else
    memmove(FrameBuffer + offset + n_comp * i * x_size, CB->data + n_comp * i * MCU_sx, n_comp * goodcolumns);
#endif

  if (leftover == 0) {
#ifndef NO_WRITE
    SYSACE_FILE *fpBMP;

    // Header and 3 bytes per pixel
    unsigned long ulBitmapSize = (y_size * x_size * 3) + 54;

    /* Create bitmap file */
    fpBMP = sysace_fopen(SYSACE_FILENAME, "w");

    if (fpBMP == 0) {
      fprintf(stderr, "Error: Couldn't open file to write BMP file\n");
      exit(-1);
    }

    /* Write header */
    /* All values are in big endian order (LSB first) */
    BMP_Header[2] = (ulBitmapSize & 0x000000FF);
    BMP_Header[3] = (ulBitmapSize & 0x0000FF00) >> 8;
    BMP_Header[4] = (ulBitmapSize & 0x00FF0000) >> 16;
    BMP_Header[5] = (ulBitmapSize & 0xFF000000) >> 24;

    BMP_Header[18] = x_size & 0x00ff;
    BMP_Header[19] = (x_size & 0xff00) >> 8;

    BMP_Header[22] = y_size & 0x00ff;
    BMP_Header[23] = (y_size & 0xff00) >> 8;

    unsigned char cc = 0;

    for (cc = 0; cc < sizeof(BMP_Header); cc++) {
      sysace_putc(BMP_Header[cc], fpBMP);
    }

    //sysace_fwrite(BMP_Header, 1, sizeof(BMP_Header), fpBMP);

    /* Write data */
    for (i = y_size - 1; i >= 0; i--) {
      /* in bitmaps the bottom line of the image is at the
	 beginning of the file */
      for (j = 0; j < x_size; j++) {
	sysace_putc(FrameBuffer[3 * (i * x_size + j) + 0], fpBMP);
	sysace_putc(FrameBuffer[3 * (i * x_size + j) + 1], fpBMP);
	sysace_putc(FrameBuffer[3 * (i * x_size + j) + 2], fpBMP);
      }
      for (j = 0; j < x_size % 4; j++)
	sysace_putc(0, fpBMP);
    }

    sysace_fclose(fpBMP);
#endif
    //printf("DONE!\n");
    exit(0);
  }
}

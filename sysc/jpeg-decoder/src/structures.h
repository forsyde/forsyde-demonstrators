#ifndef _JPEG_STRUCTS
#define _JPEG_STRUCTS


#include "conf.h"

// C types
/* block of pixel-space values */
typedef union {
	unsigned char block[8][8];
	unsigned char linear[64];
} PBlock;

/* block of frequency-space values */
typedef union {
	int block[8][8];
	int linear[64];
} FBlock;

/* block of frequency-space values before IQZZ */
typedef struct {
	unsigned long linear[64];
	PBlock qtable;
} FValue;

/* pixel value array */
typedef struct {
	unsigned char data[3072];
} ColorBuffer;

/* component descriptor */
typedef struct {
	unsigned char CID, IDX;
	unsigned char HS, VS;
	unsigned char HDIV, VDIV;

	char QT, DC_HT, AC_HT;
	int PRED;
} cd_t;

/* settings for color conversion */
typedef struct {
	int MCU_sx, MCU_sy;
	int n_comp;
	cd_t comp[3];
} SubHeader1;

/* settings for rasterization */
typedef struct {
	int MCU_sx;
	int n_comp;
	int x_size, y_size;
	int goodrows, goodcolumns, offset;
	int leftover;
	int MCU_column;
} SubHeader2;

#endif

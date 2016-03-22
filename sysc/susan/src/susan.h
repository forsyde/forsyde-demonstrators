#ifndef SUSAN_H
#define SUSAN_H

#include "conf.h"
#include <string.h>
/* uncomment if using djgpp gnu C for DOS or certain Win95 compilers */
/*#define FOPENB*/

/* size for non-max corner suppression; SEVEN_SUPP or FIVE_SUPP */
#define SEVEN_SUPP

/* max corners per frame */
#define MAX_CORNERS   15000

#define  FTOI(a) ( (a) < 0 ? ((int)(a-0.5)) : ((int)(a+0.5)) )

#define BLOCK_SIZE 19
#define OUTER_PIXEL 4
#define OVERLAP_PIXEL 7

#define WIDTH 76
#define HEIGHT 95
#define BRIGHT_THRESHOLD 20
#define FORM_EDGE_CORNER 6
#define FORM_SMOOTH 2
#define MAX_NO_CORNER 1850
#define MAX_NO_EDGES 2650
#define DRAWING_MODE 0

#define INPUT_FILE "input_large.pgm"
#define OUTPUT_FILE "output_large.pgm"

#include <math.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned char uchar;
typedef struct {
    int x, y, info, dx, dy, I;
} CORNER_LIST[MAX_CORNERS];



/**
 * Data structure for the lookup table
 */
typedef struct BrightnessLUT {

    uchar bpBuffer[516];

} BrightnessLUT;

/**
 * Data structure for the MCU block
 */
typedef struct MCU_BLOCK {

    /*The width of the whole image*/
    int IMAGE_WIDTH;
    /*The height of the whole image*/
    int IMAGE_HEIGHT;

    /*The index of the row of the MCU block*/
    int ROW;
    /*The index of the column of the MCU block*/
    int COLUMN;

    /*The width of the central part of interest (without the overhead)*/
    int BL_WIDTH;
    /*The height of the central part of interest (without the overhead)*/
    int BL_HEIGHT;

    /*The dimension of overhead on all four sides*/
    int TOP;
    int LEFT;
    int BOTTOM;
    int RIGHT;

    /*Array holding the intensity of the blocks' pixels*/
    uchar IN[(BLOCK_SIZE + 2 * OVERLAP_PIXEL)
        * (BLOCK_SIZE + 2 * OVERLAP_PIXEL) + 3];

} MCU_BLOCK;

/**
 * Data structure for the data of the whole .pgm image
 */
typedef struct ImageInput {

    int xSize;
    int ySize;
    uchar imageBuffer[WIDTH*HEIGHT];

} ImageInput;

/**
 * Data structure for keeping the USAN value for every pixel of the MCU block
 */
typedef struct EdgeStrength {

    int rBuffer[(BLOCK_SIZE + 2 * OVERLAP_PIXEL) * (BLOCK_SIZE + 2
        * OVERLAP_PIXEL)];

} EdgeStrength;

/**
 * Data structure for keeping the strength of the 'edginess'
 * of every pixel within MCU block
 */
typedef struct EdgeDirection {

    uchar midBuffer[(BLOCK_SIZE + 2 * OVERLAP_PIXEL) * (BLOCK_SIZE + 2
        * OVERLAP_PIXEL) + 3];

} EdgeDirection;

/**
 *
 * Declarations of the specific methods used within the KPN
 */

void splitIntoMCUs(MCU_BLOCK* block);
void takeMCU(MCU_BLOCK *block, int f_i, int f_j, int n_columns, int n_rows,
    int remainder_last_row, int remainder_last_column, int x_size, int row,
    int column, uchar* tmp);

void susanUsan(const MCU_BLOCK *imgInput, MCU_BLOCK *imgOutput, EdgeStrength *strength);

void susanDirection(const MCU_BLOCK *imgInput, const EdgeStrength *strength,
       MCU_BLOCK *imgOutput, EdgeStrength *strengthOutput, EdgeDirection *direction);

void susanThin(const MCU_BLOCK *imgInput, const EdgeStrength *edgeSt, const EdgeDirection *edgeDirInput,
               MCU_BLOCK *imgOutput, EdgeDirection *edgeDir);

void wrapUp(const MCU_BLOCK* imgOutput, const EdgeDirection *edgeDir);
void edgeDraw(uchar* outputMidBuffer, uchar* outputImageBuffer,
    const int x_size, const int y_size);
void stitch(MCU_BLOCK* imgOutput, uchar* outputImageBuffer,
    uchar* outputMidBuffer, const EdgeDirection *edgeDir);
void writeToFile(char* fileName, const MCU_BLOCK* imgAfterThin, uchar* outputImageBuffer);




void splitIntoMCUs_wrap(int task_id, void *** input, void ***output, int csdf_cycle);
void susanUsan_wrap(int task_id, void *** input, void ***output, int csdf_cycle);
void susanDirection_wrap(int task_id, void *** input, void ***output, int csdf_cycle);
void susanThin_wrap(int task_id, void *** input, void ***output, int csdf_cycle);
void wrapUp_wrap(int task_id, void *** input, void ***output, int csdf_cycle);

#ifdef __cplusplus
}
#endif

#endif

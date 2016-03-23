#include <stdlib.h>
#include <string.h>
#include "structures.h"
#include "huffman.h"
#include "utilities.h"

// #if microblaze
// # include <xparameters.h>
// # include <xtmrctr_l.h>
// #endif

#define FILE_NAME "surfer.jpg"

static SYSACE_FILE *fp;				// input file handle
static int MCU_sx, MCU_sy;		// MCU size in pixels
static int n_comp;				// number of components 1,3 
static cd_t comp[3];			//descriptors for 3 components
static int x_size, y_size;		// Video frame size
static int goodrows, goodcolumns, offset;	// final rasterization information
static int leftover;			// number of MCUs left for processing
static int MCU_row, MCU_column;	// current position in MCU unit      
static int MCU_valid[10];		// components of above MCU blocks 
static PBlock QTable[4];		// quantization tables 
static int QTvalid[4];
static int rx_size, ry_size;	// down-rounded Video frame size (integer MCU) 
static int mx_size, my_size;	// picture size in units of MCUs 
static int in_frame, curcomp;	// frame started ? current component ? 
static int n_restarts, restart_interval, mcu_rst;	// RST check 

static int init_MCU(void);
static int load_quant_tables(SYSACE_FILE * fp);
static void skip_segment(SYSACE_FILE * fi);
static int load_MCU(SYSACE_FILE * fp, FValue mcu_after_vld[10]);
static void copy_subheaders(SubHeader1 * sh1, SubHeader2 * sh2);

void init_header_vld(FValue * mcu_after_vld, SubHeader1 * sh1, SubHeader2 * sh2)
{

	unsigned int aux, mark;
	int i;

	init_huffman();

	fp = sysace_fopen(FILE_NAME, "r");
	if (fp == NULL) {
		//fputs("File error", stderr);
		exit(1);
	}

	n_restarts = 0;
	restart_interval = 0;
	mcu_rst = -1;

	/* First find the SOI marker: */
	aux = get_next_MK(fp);
	if (aux != SOI_MK)
		aborted_stream(fp);

#ifdef VERBOSE
	fprintf(stderr, "%ld:\tINFO:\tFound the SOI marker!\n", sysace_ftell(fp));
#endif

	in_frame = 0;
	for (i = 0; i < 4; i++)
		QTvalid[i] = 0;

	restart_interval = 0;

	/* Now process segments as they appear: */
	do {
		mark = get_next_MK(fp);

		switch (mark) {
		case SOF_MK:
#ifdef VERBOSE
			fprintf(stderr, "%ld:\tINFO:\tFound the SOF marker!\n", sysace_ftell(fp));
#endif
			in_frame = 1;
			get_size(fp);		/* header size, don't care */

			/* load basic image parameters */
			sysace_fgetc(fp);			/* precision, 8bit, don't care */
			y_size = get_size(fp);
			x_size = get_size(fp);
#ifdef VERBOSE
			fprintf(stderr, "\tINFO:\tImage size is %d by %d\n", x_size, y_size);
#endif

			n_comp = sysace_fgetc(fp);
#ifdef VERBOSE
			fprintf(stderr, "\tINFO:\t");
			switch (n_comp) {
			case 1:
				fprintf(stderr, "Monochrome");
				break;
			case 3:
				fprintf(stderr, "Color");
				break;
			default:
				fprintf(stderr, "Not a");
				break;
			}
			fprintf(stderr, " JPEG image!\n");
#endif

			for (i = 0; i < n_comp; i++) {
				/* component specifiers */
				comp[i].CID = sysace_fgetc(fp);
				aux = sysace_fgetc(fp);
				comp[i].HS = first_quad(aux);
				comp[i].VS = second_quad(aux);
				comp[i].QT = sysace_fgetc(fp);
			}

#ifdef VERBOSE
			if ((n_comp > 1))
				fprintf(stderr, "\tINFO:\tColor format is %d:%d:%d, H=%d\n", comp[0].HS * comp[0].VS, comp[1].HS * comp[1].VS, comp[2].HS * comp[2].VS, comp[1].HS);
#endif

			if (init_MCU() == -1)
				aborted_stream(fp);

			break;

		case DHT_MK:
#ifdef VERBOSE
			fprintf(stderr, "%ld:\tINFO:\tDefining Huffman Tables\n", sysace_ftell(fp));
#endif
			if (load_huff_tables(fp) == -1)
				aborted_stream(fp);
			break;

		case DQT_MK:
#ifdef VERBOSE
			fprintf(stderr, "%ld:\tINFO:\tDefining Quantization Tables\n", sysace_ftell(fp));
#endif
			if (load_quant_tables(fp) == -1)
				aborted_stream(fp);
			break;
		case COM_MK:
#ifdef VERBOSE
			fprintf(stderr, "%ld:\tINFO:\tSkipping comments\n", sysace_ftell(fp));
#endif
			skip_segment(fp);
			break;
		case DRI_MK:
			get_size(fp);		// skip size
			restart_interval = get_size(fp);
#ifdef VERBOSE
			fprintf(stderr, "%ld:\tINFO:\tDefining Restart Interval %d\n", sysace_ftell(fp), restart_interval);
#endif
			break;

		case SOS_MK:
#ifdef VERBOSE
			fprintf(stderr, "%ld:\tINFO:\tFound the SOS marker!\n", sysace_ftell(fp));
#endif
			get_size(fp);
			aux = sysace_fgetc(fp);
			if (aux != (unsigned int) n_comp) {
				fprintf(stderr, "\tERROR:\tBad component interleaving!\n");
				aborted_stream(fp);
			}

			for (i = 0; i < n_comp; i++) {
				aux = sysace_fgetc(fp);
				if (aux != comp[i].CID) {
					fprintf(stderr, "\tERROR:\tBad Component Order!\n");
					aborted_stream(fp);
				}
				aux = sysace_fgetc(fp);
				comp[i].DC_HT = first_quad(aux);
				comp[i].AC_HT = second_quad(aux);
			}
			sysace_fgetc(fp);
			sysace_fgetc(fp);
			sysace_fgetc(fp);

			MCU_column = 0;
			MCU_row = 0;
			clear_bits();
			reset_prediction(comp);

			if (restart_interval) {
				n_restarts = ceil_div(mx_size * my_size, restart_interval) - 1;
				leftover = mx_size * my_size - n_restarts * restart_interval;
				mcu_rst = n_restarts * restart_interval;
			} else {
				leftover = mx_size * my_size;
			}

			return;
			break;

		case EOI_MK:
#ifdef VERBOSE
			fprintf(stderr, "%ld:\tINFO:\tFound the EOI marker before any MCU is obtained!\n", sysace_ftell(fp));
#endif
			aborted_stream(fp);
			break;

		case EOF:
#ifdef VERBOSE
			fprintf(stderr, "%ld:\tERROR:\tRan out of input data!\n", sysace_ftell(fp));
#endif
			aborted_stream(fp);
			break;

		default:
			if ((mark & MK_MSK) == APP_MK) {
#ifdef VERBOSE
				fprintf(stderr, "%ld:\tINFO:\tSkipping application data\n", sysace_ftell(fp));
#endif
				skip_segment(fp);
				break;
			}
			if (RST_MK(mark)) {
				reset_prediction(comp);
				break;
			}
			/* if all else has failed ... */
			fprintf(stderr, "%ld:\tWARNING:\tLost Sync outside scan, %d!\n", sysace_ftell(fp), mark);
			aborted_stream(fp);
			break;
		}						// end switch 
	} while (1);
}

void header_vld(FValue * mcu_after_vld, SubHeader1 * subheader1, SubHeader2 * subheader2)
{
	unsigned int aux, mark;
	int end = 0;

	if (mcu_rst > 0) {
		load_MCU(fp, mcu_after_vld);

		mcu_rst--;
		if (mcu_rst % restart_interval == 0) {
			aux = get_next_MK(fp);
			if (!RST_MK(aux)) {
				fprintf(stderr, "%ld:\tERROR:\tLost Sync after interval!\n", sysace_ftell(fp));
				aborted_stream(fp);
			}
#ifdef VERBOSE
			fprintf(stderr, "%ld:\tINFO:\tFound Restart Marker\n", sysace_ftell(fp));
#endif
			reset_prediction(comp);
			clear_bits();
		}
		copy_subheaders(subheader1, subheader2);
		return;
	} else if (leftover > 0) {	// still more MCUs to process
		load_MCU(fp, mcu_after_vld);
		leftover--;
		copy_subheaders(subheader1, subheader2);
		return;
	} else if (leftover == 0) {	//no more MCU to process
		/* Now process segments as they appear: */
		in_frame = 0;			//finished MCUs
		do {
			mark = get_next_MK(fp);

			switch (mark) {

			case COM_MK:
#ifdef VERBOSE
				fprintf(stderr, "%ld:\tINFO:\tSkipping comments\n", sysace_ftell(fp));
#endif
				skip_segment(fp);
				break;

			case EOI_MK:
#ifdef VERBOSE
				fprintf(stderr, "%ld:\tINFO:\tFound the EOI marker!\n", sysace_ftell(fp));
#endif

				end = 1;
				leftover = -1;
				break;

			case EOF:
#ifdef VERBOSE
				fprintf(stderr, "%ld:\tERROR:\tRan out of input data!\n", sysace_ftell(fp));
#endif
				aborted_stream(fp);

			default:
				if ((mark & MK_MSK) == APP_MK) {
#ifdef VERBOSE
					fprintf(stderr, "%ld:\tINFO:\tSkipping application data\n", sysace_ftell(fp));
#endif
					skip_segment(fp);
					break;
				}
				if (RST_MK(mark)) {
					reset_prediction(comp);
					break;
				}
				/* if all else has failed ... */
				fprintf(stderr, "%ld:\tWARNING:\tLost Sync outside scan, %d!\n", sysace_ftell(fp), mark);
				aborted_stream(fp);
				break;
			}					/* end switch */
		} while (!end);
	}
}

/*
 * initialize locations
 */
int init_MCU(void)
{
	int i, j, k, n, hmax = 0, vmax = 0;

	for (i = 0; i < 10; i++)
		MCU_valid[i] = -1;

	k = 0;

	for (i = 0; i < n_comp; i++) {
		if (comp[i].HS > hmax)
			hmax = comp[i].HS;
		if (comp[i].VS > vmax)
			vmax = comp[i].VS;
		n = comp[i].HS * comp[i].VS;

		comp[i].IDX = k;
		for (j = 0; j < n; j++) {
			MCU_valid[k] = i;
			k++;
			if (k == 10) {
				fprintf(stderr, "\tERROR:\tMax subsampling exceeded!\n");
				return -1;
			}
		}
	}

	MCU_sx = 8 * hmax;
	MCU_sy = 8 * vmax;
	for (i = 0; i < n_comp; i++) {
		comp[i].HDIV = (hmax / comp[i].HS > 1);	/* if 1 shift by 0 */
		comp[i].VDIV = (vmax / comp[i].VS > 1);	/* if 2 shift by one */
	}

	mx_size = ceil_div(x_size, MCU_sx);
	my_size = ceil_div(y_size, MCU_sy);
	rx_size = MCU_sx * floor_div(x_size, MCU_sx);
	ry_size = MCU_sy * floor_div(y_size, MCU_sy);

	return 0;
}


/*----------------------------------------------------------*/
/* loading and allocating of quantization table             */
/* table elements are in ZZ order (same as unpack output)   */
/*----------------------------------------------------------*/
int load_quant_tables(SYSACE_FILE * fi)
{
	char aux;
	unsigned int size, n, i, id, x;

	size = get_size(fi);		/* this is the table's size */
	n = (size - 2) / 65;

	for (i = 0; i < n; i++) {
		aux = sysace_fgetc(fi);
		if (first_quad(aux) > 0) {
			fprintf(stderr, "\tERROR:\tBad QTable precision!\n");
			return -1;
		}
		id = second_quad(aux);
#ifdef VERBOSE
		fprintf(stderr, "\tINFO:\tLoading table %d\n", id);
#endif
		QTvalid[id] = 1;
		for (x = 0; x < 64; x++)
			QTable[id].linear[x] = sysace_fgetc(fi);
		/*
		   -- This is useful to print out the table content --
		   for (x = 0; x < 64; x++)
		   fprintf(stderr, "%d\n", QTable[id]->linear[x]);
		 */
	}
	return 0;
}

void skip_segment(SYSACE_FILE * fi)
{								/* skip a segment we don't want */
	unsigned int size;
	char tag[5];
	int i;

	size = get_size(fi);
	if (size > 5) {
		for (i = 0; i < 4; i++)
			tag[i] = sysace_fgetc(fi);
		tag[4] = '\0';
#ifdef VERBOSE
		fprintf(stderr, "\tINFO:\tTag is %s\n", tag);
#endif
		size -= 4;
	}
	sysace_fseek(fi, size - 2, SEEK_CUR);
}

int load_MCU(SYSACE_FILE * fp, FValue * mcu_after_vld)
{
	if (MCU_column == mx_size) {
		MCU_column = 0;
		MCU_row++;
		if (MCU_row == my_size) {
			in_frame = 0;
			return 0;
		}
#ifdef VERBOSE
		fprintf(stderr, "%ld:\tINFO:\tProcessing stripe %d/%d\n", sysace_ftell(fp), MCU_row + 1, my_size);
#endif
	}

	for (curcomp = 0; MCU_valid[curcomp] != -1; curcomp++) {
		unsigned int i, run, cat;
		int value;
		unsigned char symbol;
		FValue *fvalue = &mcu_after_vld[curcomp];
		int select = MCU_valid[curcomp];

		/* Init the block with 0's: */
		for (i = 0; i < 64; i++)
			fvalue->linear[i] = 0;

		/* First get the DC coefficient: */
		symbol = get_symbol(fp, HUFF_ID(DC_CLASS, comp[select].DC_HT));
		if(symbol == 0) {
		   	value = 0;
		} else
			value = reformat(get_bits(fp, symbol), symbol);

		value += comp[select].PRED;
		comp[select].PRED = value;
		fvalue->linear[0] = value;

		/* Now get all 63 AC values: */
		for (i = 1; i < 64; i++) {
			symbol = get_symbol(fp, HUFF_ID(AC_CLASS, comp[select].AC_HT));
			if (symbol == HUFF_EOB)
				break;
			if (symbol == HUFF_ZRL) {
				i += 15;
				continue;
			}
			cat = symbol & 0x0F;
			run = (symbol >> 4) & 0x0F;
			i += run;
			value = reformat(get_bits(fp, cat), cat);

			fvalue->linear[i] = value;
		}
		memcpy(&fvalue->qtable, &QTable[(int) comp[select].QT], sizeof(PBlock));
	}

	/* cut last row/column as needed */
	if ((y_size != ry_size) && (MCU_row == (my_size - 1)))
		goodrows = y_size - ry_size;
	else
		goodrows = MCU_sy;

	if ((x_size != rx_size) && (MCU_column == (mx_size - 1)))
		goodcolumns = x_size - rx_size;
	else
		goodcolumns = MCU_sx;

	offset = n_comp * (MCU_row * MCU_sy * x_size + MCU_column * MCU_sx);
	MCU_column++;

	return 1;
}

void copy_subheaders(SubHeader1 * subheader1, SubHeader2 * subheader2)
{
	subheader1->MCU_sx = MCU_sx;
	subheader1->MCU_sy = MCU_sy;
	subheader1->n_comp = n_comp;
	memcpy(&subheader1->comp, &comp, 3 * sizeof(cd_t));

	subheader2->leftover = leftover;
	subheader2->goodrows = goodrows;
	subheader2->goodcolumns = goodcolumns;
	subheader2->x_size = x_size;
	subheader2->y_size = y_size;
	subheader2->MCU_sx = MCU_sx;
	subheader2->offset = offset;
	subheader2->n_comp = n_comp;
}

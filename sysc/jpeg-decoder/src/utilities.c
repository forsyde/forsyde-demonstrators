#include "stdio-wrapper.h"
//#include <stdlib.h>
#include "structures.h"
#include "utilities.h"

/*----------------------------------------------------------------*/
/* find next marker of any type, returns it, positions just after */
/* EOF instead of marker if end of file met while searching ...	  */
/*----------------------------------------------------------------*/

static int passed = 0;

unsigned int get_next_MK(SYSACE_FILE * fp)
{
	unsigned int c;
	int ffmet = 0;
	int locpassed = -1;

	passed--;					/* as we fetch one anyway */

	while ((c = sysace_fgetc(fp)) != (unsigned int) EOF) {
		switch (c) {
		case 0xFF:
			ffmet = 1;
			break;
		case 0x00:
			ffmet = 0;
			break;
		default:
			if (locpassed > 1)
				fprintf(stderr, "NOTE: passed %d bytes\n", locpassed);
			if (ffmet) {
				return (0xFF00 | c);
			}
			ffmet = 0;
			break;
		}
		locpassed++;
		passed++;
	}

	return (unsigned int) EOF;
}


unsigned int get_size(SYSACE_FILE * fp)
{
	unsigned char aux;

	aux = sysace_fgetc(fp);
	return (aux << 8) | sysace_fgetc(fp);	/* big endian */
}

void aborted_stream(SYSACE_FILE * fp)
{
	fprintf(stderr, "%ld:\tERROR:\tAbnormal end of decompression process!\n", sysace_ftell(fp));
	fprintf(stderr, "\tINFO:\tTotal skipped bytes %d\n", passed);

//#ifdef DEBUG
//	exit(-1);
//#endif
}

/* Returns ceil(N/D). */
int ceil_div(int N, int D)
{
	int i = N / D;

	if (N > D * i)
		i++;
	return i;
}


/* Returns floor(N/D). */
int floor_div(int N, int D)
{
	int i = N / D;

	if (N < D * i)
		i--;
	return i;
}

void reset_prediction(cd_t * comp)
{
	int i;

	for (i = 0; i < 3; i++)
		comp[i].PRED = 0;
}

int reformat(unsigned long S, int good)
{
	int St;

	if (!good)
		return 0;
	St = 1 << (good - 1);		/* 2^(good-1) */
	if (S < (unsigned long) St)
		return (S + 1 + ((-1) << good));
	else
		return S;
}

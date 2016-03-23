#ifndef STDIO_WRAPPER_INCLUDED
#define STDIO_WRAPPER_INCLUDED

#include "conf.h"

//#define VERBOSE


#include <stdio.h>
#include <stdlib.h>
# define SYSACE_FILE   FILE
# define sysace_fopen  fopen
# define sysace_fclose fclose
# define sysace_fread  fread
# define sysace_fwrite fwrite
# define sysace_ftell  ftell
# define sysace_fseek  fseek
# define sysace_fgetc  fgetc
# define sysace_putc   putc


#endif /* STDIO_WRAPPER_INCLUDED */


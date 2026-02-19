#ifndef AHTHEX_H
#define AHTHEX_H


#include <errno.h>
#include <conio.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include <io.h>

#include "ahtdefs.h"


#if defined(OS_DOS)
	#include <dos.h>
#elif defined(OS_WIN)
	#include <windows.h>
#elif defined(OS_UNIX)
	#include <termio.h>
	#include <unistd.h>
#endif

extern uint16_t MAXCOLUMN;
extern uint16_t MAXROW;

#define DEBUG_MODE

#define OPTIONS_FILE_NAME "OPTIONS.AHT"


#define DEFAULT_ALIGN -1 /* used in TUI_CreateMenu() function */

#define MAX_NUMBER_OF_FILE 32

#define HEX 0
#define DEC 1
#define OCT 2
#define ASCII 3


#if defined(OS_WIN)

	#define MAXPATH MAX_PATH
	
	#define farmalloc malloc
	#define farcalloc calloc
	#define farrealloc realloc
	#define farfree free
#endif

typedef struct tagHEXFILE
{
	long col, row, index;
	long size;
	uint8_t *buff;
	char *filename;
}HEXFILE;

/* DOSPROC.C / WINPROC.C */
typedef struct _INPUTKEY
{
	uint16_t bAsciiCode;
	uint16_t bScanCode;
	uint8_t bShiftState;
	uint8_t bCtrlState;
	uint8_t bAltState;
}INPUTKEY;

bool is_ansi_supported(void);

#if 0
/* FILES.C */
long get_file_size(FILE *f); /* error: -1 */
uint8_t* read_file(HEXFILE *h);
bool write_file(HEXFILE *h);
bool save_as(HEXFILE *h);
char close_file(Node *n);
bool open_file(Node *n);
bool create_new_file(Node *n);


/* OPTIONS.C */
extern uint16_t opt_offset_base;
extern uint16_t opt_row_number;
bool get_options(void);
bool set_options(void);
bool options(void);

extern Node *n;
extern HEXFILE *cf;
extern size_t file_count; /* cf: current_file */

uint8_t* delete_byte();

#endif

#endif /* AHTHEX_H */

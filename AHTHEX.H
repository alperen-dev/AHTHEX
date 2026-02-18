#ifndef __AHTHEX_H__
#define __AHTHEX_H__

#include "AHTDEFS.H"
#include "LNKLST.H"
#include <errno.h>
#include <conio.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include <io.h>

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

#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define MIN(a, b) (((a) < (b)) ? (a) : (b))

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

/* gfx_card_installition_check */

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
void SetVideoMode(uint8_t VideoMode);
uint8_t GetVideoMode(void);
void PutChar(uint8_t ch, uint32_t x, uint32_t y);
void ChangeCharAttrib(uint8_t Attrib, uint32_t x, uint32_t y);
void PutStr(uint8_t * lpszStr, uint32_t x, uint32_t y);
void SetConCursorPos(uint8_t x, uint8_t y);
void GetConCursorPos(uint8_t *x, uint8_t *y);
void ClearScreen(void);
void ClearRow(int Row);

bool SetScreenResolution(uint16_t Row);

long _farstrlen(uint8_t * str);
uint8_t *_farmemsearch(uint8_t * s1, uint8_t * s2, long s1_length, long s2_length);

uint8_t hexchar_to_byte(uint8_t byte[2]); /* example:  "F2" -> 0xF2 */

void TUI_Rectangle(uint8_t * lpTitle, int x, int y, int w, int h, bool bIsDoubly);
int TUI_CreateMenu(uint8_t * *lpTexts, uint8_t * lpTitle, int ActiveMenu, long StartX, long StartY, bool bIsDoubly);

bool CheckShiftState();
bool CheckCtrlState();
bool CheckAltState();
INPUTKEY *GetInput(void);

#define Debug(errno, message) DebugPrint(errno, __FILE__, __LINE__, message)

bool DebugInit(void);
bool DebugClose(void);
bool DebugPrint(int Errno, char *lpFileName, int Line, char *lpMessage);
bool debug(char *pStr, ...);


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

#endif /* __AHTHEX_H__ */

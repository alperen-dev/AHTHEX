#ifndef _AHTHEX_H_
#define _AHTHEX_H_

#include <errno.h>
#include <conio.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include <io.h>
#include "AHTDEFS.H"
#include "LNKLST.H"

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

#define DEFAULT_ALIGN -1 // used in TUI_CreateMenu() function

#define MAX_NUMBER_OF_FILE 32

#define HEX 0
#define DEC 1
#define OCT 2
#define ASCII 3



/* KB */
#ifdef __MSDOS__
	#define KB_ESC 27
	#define KB_CONTROL 0
	#define KB_UP 72
	#define KB_DOWN 80
	#define KB_LEFT 75
	#define KB_RIGHT 77
	#define KB_RETURN 13
	#define KB_CTRL_RETURN 10
	#define KB_PG_UP 73
	#define KB_PG_DOWN 81
	#define KB_HOME 71
	#define KB_END 79
	#define KB_CTRL_F 6
	#define KB_CTRL_RIGHT 116
	#define KB_CTRL_LEFT 115
	#define KB_CTRL_S 19
	#define KB_ALT_S 31
	#define KB_CTRL_X 24
	#define KB_CTRL_N 14
	#define KB_CTRL_O 15
	#define KB_ALT_O 24
	#define KB_BACKSPACE 8
	#define KB_DEL 83 /* control */
#elif defined __WINDOWS__ || defined __WIN32__ || defined __WIN64__ || defined WIN32 || defined WIN64
	#define KB_ESC 27
	#define KB_CONTROL 224
	#define KB_UP 72
	#define KB_DOWN 80
	#define KB_LEFT 75
	#define KB_RIGHT 77
	#define KB_RETURN 13
	#define KB_CTRL_RETURN 10
	#define KB_PG_UP 73
	#define KB_PG_DOWN 81
	#define KB_HOME 71
	#define KB_END 79
	#define KB_CTRL_F 6
	#define KB_CTRL_RIGHT 116
	#define KB_CTRL_LEFT 115
	#define KB_CTRL_S 19
	#define KB_ALT_S 115
	#define KB_CTRL_X 24
	#define KB_CTRL_N 14
	#define KB_CTRL_O 15
	#define KB_ALT_O 111
	#define KB_BACKSPACE 8
	#define KB_DEL 83 // control
#endif

#if defined __WINDOWS__ || defined __WIN32__ || defined __WIN64__ || defined WIN32 || defined WIN64

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
	LPBYTE buff;
	char *filename;
}HEXFILE;

// gfx_card_installition_check

// DOSPROC.C / WINPROC.C
typedef struct _INPUTKEY
{
	WORD bAsciiCode;
	WORD bScanCode;
	BYTE bShiftState;
	BYTE bCtrlState;
	BYTE bAltState;
}INPUTKEY;

VOID SetVideoMode(BYTE VideoMode);
BYTE GetVideoMode(VOID);
VOID PutChar(BYTE ch, DWORD x, DWORD y);
VOID ChangeCharAttrib(BYTE Attrib, DWORD x, DWORD y);
VOID PutStr(LPSTR lpszStr, DWORD x, DWORD y);
VOID SetConCursorPos(BYTE x, BYTE y);
VOID GetConCursorPos(BYTE *x, BYTE *y);
VOID ClearScreen(VOID);
VOID ClearRow(INT Row);

BOOL SetScreenResolution(WORD Row);

LONG _farstrlen(LPSTR str);
LPSTR _farmemsearch(LPSTR s1, LPSTR s2, LONG s1_length, LONG s2_length);

BYTE hexchar_to_byte(BYTE byte[2]); // example:  "F2" -> 0xF2

VOID TUI_Rectangle(LPSTR lpTitle, INT x, INT y, INT w, INT h, BOOL bIsDoubly);
INT TUI_CreateMenu(LPSTR *lpTexts, LPSTR lpTitle, INT ActiveMenu, LONG StartX, LONG StartY, BOOL bIsDoubly);

bool CheckShiftState();
bool CheckCtrlState();
bool CheckAltState();
INPUTKEY *GetInput(VOID);

#define Debug(errno, message) DebugPrint(errno, __FILE__, __LINE__, message)

BOOL DebugInit(VOID);
BOOL DebugClose(VOID);
BOOL DebugPrint(INT Errno, LPSTR lpFileName, INT Line, LPSTR lpMessage);
BOOL debug(CHAR *pStr, ...);


// FILES.C
long get_file_size(FILE *f); // error: -1
LPBYTE read_file(HEXFILE *h);
BOOL write_file(HEXFILE *h);
BOOL save_as(HEXFILE *h);
char close_file(Node *n);
BOOL open_file(Node *n);
BOOL create_new_file(Node *n);


//OPTIONS.C
extern WORD opt_offset_base;
extern WORD opt_row_number;
BOOL get_options(VOID);
BOOL set_options(VOID);
BOOL options(VOID);

extern Node *n;
extern HEXFILE *cf;
extern size_t file_count; // cf: current_file

LPBYTE delete_byte();

#endif /* _AHTHEX_H_ */

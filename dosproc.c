#include "ahtdefs.h"
#include <stdio.h>
#include <dos.h>
#include <i86.h>
#include <assert.h>

#define peekb(s, o) (*((uint8_t far*)(((uint32_t)(s) << 16LU) + (o))))
#define peek(s, o) (*((uint16_t far*)(((uint32_t)(s) << 16LU) + (o))))

#define GET_SCREEN_WIDTH_FAST() (peekb(0x0040, 0x004A))
#define GET_SCREEN_HEIGHT_FAST() (peekb(0x0040, 0x0084)+1)
#define GET_VIDEO_MODE_FAST() (peekb(0x0040, 0x0049))

/* ANSI.SYS or something similar function tool check */
#define ANSI_DSR_MAX_ATTEMPT 30000

typedef enum
{
	ANSI_SUPPORT_UNKNOWN,
	ANSI_SUPPORT_YES,
	ANSI_SUPPORT_NO
}AnsiSupport;
AnsiSupport ansiSupport = ANSI_SUPPORT_UNKNOWN;

typedef struct
{
	uint8_t row;
	uint8_t col;
}COORD;

uint8_t get_active_page(void);
uint8_t get_video_mode(void);
void set_cursor_pos(uint8_t col, uint8_t row);
COORD get_cursor_pos(void);
uint8_t get_cursor_x(void);
uint8_t get_cursor_y(void);
static void vram_cread(uint8_t *buffer, size_t length); /* read characters from current cursor position */
static void vram_cwrite(uint8_t *buffer, size_t size);

static bool check_ansi_interrupt(void)
{
	union REGS regs;
	regs.x.ax = 0x1A00;
	int86(0x2F, &regs, &regs);
	if(regs.h.al == 0xFF)
		return true;
	return false;
}

static bool check_ansi_vram(void)
{
	uint8_t orjBuffer[3];
	bool has_ansi = false;
	COORD orjPos;
	uint8_t videoMode = get_video_mode();
	
	if(videoMode > 0x03 && videoMode != 0x07) /* if not in text mode, don't try */
	{
		return false;
	}
	
	orjPos = get_cursor_pos();
	set_cursor_pos(0, 0);
	
	vram_cread(orjBuffer, 3);
	
	printf("\x1B[s");
	fflush(stdout);
	
	/* if cursor moved, that mean ANSI.SYS or similar tool is not installed */
	if(get_cursor_x() > 0)
	{
		set_cursor_pos(0, 0);
		vram_cwrite(orjBuffer, 3);
	}
	else
	{
		has_ansi = true;
	}
	
	set_cursor_pos(orjPos.col, orjPos.row);
	return has_ansi;
}

bool is_ansi_supported(void)
{
	if(ansiSupport != ANSI_SUPPORT_UNKNOWN) /* already checked */
		return ansiSupport;
	
	if(check_ansi_interrupt() == true)
	{
		printf("[+] ANSI support detected using interrupt\n");
		ansiSupport = ANSI_SUPPORT_YES;
		return true;
	}
	else if(check_ansi_vram() == true) /* if not found on interrupt, check its behaviour */
	{
		printf("[+] ANSI support detected using vram\n");
		ansiSupport = ANSI_SUPPORT_YES;
		return true;
	}
	else
	{
		ansiSupport = ANSI_SUPPORT_NO;
	}
	return false; /* ANSI driver not found */
}

uint8_t get_screen_width(void)
{
	union REGS regs;
	regs.h.ah = 0x0F;
	int86(0x10, &regs, &regs);
	return regs.h.ah;
}

uint8_t get_screen_height(void)
{
	union REGS regs;
	regs.x.ax = 0x1130;
	int86(0x10, &regs, &regs);
	return (regs.h.dl != 0) ? regs.h.dl+1 : 25; /* old BIOSes can return 0, so we are assume there are 25 row */
}

uint8_t get_video_mode(void)
{
	union REGS regs;
	regs.h.ah = 0x0F;
	int86(0x10, &regs, &regs);
	return regs.h.al;
}

uint8_t get_active_page(void)
{
	union REGS regs;
	regs.h.ah = 0x0F;
	int86(0x10, &regs, &regs);
	return regs.h.bh;
}

void set_cursor_pos(uint8_t col, uint8_t row)
{
	union REGS regs = {0};
	regs.h.ah = 0x02;
	regs.h.bh = get_active_page();
	regs.h.dh = row;
	regs.h.dl = col;
	int86(0x10, &regs, &regs);
}

COORD get_cursor_pos(void)
{
	union REGS regs = {0};
	COORD coord;
	regs.h.ah = 0x03;
	regs.h.bh = get_active_page();
	int86(0x10, &regs, &regs);
	coord.row = regs.h.dh;
	coord.col = regs.h.dl;
	return coord;
}

uint8_t get_cursor_x(void)
{
	union REGS regs = {0};
	regs.h.ah = 0x03;
	regs.h.bh = get_active_page();
	int86(0x10, &regs, &regs);
	return regs.h.dl;
}

uint8_t get_cursor_y(void)
{
	union REGS regs = {0};
	regs.h.ah = 0x03;
	regs.h.bh = get_active_page();
	int86(0x10, &regs, &regs);
	return regs.h.dh;
}

static uint8_t far * VIDEO_MEMORY = (uint8_t far*)0xB8000000LU;

/* return true on success */
static void vram_cread(uint8_t *buffer, size_t size)
{
	COORD coord = get_cursor_pos();
	uint8_t far *vidmem = (uint8_t far*)VIDEO_MEMORY + (coord.col + coord.row * GET_SCREEN_WIDTH_FAST());
	size_t i = 0;
	
	assert(buffer);
	
	for(i = 0; i < size; i++)
	{
		buffer[i] = vidmem[i*2];
	}
}

static void vram_cwrite(uint8_t *buffer, size_t size)
{
	COORD coord = get_cursor_pos();
	uint8_t far *vidmem = (uint8_t far*)VIDEO_MEMORY + (coord.col + coord.row * GET_SCREEN_WIDTH_FAST());
	size_t i = 0;
	
	assert(buffer);
	
	for(i = 0; i < size; i++)
	{
		vidmem[i*2] = buffer[i];
	}
}

#if 0
/* DSR -> device status report */
static bool check_ansi_dsr(void)
{
	uint32_t attempt = 0;
	
	printf("\x1B[6n"); /* DSR request */
	
	while(attempt < ANSI_DSR_MAX_ATTEMPT)
	{
		if(kbhit())
		{
			if(getch() == KB_ESC) /* answer format: ESC[r;c R */
			{
				while(kbhit())
				{
					getch();
				}
				return true;
			}
		}
		attempt++;
	}
	printf("\b\b\b\b    \b\b\b\b"); /* clear consol buffer if ANSI driver is not found, its already clean if found */
	return false;
}
#endif

#if 0
void SetVideoMode(uint8_t VideoMode)
{
	union REGS regs = {0};
	regs.h.al = VideoMode;
	int86(0x10, &regs, &regs);
}

uint8_t GetVideoMode(void)
{
	union REGS regs = {0};
	regs.h.ah = 0x0F;
	int86(0x10, &regs, &regs);
	return regs.h.al;
}

void PutChar(uint8_t ch, uint32_t x, uint32_t y)
{
	*((uint8_t*)VIDEO_MEMORY + (x + y * peek(0x0040, 0x004A)) * 2LU) = ch;
}

void ChangeCharAttrib(uint8_t Attrib, uint32_t x, uint32_t y)
{
	*((uint8_t*)VIDEO_MEMORY + (x + y * peek(0x0040, 0x004A)) * 2LU + 1LU) = Attrib;
}

void PutStr(uint8_t * lpszStr, uint32_t x, uint32_t y)
{
	uint8_t* lpVidMem = ((uint8_t*)VIDEO_MEMORY + (x + y * peek(0x0040, 0x004A)) * 2LU);
	while(*lpszStr != '\0')
	{
		*lpVidMem = *lpszStr;
		lpszStr++;
		lpVidMem += 2;
	}
}

void PutStrAttrib(uint8_t * lpszStr, uint32_t x, uint32_t y, uint8_t Attribute)
{
	uint8_t far *lpVidMem = ((uint8_t far *)VIDEO_MEMORY + (x + y * peek(0x0040, 0x004A)) * 2LU);
	while(*lpszStr != '\0')
	{
		*lpVidMem++ = *lpszStr;
		lpszStr++;
		*lpVidMem++ = Attribute;
	}
}

void SetConCursorPos(uint8_t x, uint8_t y)
{
	union REGS regs = {0};
	regs.h.ah = 0x02;
	regs.h.dh = y;
	regs.h.dl = x;
	int86(0x10, &regs, &regs);
}

void GetConCursorPos(uint8_t *x, uint8_t *y)
{
	union REGS regs = {0};
	regs.h.ah = 0x03;
	int86(0x10, &regs, &regs);
	if(x != NULL)
		*x = regs.h.dl;
	if(y != NULL)
		*y = regs.h.dh;
}

void ClearScreen(void) // Only in text mode
{
	uint16_t far *lpVidMem = (uint16_t far *)VIDEO_MEMORY;
	const uint32_t dwScreenSize = MAXCOLUMN * MAXROW;
	register uint32_t i = 0;
	for(i = 0 ; i < dwScreenSize; i++)
	{
		*lpVidMem++ = 0x0700;
	}
	SetConCursorPos(0, 0);
}

void ClearRow(int Row) // Only in text mode
{
	uint16_t far *lpVidMem = (uint16_t far *)VIDEO_MEMORY + Row * MAXCOLUMN;
	register uint16_t i = 0;
	for(i = 0 ; i < MAXCOLUMN; i++)
	{
		*lpVidMem++ = 0x0700;
	}
}

// Video mode

#define UNKNOWN 0
#define MDA 1
#define CGA 2
#define EGA 3
#define VGA 4

int CardType = UNKNOWN;

static int gfx_card_installition_check(void)
{
	union REGS regs = {0};
	regs.h.ah = 0x12; /* ah=0x12, bl=0x10: get EGA info */
	regs.x.bx = 0xff10; /* init bh to -1 */
	regs.x.cx = 0xffff; /* init cx to -1 */
	int86(0x10, &regs, &regs);
	if ( (regs.x.cx == 0xffff) || (regs.h.bh == 0xff) ) /* EGA or better not found */
	{
		if(GetVideoMode() == 0x07) /* If cuurent video mode is 7 (black-white MDA mode) assume video card is MDA otherwise assume video card is CGA */
		{
			debug("Current video mode is 7 (b&w mode) assuming video card is MDA or Hercules.\n");
			CardType = MDA;
		}
		else
		{
			debug("Assuming video card is CGA.\n");
			CardType = CGA;
		}
	}
	else /* EGA or better found */
	{
		regs.x.ax = 0x1A00;
		int86(0x10, &regs, &regs);
		if (regs.h.al == 0x1a)
		{
			debug("Video card is VGA.\n");
			CardType = VGA; /* VGA found */
		}
		else
		{
			debug("Video card is EGA.\n");
			CardType = EGA; /* VGA not found */
		}
	}
	if(CardType == MDA)
		VIDEO_MEMORY = (uint8_t far*)0xB0000000;
	else
		VIDEO_MEMORY = (uint8_t far*)0xB8000000;
	return CardType;
}

bool SetScreenResolution(uint16_t Row)
{
	union REGS regs = {0};
	gfx_card_installition_check();
	
	SetVideoMode(0x03);
	
	if(CardType < EGA && Row > 25) // Only EGA or better cards support 25+ rows
	{
		debug("Only EGA or better cards support 25+ rows.\n");
		return false;
	}
	if(CardType == EGA && (Row != 25 && Row != 43)) // EGA only support 25 and 43 rows
	{
		debug("EGA only support 25 and 43 rows.\n");
		return false;
	}
	
	if(CardType >= EGA)
	{
		switch(Row)
		{
			case 25:
				regs.x.ax = (CardType==VGA) ? 0x1114 : 0x1111;
	 			regs.h.bl = 0;
				int86(0x10, &regs, &regs);
			break;
			case 43: /* 640x350 8x8 */
			case 28: /* 640x400 8x14 (640x392, actually) */
			case 50: /* 640x400 8x8 */
				regs.x.ax = 0x0500; /* select page 0 */
	      		int86(0x10, &regs, &regs);
	      		
	      		if(CardType == VGA)
	      		{
					regs.h.ah = 0x12; /* set resolution (with BL 0x30) */
					regs.h.bl = 0x30; /* set resolution: 0 200, 1 350, 2 400 */
					regs.h.al = (Row==43) ? 1 : 2; /* 1 means 350 pixels rows, 2 400 */
					
					int86(0x10, &regs, &regs);
					if (regs.h.al==0x12)
					{ /* did the call succeed? */
						regs.h.ah = 0x0f; /* get current video mode */
						int86(0x10, &regs, &regs);
						regs.h.ah = 0; /* set mode again, to let resolution change take effect! */
						/* r.h.al |= 0x80; */ /* mode |= flag "do not clear screen" */
						int86(0x10, &regs, &regs);
					}
	      		}
	      		
	      		regs.x.ax = (Row==28) ? 0x1111 : 0x1112; /* font selection */
				/* activate 8x8 (or for 28 lines 8x14) default font */
				/* use AL 1x for full mode reset, 0x for font load only */
				regs.h.bl = 0; /* font bank 0 */
				int86(0x10, &regs, &regs);
	      		
			break;
			default:
				debug("EGA support 25, 43, VGA also support 28, 50 rows.\n");
				return false;
		}
	}
	MAXCOLUMN = (peek(0x0040, 0x004A));
	MAXROW = ((uint16_t)peekb(0x0040, 0x0084) + 1);
	return true;
}





long _farstrlen(uint8_t * str)
{
	uint8_t * temp = str;
	while(*str != '\0')
		str++;
	return (long)(str-temp);
}

uint8_t * _farmemsearch(uint8_t * s1, uint8_t * s2, long s1_length, long s2_length)
{
	long i = 0, j = 0, flag = 1;
	for(i = 0; i < s1_length - s2_length + 1; i++, flag = 1)
	{
		for(j = 0; j < s2_length; j++)
		{
			if(s1[i+j] != s2[j]) /* Not same */
			{
				flag = 0;
				break;
			}
		}
		if(flag == 1)
			return &s1[i];
	}
	return NULL;
}

uint8_t hexchar_to_byte(uint8_t byte[2])
{
	int i = 0;
	for(i = 0; i < 2; i++)
		if( (byte[i] >= '0' && byte[i] <= '9'))
			byte[i] -= '0';
		else if(byte[i] >= 'A' && byte[i] <= 'F')
			byte[i] -= 'A'-10;
		else if(byte[i] >= 'a' && byte[i] <= 'f')
			byte[i] -= 'a'-10;
	return byte[0] * 16 + byte[1];
}


void TUI_Rectangle(uint8_t * lpTitle, int x, int y, int w, int h, bool bIsDoubly)
{
	int i = 0;
	if(bIsDoubly)
	{
		PutChar(DOUBLY_TOP_LEFT, x, y); // upper left corner
		PutChar(DOUBLY_TOP_RIGHT, x+w, y); // upper right corner
		PutChar(DOUBLY_BOTTOM_LEFT, x, y+h); // lower left corner
		PutChar(DOUBLY_BOTTOM_RIGHT, x+w, y+h); // lower right corner
		for(i = 0; i < w-1; i++)
		{
			PutChar(DOUBLY_HORIZONTAL, x+i+1, y);
			PutChar(DOUBLY_HORIZONTAL, x+i+1, y+h);
		}
		for(i = 0; i < h-1; i++)
		{
			PutChar(DOUBLY_VERTICAL, x, y+i+1);
			PutChar(DOUBLY_VERTICAL, x+w, y+i+1);
		}
	}
	else if(bIsDoubly == false)
	{
		PutChar(SINGLY_TOP_LEFT, x, y); // upper left corner
		PutChar(SINGLY_TOP_RIGHT, x+w, y); // upper right corner
		PutChar(SINGLY_BOTTOM_LEFT, x, y+h); // lower left corner
		PutChar(SINGLY_BOTTOM_RIGHT, x+w, y+h); // lower right corner
		for(i = 0; i < w-1; i++)
		{
			PutChar(SINGLY_HORIZONTAL, x+i+1, y);
			PutChar(SINGLY_HORIZONTAL, x+i+1, y+h);
		}
		for(i = 0; i < h-1; i++)
		{
			PutChar(SINGLY_VERTICAL, x, y+i+1);
			PutChar(SINGLY_VERTICAL, x+w, y+i+1);
		}
	}
	PutStr(lpTitle, x+1+(w - _farstrlen(lpTitle)) / 2, y);
}


int TUI_CreateMenu(uint8_t * *lpTexts, uint8_t * lpTitle, int ActiveMenu, long StartX, long StartY, bool bIsDoubly)
{
	CHAR ch;
	int i = 0, CurrentMenu = ActiveMenu, MenuCount = 0, LongestText = 0;
	for(i = 0; lpTexts[i] != NULL; i++)
	{
		LongestText = MAX(_farstrlen(lpTexts[i]), LongestText);
		MenuCount++;
	}
	LongestText = MAX(_farstrlen(lpTitle)+2, LongestText);
	if(ActiveMenu < 0)
		ActiveMenu = 0;
	else if(ActiveMenu >= MenuCount)
		ActiveMenu = MenuCount-1;
	
	if(StartX == DEFAULT_ALIGN)
		StartX = (MAXCOLUMN - LongestText - 1) / 2;
	if(StartY == DEFAULT_ALIGN)
		StartY = (MAXROW - MenuCount - 1) / 2;
	
	TUI_Rectangle(lpTitle, StartX, StartY, LongestText+1, MenuCount+1, bIsDoubly);
	
	do
	{
		for(i = 0; i < MenuCount; i++)
		{
			if(CurrentMenu == i)
				PutStrAttrib(lpTexts[i], StartX+1, StartY+1+i, 0x70);
			else
				PutStrAttrib(lpTexts[i], StartX+1, StartY+1+i, 0x07);
		}
		if( (ch = getch()) == KB_CONTROL)
		{
			switch(ch = getch())
			{
				case KB_UP:
					if(CurrentMenu > 0) CurrentMenu--;
					else CurrentMenu = MenuCount-1;
				break;
				case KB_DOWN:
					if(CurrentMenu < MenuCount-1) CurrentMenu++;
					else CurrentMenu = 0;
				break;
			}
		}
	}
	while(ch != KB_RETURN);
	return CurrentMenu;
}

static uint8_t GetKbFlags(void)
{
	union REGS regs = {0};
	regs.h.ah = 0x02;
	int86(0x16, &regs, &regs);
	return regs.h.al;
}

// check out Ralf's interrupt list

bool CheckShiftState() // both left and right Shift accepted
{
	return GetKbFlags() & 0x03;
}

bool CheckCtrlState()
{
	return GetKbFlags() & 0x04;
}

bool CheckAltState()
{
	return GetKbFlags() & 0x08;
}

#endif

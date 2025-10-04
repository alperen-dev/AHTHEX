#include "AHTHEX.H"

static LPVOID VIDMEM = (LPBYTE)0xB0000000LU;

VOID SetVideoMode(BYTE VideoMode)
{
	union REGS regs = {0};
	regs.h.al = VideoMode;
	int86(0x10, &regs, &regs);
}

BYTE GetVideoMode(VOID)
{
	union REGS regs = {0};
	regs.h.ah = 0x0F;
	int86(0x10, &regs, &regs);
	return regs.h.al;
}

VOID PutChar(BYTE ch, DWORD x, DWORD y)
{
	*((LPBYTE)VIDMEM + (x + y * peek(0x0040, 0x004A)) * 2LU) = ch;
}

VOID ChangeCharAttrib(BYTE Attrib, DWORD x, DWORD y)
{
	*((LPBYTE)VIDMEM + (x + y * peek(0x0040, 0x004A)) * 2LU + 1LU) = Attrib;
}

VOID PutStr(LPSTR lpszStr, DWORD x, DWORD y)
{
	LPBYTE lpVidMem = ((LPBYTE)VIDMEM + (x + y * peek(0x0040, 0x004A)) * 2LU);
	while(*lpszStr != '\0')
	{
		*lpVidMem = *lpszStr;
		lpszStr++;
		lpVidMem += 2;
	}
}

VOID PutStrAttrib(LPSTR lpszStr, DWORD x, DWORD y, BYTE Attribute)
{
	LPBYTE lpVidMem = ((LPBYTE)VIDMEM + (x + y * peek(0x0040, 0x004A)) * 2LU);
	while(*lpszStr != '\0')
	{
		*lpVidMem++ = *lpszStr;
		lpszStr++;
		*lpVidMem++ = Attribute;
	}
}

VOID SetCursorPos(BYTE x, BYTE y)
{
	union REGS regs = {0};
	regs.h.ah = 0x02;
	regs.h.dh = y;
	regs.h.dl = x;
	int86(0x10, &regs, &regs);
}

VOID GetCursorPos(BYTE *x, BYTE *y)
{
	union REGS regs = {0};
	regs.h.ah = 0x03;
	int86(0x10, &regs, &regs);
	if(x != NULL)
		*x = regs.h.dl;
	if(y != NULL)
		*y = regs.h.dh;
}

VOID ClearScreen(VOID) // Only in text mode
{
	LPWORD lpVidMem = VIDMEM;
	const DWORD dwScreenSize = MAXCOLUMN * MAXROW;
	register DWORD i = 0;
	for(i = 0 ; i < dwScreenSize; i++)
	{
		*lpVidMem++ = 0x0700;
	}
	SetCursorPos(0, 0);
}

VOID ClearRow(INT Row) // Only in text mode
{
	LPWORD lpVidMem = (LPWORD)VIDMEM + Row * MAXCOLUMN;
	register WORD i = 0;
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
	if ( (regs.x.cx == 0xffff) || (regs.h.bh == 0xff) ) // EGA or better not found
	{
		if(GetVideoMode() == 0x07) /* If cuurent video mode is 7 (black-white MDA mode) assume video card is MDA otherwihs assume video card is CGA */
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
	else // EGA or better found
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
		VIDMEM = (LPBYTE)0xB0000000;
	else
		VIDMEM = (LPBYTE)0xB8000000;
	return CardType;
}

BOOL SetScreenResolution(WORD Row)
{
	union REGS regs = {0};
	gfx_card_installition_check();
	
	SetVideoMode(0x03);
	
	if(CardType < EGA && Row > 25) // Only EGA or better cards support 25+ rows
	{
		debug("Only EGA or better cards support 25+ rows.\n");
		return FALSE;
	}
	if(CardType == EGA && (Row != 25 && Row != 43)) // EGA only support 25 and 43 rows
	{
		debug("EGA only support 25 and 43 rows.\n");
		return FALSE;
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
				return FALSE;
		}
	}
	MAXCOLUMN = (peek(0x0040, 0x004A));
	MAXROW = ((WORD)peekb(0x0040, 0x0084) + 1);
	return TRUE;
}





LONG _farstrlen(LPSTR str)
{
	LPSTR temp = str;
	while(*str != '\0')
		str++;
	return (LONG)(str-temp);
}

LPSTR _farmemsearch(LPSTR s1, LPSTR s2, LONG s1_length, LONG s2_length)
{
	LONG i = 0, j = 0, flag = 1;
	for(i = 0; i < s1_length - s2_length + 1; i++, flag = 1)
	{
		for(j = 0; j < s2_length; j++)
		{
			if(s1[i+j] != s2[j]) // Not same
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

BYTE hexchar_to_byte(BYTE byte[2])
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


VOID Rectangle(LPSTR lpTitle, INT x, INT y, INT w, INT h)
{
	INT i = 0;
	PutChar(201, x, y); // upper left corner
	PutChar(187, x+w, y); // upper right corner
	PutChar(200, x, y+h); // lower left corner
	PutChar(188, x+w, y+h); // lower right corner
	for(i = 0; i < w-1; i++)
	{
		PutChar(205, x+i+1, y);
		PutChar(205, x+i+1, y+h);
	}
	for(i = 0; i < h-1; i++)
	{
		PutChar(186, x, y+i+1);
		PutChar(186, x+w, y+i+1);
	}
	PutStr(lpTitle, x+1+(w - _farstrlen(lpTitle)) / 2, y);
}


INT CreateMenu(LPSTR *lpTexts, LPSTR lpTitle, INT ActiveMenu, LONG StartX, LONG StartY)
{
	CHAR ch;
	INT i = 0, CurrentMenu = ActiveMenu, MenuCount = 0, LongestText = 0;
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
	
	Rectangle(lpTitle, StartX, StartY, LongestText+1, MenuCount+1);
	
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


#ifdef DEBUG_MODE
static FILE *log_file = NULL;
#endif

BOOL DebugInit(VOID)
{
	#ifdef DEBUG_MODE
	if( (log_file = fopen("AHTHEX.LOG", "a") ) == NULL)
	{
		return FALSE;
	}
	#endif
	return TRUE;
}

BOOL DebugClose(VOID)
{
	#ifdef DEBUG_MODE
	if(log_file == NULL)
		return FALSE;
	return !fclose(log_file);
	#else
	return TRUE;
	#endif
}

BOOL DebugPrint(INT Errno, LPSTR lpFileName, INT Line, LPSTR lpMessage)
{
	#ifdef DEBUG_MODE
	time_t ti = time(NULL);
	struct tm *t = localtime(&ti);
	DebugInit();
	if(log_file == NULL)
		return FALSE;
	fprintf(log_file, "Date: %02d.%02d.%04d, Time: %02d.%02d.%02d, Errno: %d, File Name: %s, Line: %d\nError: %s\n",
		t->tm_mday, t->tm_mon, t->tm_year+1900, t->tm_hour, t->tm_min, t->tm_sec, Errno, lpFileName, Line, (lpMessage == NULL ? strerror(Errno) : lpMessage));
	DebugClose();
	#else
	// unreferance parameters
	Errno = Errno, lpFileName = lpFileName, Line = Line, lpMessage = lpMessage;
	#endif
	return TRUE;
}

BOOL debug(CHAR *pStr, ...)
{
	#ifdef DEBUG_MODE
	va_list ap;
	DebugInit();
	if(log_file == NULL)
		return FALSE;
	va_start(ap, pStr);
	vfprintf(log_file, pStr, ap);
	DebugClose();
	return TRUE;
	#else
	pStr = pStr;
	return TRUE;
	#endif
}

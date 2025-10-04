#include <windows.h>
#include "AHTHEX.H"

VOID SetVideoMode(BYTE VideoMode) // Do NOTHING
{
	UNREFERENCED_PARAMETER(VideoMode);
}

BYTE GetVideoMode(VOID) // Do NOTHING
{
	CONSOLE_SCREEN_BUFFER_INFO csbi;

    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    MAXCOLUMN = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    MAXROW = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
	return (BYTE)0;
}

VOID PutChar(BYTE ch, DWORD x, DWORD y)
{
	CONSOLE_SCREEN_BUFFER_INFO info;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);
	CHAR_INFO ch;
	COORD cor = {0, 0};
	SMALL_RECT smr;
	smr.Top = y;
	smr.Bottom = y;
	smr.Left = x;
	smr.Right = x;
	ReadConsoleOutputA(GetStdHandle(STD_OUTPUT_HANDLE), &ch, info.dwSize, cor, &smr);
	ch.Char.AsciiChar = ch;
	WriteConsoleOutput(GetStdHandle(STD_OUTPUT_HANDLE), &ch, info.dwSize, cor, &smr);
}

VOID ChangeCharAttrib(BYTE Attrib, DWORD x, DWORD y)
{
	CONSOLE_SCREEN_BUFFER_INFO info;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);
	CHAR_INFO ch;
	COORD cor = {0, 0};
	SMALL_RECT smr;
	smr.Top = y;
	smr.Bottom = y;
	smr.Left = x;
	smr.Right = x;
	ReadConsoleOutputA(GetStdHandle(STD_OUTPUT_HANDLE), &ch, info.dwSize, cor, &smr);
	ch.Attributes = Attrib;
	WriteConsoleOutput(GetStdHandle(STD_OUTPUT_HANDLE), &ch, info.dwSize, cor, &smr);
}

VOID PutStr(LPSTR lpszStr, DWORD x, DWORD y)
{
	DWORD i = 0;
	while(*lpszStr != '\0')
	{
		PutChar(*lpszStr++, x+i, y);
		i++;
	}
}

VOID PutStrAttrib(LPSTR lpszStr, DWORD x, DWORD y, BYTE Attribute)
{
	DWORD i = 0;
	while(*lpszStr != '\0')
	{
		PutChar(*lpszStr++, x+i, y);
		ChangeCharAttrib(Attribute, x+i, y);
		i++;
	}
}

VOID SetCursorPos(BYTE x, BYTE y)
{
	COORD pos = {x, y};
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

VOID GetCursorPos(BYTE *x, BYTE *y)
{
	CONSOLE_SCREEN_BUFFER_INFO info;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);
	if(x != NULL)
		*x = regs.h.dl;
	if(y != NULL)
		*y = regs.h.dh;
}

VOID ClearScreen(VOID) // Only in text mode
{
	CONSOLE_SCREEN_BUFFER_INFO info;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);
	CHAR_INFO ch;
	COORD cor = {0, 0};
	SMALL_RECT smr;
	smr.Top = 0;
	smr.Bottom = 24;
	smr.Left = 0;
	smr.Right = 79;
	ch.Attributes = Attrib;
	ch.Char.AsciiChar = '\0';
	WriteConsoleOutput(GetStdHandle(STD_OUTPUT_HANDLE), &ch, info.dwSize, cor, &smr);
	SetCursorPos(0, 0);
}

VOID ClearRow(INT Row) // Only in text mode
{
	CONSOLE_SCREEN_BUFFER_INFO info;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);
	CHAR_INFO ch;
	COORD cor = {0, 0};
	SMALL_RECT smr;
	smr.Top = Row;
	smr.Bottom = Row;
	smr.Left = 0;
	smr.Right = 79;
	ch.Attributes = Attrib;
	ch.Char.AsciiChar = '\0';
	WriteConsoleOutput(GetStdHandle(STD_OUTPUT_HANDLE), &ch, info.dwSize, cor, &smr);
}

BOOL SetScreenResolution(WORD Row)
{
	
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
	UNREFERENCED_PARAMETER(Errno);
	UNREFERENCED_PARAMETER(lpFileName);
	UNREFERENCED_PARAMETER(Line);
	UNREFERENCED_PARAMETER(lpMessage);
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
	UNREFERENCED_PARAMETER(pStr);
	return TRUE;
	#endif
}

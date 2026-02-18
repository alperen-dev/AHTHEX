#include "AHTHEX.H"

void SetVideoMode(uint8_t VideoMode) // Do NOTHING
{
	UNREFERENCED_PARAMETER(VideoMode);
}

uint8_t GetVideoMode(void) // Do NOTHING
{
	CONSOLE_SCREEN_BUFFER_INFO csbi;

    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    MAXCOLUMN = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    MAXROW = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
	return (uint8_t)0;
}

void PutChar(uint8_t ch, uint32_t x, uint32_t y)
{
	CONSOLE_SCREEN_BUFFER_INFO info;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);
	CHAR_INFO chi;
	COORD cor = {0, 0};
	SMALL_RECT smr;
	smr.Top = y;
	smr.Bottom = y;
	smr.Left = x;
	smr.Right = x;
	ReadConsoleOutputA(GetStdHandle(STD_OUTPUT_HANDLE), &chi, info.dwSize, cor, &smr);
	chi.Char.AsciiChar = ch;
	WriteConsoleOutput(GetStdHandle(STD_OUTPUT_HANDLE), &chi, info.dwSize, cor, &smr);
}

void ChangeCharAttrib(uint8_t Attrib, uint32_t x, uint32_t y)
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

void PutStr(uint8_t * lpszStr, uint32_t x, uint32_t y)
{
	uint32_t i = 0;
	while(*lpszStr != '\0')
	{
		PutChar(*lpszStr++, x+i, y);
		i++;
	}
}

void PutStrAttrib(uint8_t * lpszStr, uint32_t x, uint32_t y, uint8_t Attribute)
{
	uint32_t i = 0;
	while(*lpszStr != '\0')
	{
		PutChar(*lpszStr++, x+i, y);
		ChangeCharAttrib(Attribute, x+i, y);
		i++;
	}
}

void SetConCursorPos(uint8_t x, uint8_t y)
{
	COORD pos = {x, y};
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

void GetConCursorPos(uint8_t *x, uint8_t *y)
{
	CONSOLE_SCREEN_BUFFER_INFO info;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);
	if(x != NULL)
		*x = info.dwCursorPosition.X;
	if(y != NULL)
		*y = info.dwCursorPosition.Y;
}

/* gpt4 generated function */
void ClearScreen(void)
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    DWORD cellsWritten;
    DWORD consoleSize;
    COORD home = {0, 0};

    if (!GetConsoleScreenBufferInfo(hConsole, &csbi)) {
        return;
    }

    // Calculate total cells in buffer
    consoleSize = csbi.dwSize.X * csbi.dwSize.Y;

    // Fill console with spaces
    FillConsoleOutputCharacter(hConsole, ' ', consoleSize, home, &cellsWritten);

    // Reset attributes too (optional)
    FillConsoleOutputAttribute(hConsole, csbi.wAttributes, consoleSize, home, &cellsWritten);

    // Move cursor back to top-left
    SetConsoleCursorPosition(hConsole, home);
}

void ClearRow(int Row) // Only in text mode
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
	ch.Attributes = 0;
	ch.Char.AsciiChar = '\0';
	WriteConsoleOutput(GetStdHandle(STD_OUTPUT_HANDLE), &ch, info.dwSize, cor, &smr);
}

bool SetScreenResolution(uint16_t Row)
{
	
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
		PutStr(ANSI_DOUBLY_TOP_LEFT, x, y); // upper left corner
		PutStr(ANSI_DOUBLY_TOP_RIGHT, x+w, y); // upper right corner
		PutStr(ANSI_DOUBLY_BOTTOM_LEFT, x, y+h); // lower left corner
		PutStr(ANSI_DOUBLY_BOTTOM_RIGHT, x+w, y+h); // lower right corner
		for(i = 0; i < w-1; i++)
		{
			PutStr(ANSI_DOUBLY_HORIZONTAL, x+i+1, y);
			PutStr(ANSI_DOUBLY_HORIZONTAL, x+i+1, y+h);
		}
		for(i = 0; i < h-1; i++)
		{
			PutStr(ANSI_DOUBLY_VERTICAL, x, y+i+1);
			PutStr(ANSI_DOUBLY_VERTICAL, x+w, y+i+1);
		}
	}
	else if(bIsDoubly == false)
	{
		PutStr(ANSI_SINGLY_TOP_LEFT, x, y); // upper left corner
		PutStr(ANSI_SINGLY_TOP_RIGHT, x+w, y); // upper right corner
		PutStr(ANSI_SINGLY_BOTTOM_LEFT, x, y+h); // lower left corner
		PutStr(ANSI_SINGLY_BOTTOM_RIGHT, x+w, y+h); // lower right corner
		for(i = 0; i < w-1; i++)
		{
			PutStr(ANSI_SINGLY_HORIZONTAL, x+i+1, y);
			PutStr(ANSI_SINGLY_HORIZONTAL, x+i+1, y+h);
		}
		for(i = 0; i < h-1; i++)
		{
			PutStr(ANSI_SINGLY_VERTICAL, x, y+i+1);
			PutStr(ANSI_SINGLY_VERTICAL, x+w, y+i+1);
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

bool CheckShiftState() // both left and right Shift accepted
{
	return !!(GetAsyncKeyState(VK_SHIFT) & 0x8000); // !! for convert to bool (its actually bad idea)
}

bool CheckCtrlState()
{
	return !!(GetAsyncKeyState(VK_CONTROL) & 0x8000);
}

bool CheckAltState()
{
	return !!(GetAsyncKeyState(VK_MENU) & 0x8000);
}

INPUTKEY *GetInput(void)
{
	INPUT_RECORD ir;
	INPUTKEY *ik = (INPUTKEY*)malloc(sizeof(INPUTKEY));
	HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);
	DWORD dwInputRead = 0;
	if(ik == NULL || hInput == NULL)
	{
		Debug(GetLastError(), "ik or hInput is NULL");
		return NULL;
	}
	if(ReadConsoleInput(hInput, &ir, (DWORD)1, &dwInputRead) == 0) // when error return 0 value
	{
		Debug(GetLastError(), "ReadConsoleInput");
		return NULL;
	}
	if (ir.EventType == KEY_EVENT && ir.Event.KeyEvent.bKeyDown)
	{
		ik->bAsciiCode = ir.Event.KeyEvent.wVirtualKeyCode;
		ik->bScanCode = ir.Event.KeyEvent.wVirtualScanCode;
		//ik->bShiftState = ir.Event.
		return ik;
	}
	
}

#ifdef DEBUG_MODE
static FILE *log_file = NULL;
#endif

bool DebugInit(void)
{
	#ifdef DEBUG_MODE
	if( (log_file = fopen("AHTHEXW.LOG", "a") ) == NULL)
	{
		return false;
	}
	#endif
	return true;
}

bool DebugClose(void)
{
	#ifdef DEBUG_MODE
	if(log_file == NULL)
		return false;
	return !fclose(log_file);
	#else
	return true;
	#endif
}

bool DebugPrint(int Errno, uint8_t * lpFileName, int Line, uint8_t * lpMessage)
{
	#ifdef DEBUG_MODE
	time_t ti = time(NULL);
	struct tm *t = localtime(&ti);
	DebugInit();
	if(log_file == NULL)
		return false;
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
	return true;
}

bool debug(CHAR *pStr, ...)
{
	#ifdef DEBUG_MODE
	va_list ap;
	DebugInit();
	if(log_file == NULL)
		return false;
	va_start(ap, pStr);
	vfprintf(log_file, pStr, ap);
	DebugClose();
	return true;
	#else
	UNREFERENCED_PARAMETER(pStr);
	return true;
	#endif
}

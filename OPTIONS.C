#include "AHTHEX.H"

WORD opt_offset_base = HEX; // HEX, DEC, OCT
WORD opt_row_number = 25; // 25, 43, 50

INT TUI_CreateOptionsWindow()
{
	
}

static VOID set_offset_base(VOID)
{
	WORD temp = 0;
	LPSTR lpTexts[] = {"Hexadecimal", "Decimal", "Octal", "Back", NULL};
	ClearScreen();
	SetConCursorPos(0, 25);
	temp = TUI_CreateMenu(lpTexts, "Set Offset Base", opt_offset_base, DEFAULT_ALIGN, DEFAULT_ALIGN, TRUE);
	if(temp < 3) // Not "Back" button
		opt_offset_base = temp;
}

static VOID set_screen_resolution(VOID)
{
	WORD temp = 0;
	LPSTR lpTexts[] = {"80x25", "80x43 (EGA, VGA)", "80x50 (VGA)", "Back", NULL};
	ClearScreen();
	SetConCursorPos(0, 25);
	temp = TUI_CreateMenu(lpTexts, "Set Screen Resolution", 
		opt_row_number == 25 ? 0 : (opt_row_number == 43 ? 1 : (opt_row_number == 50 ? 2 : 3)),
		DEFAULT_ALIGN, DEFAULT_ALIGN, TRUE);
	if(temp < 3) // Not "Back" button
	{
		opt_row_number = temp == 0 ? 25 : (temp == 1 ? 43 : (temp == 2 ? 50: opt_row_number));
		SetScreenResolution(opt_row_number);
	}
}

BOOL get_options(VOID)
{
	FILE *f = fopen(OPTIONS_FILE_NAME, "r");
	if(f == NULL)
	{
		Debug(errno, NULL);
		return FALSE;
	}
	fscanf(f, "%d%d", &opt_offset_base, &opt_row_number);
	SetScreenResolution(opt_row_number);
	return TRUE;
}

BOOL set_options(VOID)
{
	FILE *f = fopen(OPTIONS_FILE_NAME, "w");
	if(f == NULL)
	{
		Debug(errno, NULL);
		return FALSE;
	}
	fprintf(f, "%d %d", opt_offset_base, opt_row_number);
	return TRUE;
}

VOID (*options_proc[2])(VOID) = {set_offset_base, set_screen_resolution};

BOOL options(VOID)
{
	INT temp = 0, ButtonCount = 2;
	LPSTR lpTexts[] = {"Set offset base", "Set screen resolution", "Back", NULL};
	do
	{
		ClearScreen();
		SetConCursorPos(0, 25);
		temp = TUI_CreateMenu(lpTexts, "Options", 0, DEFAULT_ALIGN, DEFAULT_ALIGN, TRUE);
		if(temp < ButtonCount) // Not "Back" button
			options_proc[temp]();
	}while(temp != ButtonCount); // Not "Back" button
	set_options();
	ClearScreen();
	return TRUE;
}

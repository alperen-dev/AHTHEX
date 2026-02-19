#include "ahthex.h"

int a1;

#if 0

uint16_t opt_offset_base = HEX; /* HEX, DEC, OCT */
uint16_t opt_row_number = 25; /* 25, 43, 50 */

static void set_offset_base(void)
{
	uint16_t temp = 0;
	uint8_t * lpTexts[] = {"Hexadecimal", "Decimal", "Octal", "Back", NULL};
	ClearScreen();
	SetConCursorPos(0, 25);
	temp = TUI_CreateMenu(lpTexts, "Set Offset Base", opt_offset_base, DEFAULT_ALIGN, DEFAULT_ALIGN, true);
	if(temp < 3) /* Not "Back" button */
		opt_offset_base = temp;
}

static void set_screen_resolution(void)
{
	uint16_t temp = 0;
	uint8_t * lpTexts[] = {"80x25", "80x43 (EGA, VGA)", "80x50 (VGA)", "Back", NULL};
	ClearScreen();
	SetConCursorPos(0, 25);
	temp = TUI_CreateMenu(lpTexts, "Set Screen Resolution", 
		opt_row_number == 25 ? 0 : (opt_row_number == 43 ? 1 : (opt_row_number == 50 ? 2 : 3)),
		DEFAULT_ALIGN, DEFAULT_ALIGN, true);
	if(temp < 3) /* Not "Back" button */
	{
		opt_row_number = temp == 0 ? 25 : (temp == 1 ? 43 : (temp == 2 ? 50: opt_row_number));
		SetScreenResolution(opt_row_number);
	}
}

bool get_options(void)
{
	FILE *f = fopen(OPTIONS_FILE_NAME, "r");
	if(f == NULL)
	{
		Debug(errno, NULL);
		return false;
	}
	fscanf(f, "%d%d", &opt_offset_base, &opt_row_number);
	SetScreenResolution(opt_row_number);
	return true;
}

bool set_options(void)
{
	FILE *f = fopen(OPTIONS_FILE_NAME, "w");
	if(f == NULL)
	{
		Debug(errno, NULL);
		return false;
	}
	fprintf(f, "%d %d", opt_offset_base, opt_row_number);
	return true;
}

void (*options_proc[2])(void) = {set_offset_base, set_screen_resolution};

bool options(void)
{
	int temp = 0, ButtonCount = 2;
	uint8_t * lpTexts[] = {"Set offset base", "Set screen resolution", "Back", NULL};
	do
	{
		ClearScreen();
		SetConCursorPos(0, 25);
		TUI_Rectangle("OPTIONS", 0, 0, MAXCOLUMN, MAXROW, true);
		temp = TUI_CreateMenu(lpTexts, "Options", 0, DEFAULT_ALIGN, DEFAULT_ALIGN, true);
		if(temp < ButtonCount) /* Not "Back" button */
			options_proc[temp]();
	}while(temp != ButtonCount); /* Not "Back" button */
	set_options();
	return true;
	ClearScreen();
}

#endif

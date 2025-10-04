#include "AHTHEX.H"

#define print_help_message() printf("AHTHEX <filename1> [filename2] ... [filename32]\n")

HEXFILE h[MAX_NUMBER_OF_FILE] = {0};
int file_count = 0, cf = 0; // cf: current_file

WORD MAXCOLUMN = 80, MAXROW = 25;

void put_hex_and_char_value(BYTE byte, long column, long row)
{
	unsigned char buff[4] = {'\0'};
	sprintf(buff, "%02X", (WORD)byte);
	PutStr(buff, 10 + column * 3, row + 2);
	PutChar(iscntrl(byte) ? '.' : byte, 61 + column, row + 2);
}

int print_to_console(LPBYTE buff, long number_of_char)
{
	long i = 0;
	long first_row = MAX(h[cf].index - (h[cf].row * 16 + h[cf].col), 0L); // ilk satiri 0'dan buyuk satir numaralari icin
	if(buff == NULL)
	{
		Debug(EINVAL, "buff = NULL");
		return FALSE;
	}
	ClearScreen();
	switch(opt_offset_base)
	{
		case HEX:
			printf( "Offset(h) 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\n"
					"---------------------------------------------------------");
			printf("\n%08lX", first_row);
		break;
		case DEC:
			printf( "Offset(d) 00 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15\n"
					"---------------------------------------------------------");
			printf("\n%08ld", first_row);
		break;
		case OCT:
			printf( "Offset(o) 00 01 02 03 04 05 06 07 10 11 12 13 14 15 16 17\n"
					"---------------------------------------------------------");
			printf("\n%08lo", first_row);
		break;
	}
	for(i = 0; i < number_of_char; i++)
	{
		if(i % 16 == 0 && i != 0)
		{
			printf( opt_offset_base == HEX ? "\n%08lX" : (opt_offset_base == DEC ? "\n%08ld" : (opt_offset_base == OCT ? "\n%08lo" : "\n%08lX")),
				first_row+i);
		}
		put_hex_and_char_value(buff[i], i % 16, i / 16);
	}
	SetCursorPos(0, MAXROW-1);
	switch(opt_offset_base)
	{
		case HEX:
			printf("Press H for help.               File %X    index: %lX, row: %lX, column: %lX", cf+1, h[cf].index, h[cf].row, h[cf].col);
		break;
		case DEC:
			printf("Press H for help.               File %d    index: %ld, row: %ld, column: %ld", cf+1, h[cf].index, h[cf].row, h[cf].col);
		break;
		case OCT:
			printf("Press H for help.               File %o    index: %lo, row: %lo, column: %lo", cf+1, h[cf].index, h[cf].row, h[cf].col);
		break;
	}
	//printf("Arrow key to explore, RETURN -> Change value, S -> save, G -> go, F -> find: ");
	return TRUE;
}

BOOL change_hex_cursor_visibility(BOOL show) // show = 1: show cursor, show = 0: unshow cursor
{
	if(h[cf].buff == NULL)
	{
		Debug(EINVAL, "h[cf].buff = NULL");
		return FALSE;
	}
	
	ChangeCharAttrib( (show == 0 ? 0x07 : 0x70), 10 + h[cf].col * 3, h[cf].row + 2); // first digit of hexadecimal value
	ChangeCharAttrib( (show == 0 ? 0x07 : 0x70), 11 + h[cf].col * 3, h[cf].row + 2); // second digit of hexadecimal value
	ChangeCharAttrib( (show == 0 ? 0x07 : 0x70), 61 + h[cf].col, h[cf].row + 2); // ascii value
	return TRUE;
}

#define unshow_cursor() change_hex_cursor_visibility(0)
#define show_cursor() change_hex_cursor_visibility(1)


BOOL goto_cursor(long index, long row)
{
	if(h[cf].buff == NULL)
	{
		Debug(EINVAL, "h[cf].buff = NULL");
		return FALSE;
	}
	else if(h[cf].size < 0)
	{
		Debug(EINVAL, "file_size < 0");
		return FALSE;
	}
	
	if(index < 0)
		index = 0;
	
	if(row < 0)
		row = 0;
	
	if(index > h[cf].size-1)
		index = h[cf].size-1;
	
	if(row > MAXROW-4)
		row = MAXROW-4;
	
	unshow_cursor();
	
	h[cf].index = index;
	h[cf].row = row;
	h[cf].col = h[cf].index % 16L;
		
	return TRUE;
}

void control_keys(BYTE secondaryByte)
{
	switch(secondaryByte)
	{
		case KB_UP:
			goto_cursor(h[cf].index-16, h[cf].row-1);
		break;
		case KB_DOWN:
			goto_cursor(h[cf].index+16, h[cf].row+1);
		break;
		case KB_LEFT:
			goto_cursor(h[cf].index-1, h[cf].row-( (16-h[cf].col) /16));
		break;
		case KB_RIGHT:
			goto_cursor(h[cf].index+1, h[cf].row+( (h[cf].col+1)/16 ));
		break;
		case KB_PG_UP:
			goto_cursor(h[cf].index-16*(MAXROW-3), h[cf].row-1);
		break;
		case KB_PG_DOWN:
			goto_cursor(h[cf].index+16*(MAXROW-3), h[cf].row+1);
		break;
		case KB_HOME:
			goto_cursor(0L, 0L);
		break;
		case KB_END:
			goto_cursor(h[cf].size-1, (h[cf].size-1)/16L);
		break;
		
		case KB_CTRL_RIGHT:
			if(++cf >= file_count)
				cf = 0;
		break;
		case KB_CTRL_LEFT:
			if(--cf < 0)
				cf = file_count-1;
		break;
		case KB_ALT_S: // save as (TODO!!!)
			save_as();
		break;
		case KB_ALT_O:
			options();
		break;
		case KB_DEL: // delete byte
			delete_byte();
		break;
	}
}

BOOL change_value(int type)
{
	if(h[cf].buff == NULL)
	{
		Debug(EINVAL, "h[cf].buff = NULL");
		return FALSE;
	}
	SetCursorPos(0, 0);
	printf("\t\t\t\t\t\t\t\t");
	if(type == HEX)
	{
		BYTE val = 0;
		ChangeCharAttrib(0x87, 10 + h[cf].col * 3, h[cf].row + 2); // first digit of hexadecimal value, blink
		ChangeCharAttrib(0x87, 11 + h[cf].col * 3, h[cf].row + 2); // second digit of hexadecimal value, blink
		
		printf("\rEnter new hex value: ");
		scanf("%x", &val);
		
		h[cf].buff[ h[cf].index ] = val;
		put_hex_and_char_value(val, h[cf].col, h[cf].row);
		ChangeCharAttrib(0x70, 10 + h[cf].col * 3, h[cf].row + 2); // first digit of hexadecimal value, reverse
		ChangeCharAttrib(0x70, 11 + h[cf].col * 3, h[cf].row + 2); // second digit of hexadecimal value, reverse
	}
	else if(type == ASCII)
	{
		BYTE ch = 0;
		ChangeCharAttrib(0x87, 61 + h[cf].col, h[cf].row + 2); // ascii value, blink
		
		printf("\rEnter new char: ");
		ch = getche();
		
		h[cf].buff[ h[cf].index ] = ch;
		put_hex_and_char_value(ch, h[cf].col, h[cf].row);
		ChangeCharAttrib(0x70, 61 + h[cf].col, h[cf].row + 2); // ascii value, reverse
	}
	SetCursorPos(0, MAXROW-1);
	return TRUE;
}

LPBYTE add_byte()
{
	LPBYTE temp = NULL;
	
	if(h[cf].buff == NULL)
	{
		Debug(EINVAL, "h[cf].buff = NULL");
		return NULL;
	}
	
	if( (temp = (LPBYTE)farrealloc((BYTE far*)h[cf].buff, h[cf].size+1)) != NULL)
	{
		h[cf].buff = temp;
		h[cf].buff[ h[cf].size ] = '\0';
		h[cf].size++;
	}
	else
	{
		Debug(errno, NULL);
	}
	return h[cf].buff;
}

LPBYTE insert_byte()
{
	register long i = 0;
	add_byte();
	for(i = h[cf].size-1; i > h[cf].index; i--)
	{
		h[cf].buff[i] = h[cf].buff[i-1];
	}
	h[cf].buff[i] = '\0';
	return h[cf].buff;
}

LPBYTE delete_byte()
{
	register long i = 0;
	if(h[cf].size > 1)
	{
		for(i = h[cf].index; i < h[cf].size-1; i++)
		{
			h[cf].buff[i] = h[cf].buff[i+1];
		}
		return h[cf].buff = farrealloc((unsigned char far*)h[cf].buff, --h[cf].size);
	}
	return h[cf].buff;
}

BOOL find_value(int type)
{
	unsigned char *value = NULL, FAR *ptr = NULL;
	int value_length = 0;
	
	if(h[cf].buff == NULL)
	{
		Debug(EINVAL, "h[cf].buff = NULL");
		return FALSE;
	}
	
	SetCursorPos(0, MAXROW-1);
	ClearRow(MAXROW-1);
	
	if((value = malloc(1024*sizeof(char))) == NULL)
	{
		Debug(errno, NULL);
		return FALSE;
	}
	
	if(type == HEX)
	{
		int i = 0;
		printf("HEX value to find (max length 512 byte): ");
		fgets(value, 16, stdin);
		value[value_length = (strlen(value)-1)] = '\0';
		
		for(i = 0; i < value_length/2; i++)
		{
			value[i] = hexchar_to_byte(&value[i*2]);
		}
		value_length /= 2;
	}
	else if(type == ASCII)
	{		
		printf("ASCII value to find (max length 1024): ");
		fgets(value, 16, stdin);
		value[value_length = strlen(value)-1] = '\0';
	}
	if((ptr = _farmemsearch(&(h[cf].buff[h[cf].index+1]) , (LPSTR)value, h[cf].size - h[cf].index - 1, value_length)) == NULL)
	{
		SetCursorPos(0, MAXROW-1);
		ClearRow(MAXROW-1);
		printf("Not found!");
		getch();
	}
	else
	{
		goto_cursor(ptr - h[cf].buff, (ptr - h[cf].buff)/16);
	}
	
	free(value);
	SetCursorPos(0, MAXROW-1);
	return TRUE;
}

void help_screen(void)
{
	ClearScreen();
	printf( "Keys:\n"
			" H --> help screen\n" 
			" Arrow keys, Page Up, Page Down --> move cursor\n"
			" Home --> Goto index first\n"
			" End --> Goto index last\n"
			" CTRL + Right --> Next file\n"
			" CTRL + Left --> Previous file\n"
			" A --> Add new byte to the end of current file\n"
			" I --> Insert new byte to current file\n"
			" Backspace or Delete --> Delete byte from current file"
			" S --> Save current file (overwrite)\n"
			" CTRL + S --> Save all file (overwrite)\n"
			" ALT + S --> Save as\n"
			" G --> Goto\n"
			" F --> Find byte using ASCII\n"
			" CTRL + F --> Find byte using Hexadecimal\n"
			" Return (Enter) --> Change value (byte) using ASCII\n"
			" CTRL + Return (Enter) --> Change value (byte) using Hexadecimal\n"
			" CTRL + X --> Close current file without save\n"
			" CTRL + N --> Create new file (TODO)\n"
			" CTRL + O --> Open file\n"
			" ALT + O --> Options"
			" ESC --> Exit without save\n"
			"\n"
			"\n"
			"Press any key to return editor\n");
	PutChar('A', 38, MAXROW-1); ChangeCharAttrib(0x0F, 38, MAXROW-1);
	PutChar('H', 39, MAXROW-1); ChangeCharAttrib(0x0F, 39, MAXROW-1);
	PutChar('T', 40, MAXROW-1); ChangeCharAttrib(0x0F, 40, MAXROW-1);
	
	getch();
	ClearScreen();
}

int main(int argc, char **argv)
{
	char ch = 0;
	BYTE CurrentVideoMode = GetVideoMode();
	
	get_options();
	
	if(argc > MAX_NUMBER_OF_FILE+1)
	{
		print_help_message();
		Debug(EINVAL, NULL);
		return EINVAL;
	}
	
	file_count = argc-1;
	
	if(file_count == 0)
	{
		create_new_file();
	}
	else // command line parameter
	{
		for(cf = 0; cf < file_count; cf++)
		{
			h[cf].filename = argv[cf+1];
		}
		
		for(cf = 0; cf < file_count; cf++)
		{
			if( read_file() == NULL)
			{
				fprintf(stderr, "%s file can't open: %s", h[cf].filename, strerror(errno));
				return errno;
			}
		}
		cf = 0;
	}
	
	do
	{
		print_to_console(&( h[cf].buff[ h[cf].index - (h[cf].col + h[cf].row * 16) ] ),
			MIN( (h[cf].col + 1 + h[cf].row * 16) + (h[cf].size-1-h[cf].index) , 16*(MAXROW-3)) );
		show_cursor();
		switch(toupper(ch = getch()))
		{
			case 'H':
				help_screen();
			break;
			case KB_CONTROL:
				control_keys(ch = getch());
			break;
			case 'A': // Add new byte
				add_byte();
			break;
			case 'I': // Insert new byte
				insert_byte();
			break;
			case KB_BACKSPACE: // Delete byte
				delete_byte();
			break;
			case 'S': // write (save)
				write_file();
			break;
			case KB_CTRL_S:
			{
				int i = 0, temp = cf;
				for(i = 0; i < file_count; i++)
				{
					cf = i;
					write_file();
				}
				cf = temp;
			}
			break;
			case 'G': // go to address
			{
				long index = 0;
				SetCursorPos(0, 0);
				printf("\t\t\t\t\t\t\t\t\rgo to index(d): ");
				scanf(opt_offset_base == HEX ? "%lx" : (opt_offset_base == DEC ? "%ld" : (opt_offset_base == OCT ? "%lo" : "%lx")), &index);
				goto_cursor(index, index/16);
			}
			break;
			
			case 'F': // find ascii
				find_value(ASCII);
			break;
			case KB_CTRL_F: // find hexadecimal
				find_value(HEX);
			break;
			case KB_RETURN: // 13, for ascii edit
				change_value(ASCII);
			break;
			case KB_CTRL_RETURN: // 10, for hexadecimal edit
				change_value(HEX);
			break;
			case KB_CTRL_X: // close current file without save
				ch = close_file();
			break;
			case KB_CTRL_N:
				create_new_file();
			break;
			case KB_CTRL_O:
				open_file();
			break;
		}
	}while(ch != KB_ESC);
	SetVideoMode(CurrentVideoMode);
	return 0;
}

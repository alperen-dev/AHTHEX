#include "AHTHEX.H"

#define print_help_message() printf("AHTHEX <filename1> [filename2] ... [filename32]\n")

Node *n = NULL; 
HEXFILE *cf = NULL; // cf: current_file
size_t file_count = 0; 

WORD MAXCOLUMN = 80, MAXROW = 25;

void put_hex_and_char_value(BYTE byte, long column, long row)
{
	unsigned char buff[4] = {'\0'};
	sprintf(buff, "%02X", (WORD)byte);
	PutStr(buff, 10 + column * 3, row + 2);
	PutChar(iscntrl(byte) ? '.' : byte, 61 + column, row + 2);
}

bool print_to_console(LPBYTE buff, long number_of_char)
{
	if(cf != NULL)
	{
		long i = 0;
		long first_row = MAX(cf->index - (cf->row * 16 + cf->col), 0L); // ilk satiri 0'dan buyuk satir numaralari icin
		
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
		SetConCursorPos(0, MAXROW-1);
		switch(opt_offset_base)
		{
			case HEX:
				printf("Press H for help.               File %X    index: %lX, row: %lX, column: %lX", 0, cf->index, cf->row, cf->col);
			break;
			case DEC:
				printf("Press H for help.               File %d    index: %ld, row: %ld, column: %ld", 0, cf->index, cf->row, cf->col);
			break;
			case OCT:
				printf("Press H for help.               File %o    index: %lo, row: %lo, column: %lo", 0, cf->index, cf->row, cf->col);
			break;
		}
		return TRUE;
	}
	return FALSE;
	
}

BOOL change_hex_cursor_visibility(BOOL show) // show = 1: show cursor, show = 0: unshow cursor
{
	if(cf->buff == NULL)
	{
		Debug(EINVAL, "hf->buff = NULL");
		return FALSE;
	}
	
	ChangeCharAttrib( (show == 0 ? 0x07 : 0x70), 10 + cf->col * 3, cf->row + 2); // first digit of hexadecimal value
	ChangeCharAttrib( (show == 0 ? 0x07 : 0x70), 11 + cf->col * 3, cf->row + 2); // second digit of hexadecimal value
	ChangeCharAttrib( (show == 0 ? 0x07 : 0x70), 61 + cf->col, cf->row + 2); // ascii value
	return TRUE;
}

#define unshow_cursor() change_hex_cursor_visibility(0)
#define show_cursor() change_hex_cursor_visibility(1)


BOOL goto_cursor(long index, long row)
{
	if(cf->buff == NULL)
	{
		Debug(EINVAL, "cf->buff = NULL");
		return FALSE;
	}
	else if(cf->size < 0)
	{
		Debug(EINVAL, "file_size < 0");
		return FALSE;
	}
	
	if(index < 0)
		index = 0;
	
	if(row < 0)
		row = 0;
	
	if(index > cf->size-1)
		index = cf->size-1;
	
	if(row > MAXROW-4)
		row = MAXROW-4;
	
	unshow_cursor();
	
	cf->index = index;
	cf->row = row;
	cf->col = cf->index % 16L;
		
	return TRUE;
}

void control_keys(BYTE secondaryByte)
{
	switch(secondaryByte)
	{
		case KB_UP:
			goto_cursor(cf->index-16, cf->row-1);
		break;
		case KB_DOWN:
			goto_cursor(cf->index+16, cf->row+1);
		break;
		case KB_LEFT:
			goto_cursor(cf->index-1, cf->row-( (16-cf->col) /16));
		break;
		case KB_RIGHT:
			goto_cursor(cf->index+1, cf->row+( (cf->col+1)/16 ));
		break;
		case KB_PG_UP:
			goto_cursor(cf->index-16*(MAXROW-3), cf->row-1);
		break;
		case KB_PG_DOWN:
			goto_cursor(cf->index+16*(MAXROW-3), cf->row+1);
		break;
		case KB_HOME:
			goto_cursor(0L, 0L);
		break;
		case KB_END:
			goto_cursor(cf->size-1, (cf->size-1)/16L);
		break;
		
		case KB_CTRL_RIGHT:
			n = GetNextNode(n);
			cf = (HEXFILE*)GetData(n);
		break;
		case KB_CTRL_LEFT:
			n = GetPrevNode(n);
			cf = (HEXFILE*)GetData(n);
		break;
		//case KB_ALT_S: // save as (TODO!!!)
		//	save_as(cf);
		//break;
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
	if(cf->buff == NULL)
	{
		Debug(EINVAL, "cf->buff = NULL");
		return FALSE;
	}
	SetConCursorPos(0, 0);
	printf("\t\t\t\t\t\t\t\t");
	if(type == HEX)
	{
		BYTE val = 0;
		ChangeCharAttrib(0x87, 10 + cf->col * 3, cf->row + 2); // first digit of hexadecimal value, blink
		ChangeCharAttrib(0x87, 11 + cf->col * 3, cf->row + 2); // second digit of hexadecimal value, blink
		
		printf("\rEnter new hex value: ");
		scanf("%x", &val);
		
		cf->buff[ cf->index ] = val;
		put_hex_and_char_value(val, cf->col, cf->row);
		ChangeCharAttrib(0x70, 10 + cf->col * 3, cf->row + 2); // first digit of hexadecimal value, reverse
		ChangeCharAttrib(0x70, 11 + cf->col * 3, cf->row + 2); // second digit of hexadecimal value, reverse
	}
	else if(type == ASCII)
	{
		BYTE ch = 0;
		ChangeCharAttrib(0x87, 61 + cf->col, cf->row + 2); // ascii value, blink
		
		printf("\rEnter new char: ");
		ch = getche();
		
		cf->buff[ cf->index ] = ch;
		put_hex_and_char_value(ch, cf->col, cf->row);
		ChangeCharAttrib(0x70, 61 + cf->col, cf->row + 2); // ascii value, reverse
	}
	SetConCursorPos(0, MAXROW-1);
	return TRUE;
}

LPBYTE add_byte()
{
	LPBYTE temp = NULL;
	
	if(cf->buff == NULL)
	{
		Debug(EINVAL, "cf->buff = NULL");
		return NULL;
	}
	
	if( (temp = (LPBYTE)farrealloc((UCHAR far*)cf->buff, cf->size+1)) != NULL)
	{
		cf->buff = temp;
		cf->buff[ cf->size ] = '\0';
		cf->size++;
	}
	else
	{
		Debug(errno, NULL);
	}
	return cf->buff;
}

LPBYTE insert_byte()
{
	register long i = 0;
	add_byte();
	for(i = cf->size-1; i > cf->index; i--)
	{
		cf->buff[i] = cf->buff[i-1];
	}
	cf->buff[i] = '\0';
	return cf->buff;
}

LPBYTE delete_byte()
{
	register long i = 0;
	if(cf->size > 1)
	{
		for(i = cf->index; i < cf->size-1; i++)
		{
			cf->buff[i] = cf->buff[i+1];
		}
		return cf->buff = farrealloc((UCHAR far*)cf->buff, --cf->size);
	}
	return cf->buff;
}

BOOL find_value(int type)
{
	unsigned char *value = NULL, FAR *ptr = NULL;
	int value_length = 0;
	
	if(cf->buff == NULL)
	{
		Debug(EINVAL, "cf->buff = NULL");
		return FALSE;
	}
	
	SetConCursorPos(0, MAXROW-1);
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
	if((ptr = _farmemsearch(&(cf->buff[cf->index+1]) , (LPSTR)value, cf->size - cf->index - 1, value_length)) == NULL)
	{
		SetConCursorPos(0, MAXROW-1);
		ClearRow(MAXROW-1);
		printf("Not found!");
		getch();
	}
	else
	{
		goto_cursor(ptr - cf->buff, (ptr - cf->buff)/16);
	}
	
	free(value);
	SetConCursorPos(0, MAXROW-1);
	return TRUE;
}

void help_screen(void)
{
	ClearScreen(); // konu olarak ayir
	printf( "Keys:\n"
			" H --> help screen\n" 
			" Arrow keys, Page Up, Page Down --> move cursor\n"
			" Home --> Goto index first\n"
			" End --> Goto index last\n"
			" CTRL + Right --> Next file\n"
			" CTRL + Left --> Previous file\n"
			" CTRL + S --> Save\n"
			" CTRL + ALT + S --> Save as...\n"
			" CTRL + SHIFT + S --> Save all\n"
			" A --> Add new byte to the end of current file\n"
			" I --> Insert new byte to current file\n"
			" Backspace or Delete --> Delete byte from current file\n"
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
			" ALT + O --> Options\n"
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
	unsigned char ch = 0;
	size_t i = 0;
	BYTE CurrentVideoMode = GetVideoMode();
	
	GetInput();
	
	get_options();
	
	
	
	if(argc > MAX_NUMBER_OF_FILE+1)
	{
		print_help_message();
		Debug(EINVAL, NULL);
		return EINVAL;
	}
	
	file_count = argc-1; // assume all of command line parameter is file name
	
	// create circular doubly linked list for file header
	for(i = 0; i < file_count; i++)
	{
		HEXFILE *hf = (HEXFILE*)calloc(1, sizeof(HEXFILE)); // actually we can use malloc but we want zero initialization
		if(hf == NULL)
		{
			Debug(ENOMEM, "Not enought memory");
			return ENOMEM;
		}
		
		
		hf->filename = argv[i+1];
		
		if( read_file(hf) == NULL)
		{
			fprintf(stderr, "%s file can't open: %s", hf->filename, strerror(errno));
			return errno;
		}
		
		
		
		if((n = AddNext(n, hf)) == NULL)
		{
			Debug(errno, "List couldn't created");
			return errno;
		}
	}
	n = GetNextNode(n); // start at first parameter
	
	
	
	if(file_count == 0)
	{
		create_new_file(n);
	}
	
	do
	{		
		cf = (HEXFILE*)n->data;
		print_to_console(&( cf->buff[ cf->index - (cf->col + cf->row * 16) ] ),
			MIN( (cf->col + 1 + cf->row * 16) + (cf->size-1-cf->index) , 16*(MAXROW-3)) );
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
				write_file(cf);
			break;
			case KB_CTRL_S:
			{
				Node *temp = n;
				do
				{
					write_file((HEXFILE*)n->data);
					temp = GetNextNode(n);
				}while(temp != n); // circular doubly linked list
			}
			break;
			case 'G': // go to address
			{
				long index = 0;
				SetConCursorPos(0, 0);
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
				ch = close_file(n);
			break;
			case KB_CTRL_N:
				create_new_file(n);
			break;
			case KB_CTRL_O:
				open_file(n);
			break;
		}
	}while(ch != KB_ESC);
	SetVideoMode(CurrentVideoMode);
	return 0;
}

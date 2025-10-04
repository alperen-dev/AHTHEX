#include "AHTHEX.H"

static char temp_filename[MAXPATH];

long get_file_size(FILE *f) // error: -1
{
	long temp = 0L, size = 0L;
	if(f == NULL)
		return -1;
	if((temp = ftell(f)) == -1L)
		return -1;
	if(fseek(f, 0L, SEEK_END) != 0)
		return -1;
	if((size = ftell(f)) == -1L)
		return -1;
	if(fseek(f, temp, SEEK_SET) != 0)
		return -1;
	return  (h[cf].size = size);
}

LPBYTE read_file(VOID)
{
	register long i = 0;
	FILE *f = NULL;
	LPBYTE buff = NULL;
	if(h[cf].filename == NULL)
	{
		errno = EINVAL;
		Debug(errno, "h[cf].filename = NULL");
		return NULL;
	}
	if((f = fopen(h[cf].filename, "rb")) == NULL)
	{
		Debug(errno, NULL);
		return NULL;
	}
	if( (i = get_file_size(f)) < 0)
	{
		errno = ERANGE;
		Debug(errno, NULL);
		fclose(f);
		return NULL;
	}
	if( (buff = (LPBYTE)farmalloc( h[cf].size = MAX(h[cf].size, 1) ) ) == NULL)
	{
		errno = ENOMEM;
		Debug(errno, NULL);
		fclose(f);
		return NULL; // Not enought memory.
	}
	if(i == 0) // empty file
	{
		buff[0] = '\0';
		fclose(f);
		return (LPBYTE)(h[cf].buff = buff);
	}
	if(fseek(f, 0L, SEEK_SET))
	{
		Debug(errno, "Cursor can't set");
		fclose(f);
		farfree((void far*)buff);
		return NULL;
	}
	for(i = 0; i < h[cf].size; i++) // read file
	{
		*buff++ = fgetc(f);
	}
	fclose(f);
	return (LPBYTE)(h[cf].buff = buff-i);
}

BOOL write_file(VOID)
{
	register long i = 0;
	FILE *f = NULL;
	LPBYTE buff = h[cf].buff;
	
	if(h[cf].filename == NULL)
	{
		return save_as();
	}
	
	if(h[cf].buff == NULL)
	{
		Debug(EINVAL, "h[cf].buff = NULL");
		return FALSE;
	}
	
	if( (f = fopen(h[cf].filename, "wb")) == NULL)
	{
		Debug(errno, NULL);
		return FALSE;
	}
	for(i = 0; i < h[cf].size; i++)
	{
		fputc(*(buff++), f);
	}
	fclose(f);
	return TRUE;
}

BOOL save_as(VOID)
{
	ClearScreen();
	printf("type file name: ");
	fgets(temp_filename, MAXPATH, stdin);
	temp_filename[strlen(temp_filename)-1] = '\0';
	
	if(access(temp_filename, 0) == 0)
	{
		char ch = 0;
		while(toupper(ch) != 'Y')
		{
			printf("\n'%s' already exist. Overwrite Y/N ", temp_filename);
			if( toupper(ch = getch()) == 'N')
				return FALSE;
		}
	}
	
	if(h[cf].filename != NULL)
		free(h[cf].filename);
	
	if((h[cf].filename = malloc(sizeof(char) * (strlen(temp_filename) + 1) )) == NULL)
	{
		Debug(errno, NULL);
		return FALSE;
	}
	strcpy(h[cf].filename, temp_filename);
	ClearScreen();
	return write_file();
}

char close_file(VOID)
{
	int i = 0;
	if(cf != file_count-1)
	{
		for(i = cf; i < file_count-1; i++)
		{
			memmove(&h[i], &h[i+1], sizeof(HEXFILE));
		}
	}
	else if(cf != 0)
	{
		memmove(&h[cf], '\0', sizeof(HEXFILE));
		cf--;
	}
	else // cf is 0 and last file: close editor
	{
		return KB_ESC;
	}
	file_count--;
	return 0;
}

BOOL open_file(VOID)
{
	int temp_cf = cf;
	ClearScreen();
	if(file_count >= MAX_NUMBER_OF_FILE)
	{
		printf("Can't open file: file_count >= MAX_NUMBER_OF_FILE\n");
		Debug(EMFILE, NULL);
		getch();
		return FALSE;
	}
	printf("Type file name: ");
	
	fgets(temp_filename, MAXPATH, stdin);
	temp_filename[strlen(temp_filename)-1] = '\0';
	
	if((h[file_count].filename = malloc(sizeof(char) * (strlen(temp_filename) + 1) )) == NULL)
	{
		Debug(errno, NULL);
		return FALSE;
	}
	cf = file_count;
	strcpy(h[cf].filename, temp_filename);
	if(read_file() == NULL)
	{
		cf = temp_cf;
		ClearScreen();
		return FALSE;
	}
	file_count++;
	ClearScreen();
	return TRUE;
}

BOOL create_new_file(VOID)
{
	if(file_count >= MAX_NUMBER_OF_FILE)
	{
		Debug(EMFILE, NULL);
		return FALSE;
	}
	h[file_count].buff = (LPBYTE)farmalloc( 1 );
	cf = file_count;
	file_count++;
	h[cf].buff[0] = '\0';
	h[cf].size = 1;
	return FALSE;
}

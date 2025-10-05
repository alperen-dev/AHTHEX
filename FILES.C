// TODO: make this file portable.

#include "AHTHEX.H"

static char temp_filename[MAXPATH];

long get_file_size(FILE *f) // error: -1
{
	long temp = 0L, size = 0L;
	if(f == NULL) // wrong parameter
		return -1;
	if((temp = ftell(f)) == -1L) // current position cannot get
		return -1;
	if(fseek(f, 0L, SEEK_END) != 0) // cursor cannot set
		return -1;
	if((size = ftell(f)) == -1L) // current position cannot get
		return -1;
	if(fseek(f, temp, SEEK_SET) != 0) // cursor cannot set
		return -1;
	return size;
}

LPBYTE read_file(HEXFILE *h)
{
	register long i = 0;
	FILE *f = NULL;
	LPBYTE buff = NULL;
	
	// check parameter control
	if(h->filename == NULL)
	{
		errno = EINVAL;
		Debug(errno, "h->filename = NULL");
		return NULL;
	}
	
	// open file as read access
	if((f = fopen(h->filename, "rb")) == NULL)
	{
		Debug(errno, NULL);
		return NULL;
	}
	
	// get file size
	if( (i = get_file_size(f)) < 0)
	{
		errno = ERANGE;
		Debug(errno, "Cannot get file size");
		fclose(f);
		return NULL;
	}
	
	// allocate memory as large as file size
	if( (buff = (LPBYTE)farmalloc( h->size = MAX(i, 1) ) ) == NULL)
	{
		errno = ENOMEM;
		Debug(errno, NULL);
		fclose(f);
		return NULL; // Not enought memory.
	}
	
	// check file emptiness
	if(i == 0) // empty file
	{
		buff[0] = '\0';
		fclose(f);
		return (LPBYTE)(h->buff = buff);
	}
	
	// set file cursor to start
	if(fseek(f, 0L, SEEK_SET))
	{
		Debug(errno, "Cursor can't set");
		fclose(f);
		farfree((void far*)buff);
		return NULL;
	}
	
	// read file to buffer
	for(i = 0; i < h->size; i++) // read file
	{
		*buff++ = fgetc(f);
	}
	
	fclose(f);
	return (LPBYTE)(h->buff = buff-i);
}

BOOL write_file(HEXFILE *h)
{
	register long i = 0;
	FILE *f = NULL;
	LPBYTE buff = h->buff;
	
	// check parameters
	if(h->filename == NULL)
	{
		return save_as(h);
	}
	if(h->buff == NULL)
	{
		Debug(EINVAL, "h->buff = NULL");
		return FALSE;
	}
	
	// open file as write access
	if( (f = fopen(h->filename, "wb")) == NULL)
	{
		Debug(errno, NULL);
		return FALSE;
	}
	
	// write file
	for(i = 0; i < h->size; i++)
	{
		fputc(*(buff++), f);
	}
	
	fclose(f);
	return TRUE;
}

BOOL save_as(HEXFILE *h)
{
	ClearScreen();
	printf("type file name: ");
	fgets(temp_filename, MAXPATH, stdin);
	temp_filename[strlen(temp_filename)-1] = (temp_filename[strlen(temp_filename)-1] == '\n' ? '\0' : temp_filename[strlen(temp_filename)-1]);
	
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
	
	if(h->filename != NULL)
		free(h->filename);
	
	if((h->filename = malloc(sizeof(char) * (strlen(temp_filename) + 1) )) == NULL)
	{
		Debug(errno, NULL);
		return FALSE;
	}
	strcpy(h->filename, temp_filename);
	ClearScreen();
	return write_file(cf);
}

// These functions get parameter node because they will change multiple file properities

char close_file(Node *n)
{
	
	if(n != NULL)
	{
		if(n->data != NULL)
		{
			free(n->data);
		}
		DeleteNode(n);
	}
	file_count--;
	if(file_count == 0) // no file left, close editor
	{
		return KB_ESC;
	}
	return 0;
}

BOOL open_file(Node *n)
{
	if(n != NULL)
	{
		HEXFILE *h = (HEXFILE*)calloc(1, sizeof(HEXFILE));
		if(h == NULL)
		{
			errno = ENOMEM;
			Debug(errno, NULL);
			return NULL; // Not enought memory.
		}
		
		// get file name from user
		ClearScreen();
		printf("Type file name: ");
		
		fgets(temp_filename, MAXPATH, stdin);
		temp_filename[strlen(temp_filename)-1] = (temp_filename[strlen(temp_filename)-1] == '\n' ? '\0' : temp_filename[strlen(temp_filename)-1]);
		
		if((h->filename = (char*)malloc(sizeof(char) * (strlen(temp_filename) + 1) )) == NULL)
		{
			Debug(errno, NULL);
			free(h);
			return FALSE;
		}
		strcpy(h->filename, temp_filename);
		
		ClearScreen();
		
		if(read_file(h) == NULL)
		{
			free(h->filename);
			free(h);
			return FALSE;
		}
		
		if(AddNext(n, h) == NULL)
		{
			farfree((void far*)h->buff);
			free(h->filename);
			free(h);
			Debug(errno, "New element cannot add to list");
			return FALSE;
		}
		
		file_count++;
		return TRUE;
	}
	return FALSE;
}

BOOL create_new_file(Node *n)
{
	HEXFILE *h = (HEXFILE*)calloc(1, sizeof(HEXFILE));
	if(h == NULL)
	{
		errno = ENOMEM;
		Debug(errno, NULL);
		return NULL; // Not enought memory.
	}
	
	h->buff = (LPBYTE)farmalloc( 1 );
	file_count++;
	h->buff[0] = '\0';
	h->size = 1;
	
	if(AddNext(n, h) == NULL)
	{
		farfree((void far*)h->buff);
		free(h->filename);
		free(h);
		Debug(errno, "New element cannot add to list");
		return FALSE;
	}
	
	return FALSE;
}

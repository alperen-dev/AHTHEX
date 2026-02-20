#include <stdio.h>
#include <dos.h>
#include <i86.h>
#include <assert.h>
#include <stdarg.h>
#include <string.h>
#include <conio.h>

#include "ahtdefs.h"
#include "console.h"
#include "log.h"

#define BDA_SEGMENT	0x0040U

#define MONO_CRTC_INDEX_PORT		0x03B4
#define MONO_CRTC_DATA_PORT			0x03B5
#define GDC_INDEX_PORT				0x03CE	/* GDC: Graphics Data Controller */
#define GDC_DATA_PORT				0x03CF	/* GDC: Graphics Data Controller */
#define COLOR_CRTC_INDEX_PORT		0x03D4
#define COLOR_CRTC_DATA_PORT		0x03D5




static uint8_t far * VIDEO_MEMORY = (uint8_t far*)0xB8000000LU;

static AnsiSupport ansiSupport = ANSI_SUPPORT_UNKNOWN;

typedef struct DosCoord
{
	uint8_t row;
	uint8_t col;
}DosCoord;


#define PEEKB(s, o) (*((uint8_t far*)(((uint32_t)(s) << 16LU) + (o))))
#define PEEKW(s, o) (*((uint16_t far*)(((uint32_t)(s) << 16LU) + (o))))
#define POKEB(s, o, v) (*((uint8_t far*)(((uint32_t)(s) << 16LU) + (o))) = (v))
#define POKEW(s, o, v) (*((uint16_t far*)(((uint32_t)(s) << 16LU) + (o))) = (v))

#define GET_SCREEN_WIDTH()				(PEEKB(BDA_SEGMENT, 0x004A))
#define GET_SCREEN_HEIGHT()				(PEEKB(BDA_SEGMENT, 0x0084) + 1)
#define GET_CURRENT_PAGE()				(PEEKB(BDA_SEGMENT, 0x0062))
#define GET_CURRENT_PAGE_OFFSET()		(PEEKW(BDA_SEGMENT, 0x004E))
#define GET_VIDEO_MODE()				(PEEKB(BDA_SEGMENT, 0x0049))
#define GET_CURSOR_COL()				(PEEKB(BDA_SEGMENT, 0x0050 + GET_CURRENT_PAGE() * 2))
#define GET_CURSOR_ROW()				(PEEKB(BDA_SEGMENT, 0x0050 + GET_CURRENT_PAGE() * 2 + 1))
#define GET_CRTC_PORT()					(PEEKW(BDA_SEGMENT, 0x0063))
#define GET_MODE_SELECT_REGISTER()		(PEEKB(BDA_SEGMENT, 0x0065))



uint8_t peekb(uint16_t segment, uint16_t offset);
uint16_t peekw(uint16_t segment, uint16_t offset);
void pokeb(uint16_t segment, uint16_t offset, uint8_t val);
void pokew(uint16_t segment, uint16_t offset, uint16_t val);



static bool check_ansi_interrupt(void);
static bool check_ansi_vram(void);

uint8_t bios_get_screen_width(void);
uint8_t bios_get_screen_height(void);
uint8_t bios_get_video_mode(void);
uint8_t bios_get_current_page(void);
DosCoord bios_get_cursor_pos(void);
uint8_t bios_get_cursor_col(void);
uint8_t bios_get_cursor_row(void);

void bios_set_video_mode(uint8_t mode);
void bios_set_current_page(uint8_t page);
void bios_set_cursor_pos(uint8_t row, uint8_t col);

void vram_readc(uint8_t row, uint8_t col, uint8_t *buffer, size_t size);
void vram_reada(uint8_t row, uint8_t col, uint8_t *buffer, size_t size);
void vram_read(uint8_t row, uint8_t col, uint16_t *buffer, size_t size);
void vram_writec(uint8_t row, uint8_t col, uint8_t *buffer, size_t size);
void vram_writea(uint8_t row, uint8_t col, uint8_t *buffer, size_t size);
void vram_write(uint8_t row, uint8_t col, uint16_t *buffer, size_t size);
void vram_putc(uint8_t row, uint8_t col, uint8_t ch);
void vram_puta(uint8_t row, uint8_t col, uint8_t attr);
void vram_put(uint8_t row, uint8_t col, uint8_t ch, uint8_t attr);
void vram_putsc(uint8_t row, uint8_t col, uint8_t *str);
void vram_puts(uint8_t row, uint8_t col, uint8_t *str, uint8_t attr);


uint8_t peekb(uint16_t segment, uint16_t offset)
{
	return *(uint8_t far*)(((uint32_t)segment << 16) | offset);
}

uint16_t peekw(uint16_t segment, uint16_t offset)
{
	return *(uint16_t far*)(((uint32_t)segment << 16) | offset);
}

void pokeb(uint16_t segment, uint16_t offset, uint8_t val)
{
	*(uint8_t far*)(((uint32_t)segment << 16) | offset) = val;
}

void pokew(uint16_t segment, uint16_t offset, uint16_t val)
{
	*(uint16_t far*)(((uint32_t)segment << 16) | offset) = val;
}





uint8_t bios_get_screen_width(void)
{
	union REGS regs;
	regs.h.ah = 0x0F;
	int86(0x10, &regs, &regs);
	return regs.h.ah;
}

uint8_t bios_get_screen_height(void)
{
	union REGS regs;
	regs.x.ax = 0x1130;
	int86(0x10, &regs, &regs);
	return (regs.h.dl != 0) ? regs.h.dl+1 : 25; /* old BIOSes can return 0, so we are assume there are 25 row */
}

uint8_t bios_get_video_mode(void)
{
	union REGS regs;
	regs.h.ah = 0x0F;
	int86(0x10, &regs, &regs);
	return regs.h.al;
}

uint8_t bios_get_current_page(void)
{
	union REGS regs;
	regs.h.ah = 0x0F;
	int86(0x10, &regs, &regs);
	return regs.h.bh;
}

DosCoord bios_get_cursor_pos(void)
{
	union REGS regs = {0};
	DosCoord coord;
	regs.h.ah = 0x03;
	regs.h.bh = bios_get_current_page();
	int86(0x10, &regs, &regs);
	coord.row = regs.h.dh;
	coord.col = regs.h.dl;
	return coord;
}

uint8_t bios_get_cursor_col(void)
{
	union REGS regs = {0};
	regs.h.ah = 0x03;
	regs.h.bh = bios_get_current_page();
	int86(0x10, &regs, &regs);
	return regs.h.dl;
}

uint8_t bios_get_cursor_row(void)
{
	union REGS regs = {0};
	regs.h.ah = 0x03;
	regs.h.bh = bios_get_current_page();
	int86(0x10, &regs, &regs);
	return regs.h.dh;
}


void bios_set_video_mode(uint8_t mode)
{
	union REGS regs = {0};
	regs.h.al = mode;
	int86(0x10, &regs, &regs);
}

void bios_set_current_page(uint8_t page)
{
	union REGS regs = {0};
	regs.h.ah = 0x05;
	regs.h.al = page;
	int86(0x10, &regs, &regs);
}

void bios_set_cursor_pos(uint8_t row, uint8_t col)
{
	union REGS regs = {0};
	regs.h.ah = 0x02;
	regs.h.bh = bios_get_current_page();
	regs.h.dh = row;
	regs.h.dl = col;
	int86(0x10, &regs, &regs);
}


void vram_readc(uint8_t row, uint8_t col, uint8_t *buffer, size_t size)
{
	uint8_t far *vidmem = (uint8_t far*)VIDEO_MEMORY + GET_CURRENT_PAGE_OFFSET() + (col + (uint16_t)row * GET_SCREEN_WIDTH()) * 2U;
	size_t i = 0;
	
	assert(buffer);
	
	for(i = 0; i < size; i++)
	{
		buffer[i] = vidmem[i*2];
	}
}

void vram_reada(uint8_t row, uint8_t col, uint8_t *buffer, size_t size)
{
	uint8_t far *vidmem = (uint8_t far*)VIDEO_MEMORY + GET_CURRENT_PAGE_OFFSET() + (col + (uint16_t)row * GET_SCREEN_WIDTH()) * 2U + 1;
	size_t i = 0;
	
	assert(buffer);
	
	for(i = 0; i < size; i++)
	{
		buffer[i] = vidmem[i*2];
	}
}

void vram_read(uint8_t row, uint8_t col, uint16_t *buffer, size_t size)
{
	uint16_t far *vidmem = (uint16_t far*)VIDEO_MEMORY + GET_CURRENT_PAGE_OFFSET() + (col + (uint16_t)row * GET_SCREEN_WIDTH()) * 2U;
	size_t i = 0;
	
	assert(buffer);
	
	for(i = 0; i < size; i++)
	{
		buffer[i] = vidmem[i];
	}
}

void vram_writec(uint8_t row, uint8_t col, uint8_t *buffer, size_t size)
{
	uint8_t far *vidmem = (uint8_t far*)VIDEO_MEMORY + GET_CURRENT_PAGE_OFFSET() + (col + (uint16_t)row * GET_SCREEN_WIDTH()) * 2U;
	size_t i = 0;
	
	assert(buffer);
	
	for(i = 0; i < size; i++)
	{
		vidmem[i*2] = buffer[i];
	}
}

void vram_writea(uint8_t row, uint8_t col, uint8_t *buffer, size_t size)
{
	uint8_t far *vidmem = (uint8_t far*)VIDEO_MEMORY + GET_CURRENT_PAGE_OFFSET() + (col + (uint16_t)row * GET_SCREEN_WIDTH()) * 2U + 1;
	size_t i = 0;
	
	assert(buffer);
	
	for(i = 0; i < size; i++)
	{
		vidmem[i*2] = buffer[i];
	}
}

void vram_write(uint8_t row, uint8_t col, uint16_t *buffer, size_t size)
{
	uint16_t far *vidmem = (uint16_t far*)VIDEO_MEMORY + GET_CURRENT_PAGE_OFFSET() + (col + (uint16_t)row * GET_SCREEN_WIDTH()) * 2U;
	size_t i = 0;
	
	assert(buffer);
	
	for(i = 0; i < size; i++)
	{
		vidmem[i] = buffer[i];
	}
}

void vram_putc(uint8_t row, uint8_t col, uint8_t ch)
{
	*((uint8_t far*)VIDEO_MEMORY + GET_CURRENT_PAGE_OFFSET() + (col + (uint16_t)row * GET_SCREEN_WIDTH()) * 2U) = ch;
}

void vram_puta(uint8_t row, uint8_t col, uint8_t attr)
{
	*((uint8_t far*)VIDEO_MEMORY + GET_CURRENT_PAGE_OFFSET() + (col + (uint16_t)row * GET_SCREEN_WIDTH()) * 2U + 1) = attr;
}

void vram_put(uint8_t row, uint8_t col, uint8_t ch, uint8_t attr)
{
	*((uint8_t far*)VIDEO_MEMORY + GET_CURRENT_PAGE_OFFSET() + (col + (uint16_t)row * GET_SCREEN_WIDTH()) * 2U) = ch;
	*((uint8_t far*)VIDEO_MEMORY + GET_CURRENT_PAGE_OFFSET() + (col + (uint16_t)row * GET_SCREEN_WIDTH()) * 2U + 1) = attr;
}

void vram_putsc(uint8_t row, uint8_t col, uint8_t *str)
{
	uint8_t far *vidmem = (uint8_t far*)VIDEO_MEMORY + GET_CURRENT_PAGE_OFFSET() + (col + (uint16_t)row * GET_SCREEN_WIDTH()) * 2U;
	
	assert(str);
	
	while(*str != NULL)
	{
		*vidmem = *str++;
		vidmem += 2;
	}
}

void vram_puts(uint8_t row, uint8_t col, uint8_t *str, uint8_t attr)
{
	uint8_t far *vidmem = (uint8_t far*)VIDEO_MEMORY + GET_CURRENT_PAGE_OFFSET() + (col + (uint16_t)row * GET_SCREEN_WIDTH()) * 2U;

	assert(str);
	
	while(*str != NULL)
	{
		*vidmem++ = *str++;
		*vidmem++ = attr;
	}
}


bool is_ansi_supported(void)
{
	if(ansiSupport != ANSI_SUPPORT_UNKNOWN) /* already checked */
		return ansiSupport;
	
	if(check_ansi_interrupt() == true)
	{
		logf("[+] ANSI support detected using interrupt 2Fh.\n");
		ansiSupport = ANSI_SUPPORT_YES;
		return true;
	}
	else if(check_ansi_vram() == true) /* if not found on interrupt, check its behaviour */
	{
		logf("[+] ANSI support detected using VRAM.\n");
		ansiSupport = ANSI_SUPPORT_YES;
		return true;
	}
	else
	{
		logf("[-] ANSI support not detected! Using BIOS and VRAM instead.\n");
		ansiSupport = ANSI_SUPPORT_NO;
	}
	return false; /* ANSI driver not found */
}

bool is_utf8_supported(void)
{
	return false; /* it's . . . DOS */
}

/* clear only current page */
void clear_screen(void)
{
	if(ansiSupport == ANSI_SUPPORT_YES)
	{
		printf(ANSI_CLEAR_SCREEN);
		fflush(stdout);
	}
	else
	{
		uint16_t videoSegment = ((uint32_t)VIDEO_MEMORY >> 16) + (GET_CURRENT_PAGE_OFFSET() >> 4);
		uint16_t pageSize = GET_SCREEN_WIDTH() * GET_SCREEN_HEIGHT();
		asm
		{
			push es
			push di
			
			mov ax, videoSegment
			mov es, ax				; es = videoSegment
			xor di, di				; di = 0
			mov cx, pageSize		; cx = pageSize, loop counter
			mov ax, 0x0720			; background: black, foreground: gray, character: space
			cld						; di++  at every rep operation
			rep stosw				; [es:di] = ax, until cx = 0
			
			pop di
			pop es
		}
		bios_set_cursor_pos(0, 0);
	}
}

void 


bool init_console(void)
{
	uint8_t buffer1[64];
	uint16_t buffer2[64];
	bios_set_current_page(1);
	if(is_ansi_supported() == true)
	{
		printf("ANSI escape sequence supported\n");
	}
	else
	{
		printf("ANSI escape sequence not supported\n");
	}
	
	printf("Fast detection Lists:\n");
	printf("GET_SCREEN_WIDTH_FAST: %d\n", GET_SCREEN_WIDTH());
	printf("get_screen_width: %d\n", bios_get_screen_width());
	printf("GET_SCREEN_HEIGHT_FAST: %d\n", GET_SCREEN_HEIGHT());
	printf("get_screen_height: %d\n", bios_get_screen_height());
	printf("GET_CURRENT_PAGE_FAST: %d\n", GET_CURRENT_PAGE());
	printf("get_current_page: %d\n", bios_get_current_page());
	printf("GET_CURRENT_PAGE_OFFSET_FAST: %p\n", GET_CURRENT_PAGE_OFFSET());
	printf("GET_VIDEO_MODE_FAST: %d\n", GET_VIDEO_MODE());
	printf("get_video_mode: %d\n", bios_get_video_mode());
	printf("GET_CURSOR_COL_FAST: %d\n", GET_CURSOR_COL());
	printf("get_cursor_col: %d\n", bios_get_cursor_col());
	printf("GET_CURSOR_ROW_FAST: %d\n", GET_CURSOR_ROW());
	printf("get_cursor_row: %d\n", bios_get_cursor_row());
	vram_readc(1, 1, buffer1, 4);
	printf("(1,1) to (1,5) character at screen is: %.4s\n", buffer1);
	vram_puta(11, 0, 0x1f);
	vram_reada(11, 0, buffer1, 1);
	printf("Attribute at (11,0): %X", buffer1[0]);
	vram_puts(20, 5, "HELLO WORLD!", 0x2F);
	getchar();
	clear_screen();
	getchar();
	bios_set_current_page(0);
	
	
	return true;
}
















static bool check_ansi_interrupt(void)
{
	union REGS regs;
	regs.x.ax = 0x1A00;
	int86(0x2F, &regs, &regs);
	if(regs.h.al == 0xFF)
	{
		return true;
	}
	return false;
}

static bool check_ansi_vram(void)
{
	uint8_t orjBuffer[3];
	bool has_ansi = false;
	DosCoord orjPos;
	uint8_t videoMode = bios_get_video_mode();
	
	if(videoMode > 0x03 && videoMode != 0x07) /* if not in text mode, don't try */
	{
		return false;
	}
	
	orjPos = bios_get_cursor_pos();
	bios_set_cursor_pos(0, 0);
	
	vram_readc(0, 0, orjBuffer, 3);
	
	printf("\x1B[s");
	fflush(stdout);
	
	/* if cursor moved, that mean ANSI.SYS or similar tool is not installed */
	if(bios_get_cursor_col() > 0)
	{
		vram_writec(0, 0, orjBuffer, 3);
	}
	else
	{
		has_ansi = true;
	}
	
	bios_set_cursor_pos(orjPos.row, orjPos.col);
	return has_ansi;
}




typedef struct VbeInfoBlock /* VESA BIOS EXTENSION */
{
	char		vesaSignature[4];	/* "VESA" */
	uint16_t	vesaVersion;
	uint32_t	oemStringPtr;
	uint32_t	capabilities;
	uint32_t	videoModePtr;
	uint16_t	totalMemory;		/* 64KB unit */
	char		reserved[492];		/* Complete to 512 byte */
}VbeInfoBlock;


typedef enum GfxCard
{
	GFX_UNKNOWN,
	GFX_MDA,
	GFX_HERCULES,
	GFX_CGA,
	GFX_TANDY,
	GFX_EGA,
	GFX_MCGA,
	GFX_VGA,
	GFX_SVGA
}GfxCard;

static GfxCard get_gfx_card(void)
{
	static GfxCard gfxCard = GFX_UNKNOWN;
	if(gfxCard != GFX_UNKNOWN)
	{
		return gfxCard;
	}
	else
	{
		union REGS regs = {0};
		regs.x.ax = 0x1A00;
		int86(0x10, &regs, &regs);
		
		if(regs.h.al == 0x1A) /* VGA or better found */
		{
			uint8_t dispComb = regs.h.bl; /* Look ralf brown's interrupt list, Table 00039 (display combination) */
			
			
			/*	Some newer (Back then) EGA cards support 1A00h function even though they aren't capable as VGA.
				So we check the card is EGA or not	*/
			regs.x.ax = 0x1C00;
			regs.x.cx = 0x0001;
			int86(0x10, &regs, &regs);
			if(regs.h.al != 0x1A) /* function not supported, that mean card is EGA! */
			{
				gfxCard = GFX_EGA;
			}
			else
			{
				if(dispComb == 0x07 || dispComb == 0x08) /* 0x07 monochrome analog display, 0x08 color analog display */
				{
					/* VGA or SVGA */
					struct SREGS sregs;
					VbeInfoBlock vbeInfo;
					
					memcpy(vbeInfo.vesaSignature, "VBE2", 4);
					
					segread(&sregs);
					
					regs.x.ax = 0x4F00;
					sregs.es = FP_SEG(&vbeInfo);
					regs.x.di = FP_OFF(&vbeInfo);
					int86x(0x10, &regs, &regs, &sregs);
					
					if(regs.h.al == 0x4F) /* VESA supported */
					{
						gfxCard = GFX_SVGA;
					}
					else /* VESA not found, standard VGA */
					{
						gfxCard = GFX_VGA;
					}
				}
				if(dispComb == 0x0A || dispComb == 0x0B || dispComb == 0x0C) /* 0x0A digital color, 0x0B monochrome analog, 0x0C color analog MCGA */
				{
					gfxCard = GFX_MCGA;
					return gfxCard;
				}
			}
		}
		else /* MDA, HERCULES, CGA, EGA, or something older than VGA */
		{
			regs.h.ah = 0x12; /* ah=0x12, bl=0x10: get EGA info */
			regs.x.bx = 0xff10; /* init bh to -1 */
			regs.x.cx = 0xffff; /* init cx to -1 */
			int86(0x10, &regs, &regs);
			if((regs.x.cx == 0xffff) || (regs.h.bh == 0xff)) /* EGA not found */
			{
				if(GET_CRTC_PORT() == MONO_CRTC_INDEX_PORT)
				{
					gfxCard = GFX_MDA; /* TODO: Add detection for Hercules */
				}
				else /* not monochrome port, cga */
				{
					gfxCard = GFX_CGA;
				}
			}
			else /* EGA found */
			{
				gfxCard = GFX_EGA;
			}
		}
		return gfxCard;
	}
}

static uint16_t get_video_segment(void)
{
	
	if(GET_CRTC_PORT() == MONO_CRTC_INDEX_PORT)
	{
		return 0xB000;
	}
	else /* EGA, VGA, MCGA, SVGA */
	{
		outp();
	}
	return 0xB800;
}

#if 0
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
#endif

#if 0

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

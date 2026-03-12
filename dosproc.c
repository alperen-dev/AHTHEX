/*
* ahthex - a cross platform hex editor
* Copyright (C) 2026 alperen-dev
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*/

#include <stdio.h>
#include <dos.h>
#include <i86.h>
#include <assert.h>
#include <stdarg.h>
#include <string.h>
#include <conio.h>
#include <malloc.h>

#include "dosproc.h"
#include "console.h"
#include "log.h"

const char *GFX_CARD_NAMES[] = 
{
	"MDA",
	"Hercules",
	"CGA",
	"EGA",
	"MCGA",
	"VGA",
	"SVGA"
};



uint8_t bios_get_screen_width(void)
{
	union REGS regs = {0};
	regs.h.ah = 0x0F;
	int86(0x10, &regs, &regs);
	return regs.h.ah;
}

uint8_t bios_get_screen_height(void)
{
	union REGS regs = {0};
	regs.x.ax = 0x1130;
	int86(0x10, &regs, &regs);
	return (regs.h.dl != 0) ? regs.h.dl+1 : 25; /* old BIOSes can return 0, so we are assume there are 25 row */
}

uint8_t bios_get_video_mode(void)
{
	union REGS regs = {0};
	regs.h.ah = 0x0F;
	int86(0x10, &regs, &regs);
	return regs.h.al;
}

uint8_t bios_get_current_page(void)
{
	union REGS regs = {0};
	regs.h.ah = 0x0F;
	int86(0x10, &regs, &regs);
	return regs.h.bh;
}

void bios_get_cursor(HwCursor *hwCursor)
{
	uint16_t i = 0;
	assert(hwCursor);
	
	hwCursor->scanLines = PEEKW(BDA_SEGMENT, 0x0060);
	
	for(i = 0; i < MAX_PAGE_COUNT; i++)
	{
		hwCursor->position[i] = PEEKW(BDA_SEGMENT, 0x0050 + (i << 1));
	}
}

void bios_get_cursor_pos(uint8_t *row, uint8_t *col)
{
	union REGS regs = {0};
	regs.h.ah = 0x03;
	regs.h.bh = bios_get_current_page();
	int86(0x10, &regs, &regs);
	
	if(row != NULL)
		*row = regs.h.dh;
	if(col != NULL)
		*col = regs.h.dl;
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

void bios_set_cursor(const HwCursor *hwCursor)
{
	union REGS regs = {0};
	uint16_t i = 0;
	assert(hwCursor);
	
	regs.h.ah	= 0x01;
	regs.x.cx	= hwCursor->scanLines;
	int86(0x10, &regs, &regs);
	
	memset(&regs, 0, sizeof(union REGS));
	
	regs.h.ah	= 0x02;
	regs.h.bh	= bios_get_current_page();
	regs.h.dh	= (uint8_t)(hwCursor->position[regs.h.bh] >> 8);
	regs.h.dl	= (uint8_t)(hwCursor->position[regs.h.bh]);
	int86(0x10, &regs, &regs);
	
	for(i = 0; i < MAX_PAGE_COUNT; i++)
	{
		POKEW(BDA_SEGMENT, 0x0050 + (i << 1), hwCursor->position[i]);
	}
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


bool is_utf8_supported(void)
{
	return false; /* it's . . . DOS */
}


void vram_fill(ScreenCell far* videoMemory, ScreenCell fillValue, uint16_t count)
{
	asm
	{
		push es
		push di
		
		mov cx, count				; cx = count, loop counter
		mov ax, word ptr fillValue	; ax = fillValue
		
		les di, videoMemory
		
		cld							; di++  at every rep operation
		rep stosw					; [es:di] = ax, until cx = 0
		
		pop di
		pop es
	}
}

void vram_read(const ScreenCell far* videoMemory, ScreenCell far* buffer, uint16_t count)
{
	asm
	{
		push es
		push ds
		push di
		push si

		mov cx, count			; cx = count

		les di, buffer
		lds si, videoMemory
		

		cld						; di++ and si++  at every rep operation
		rep movsw

		pop si
		pop di
		pop ds
		pop es
    }
}

void vram_write(ScreenCell far* videoMemory, const ScreenCell far* buffer, uint16_t count)
{
	asm
	{
		push es
		push ds
		push di
		push si

		mov cx, count			; cx = count
		
		les di, videoMemory
		lds si, buffer
		

		cld						; di++ and si++  at every rep operation
		rep movsw

		pop si
		pop di
		pop ds
		pop es
    }
}

#if 0
void text_clear_screen(void)
{
	uint16_t videoSegment = FP_SEG(videoSegment) + (GET_CURRENT_PAGE_OFFSET() >> 4);
	uint16_t pageSize = GET_SCREEN_WIDTH() * GET_SCREEN_HEIGHT();
	vram_fill(videoSegment, pageSize, ' ', 0x07);
	bios_set_cursor_pos(0, 0);
}

void text_clear_entire_screen(void)
{
	uint16_t videoSegment = ((uint32_t)VIDEO_MEMORY >> 16);
	
	vram_fill(FP_SEG(videoMemory), 0, 32768, ' ', 0x07);
	
	bios_set_current_page(0);
	bios_set_cursor_pos(0, 0);
}
#endif













/* Enabled by default, disabled if graphics card doesn't have page flipping capability */
bool usePageFlipping = true;

VideoState oldState;
VideoState v; /* current video state */

ScreenCell far* firstPageMem	= NULL;
ScreenCell far* secondPageMem	= NULL;

/* 0 and 2, 2 for compatibility reason */
VideoPage firstPageNumber	= 0;
VideoPage secondPageNumber	= 2;

/* 4096 by default, user can change this settings from settings menu [0-65535] (0 mean disabled), or 0 automatically if buffer allocation failed */
size_t oldVideoBufferSize = 4096U;


#define VIDEO_TEXT_COLOR	0x03
#define VIDEO_TEXT_MONO		0x07

static GfxCard get_gfx_card(void)
{
	static GfxCard gfxCard = GFX_UNKNOWN;
	union REGS regs = {0};
	
	if(gfxCard != GFX_UNKNOWN)
		return gfxCard;
	
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
		if(regs.h.al != 0x1C) /* function not supported, that mean card is EGA! */
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
				
				memcpy(vbeInfo.vesaSignature, "VBE2", 4); /* VESA 2.0 request signature "VBE2", required to recieve version 2.0 info */
				
				
				
				regs.x.ax = 0x4F00;
				sregs.es = FP_SEG(&vbeInfo);
				regs.x.di = FP_OFF(&vbeInfo);
				segread(&sregs);
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
			if(GET_CRTC_PORT() == PORT_MONO_CRTC_INDEX)
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

static ScreenCell far *get_video_memory(void)
{
	if(GET_CRTC_PORT() == PORT_MONO_CRTC_INDEX)
	{
		return 0xB0000000;
	}
	else /* EGA, VGA, MCGA, SVGA */
	{
		if(GET_VIDEO_MODE() > 8)
			return 0xA0000000;
	}
	return 0xB8000000;
}


VideoPage get_other_page(VideoPage currPage)
{
	if(currPage == firstPageNumber)
	{
		return secondPageNumber;
	}
	/* return firstPage if not currently in firstPage (for speed) */
	return firstPageNumber;
}

ScreenCell far *get_page_memory(VideoMode page)
{
	if(page == firstPageNumber)
	{
		return firstPageMem;
	}
	else if(page == secondPageNumber)
	{
		return secondPageMem;
	}
	return NULL;
}

bool flip_page(void)
{
	VideoPage page = 0;
	ScreenCell far *mem = NULL;
	if(usePageFlipping == false)
		return false;
	page	= get_other_page(v.page);
	if((mem	= get_page_memory(page)) == NULL)
	{
		return false;
	}
	v.page		= page;
	v.videoMem	= mem;
	bios_set_current_page(v.page);
	return true;
}

bool update(void)
{
	if(v.buffer == NULL)
		return false;
	if(usePageFlipping)
	{
		ScreenCell far* otherMem = get_page_memory(get_other_page(v.page));
		logf("%u bytes write from %04X:%04X to %04X:%04X\n", v.bufferLength, FP_SEG(v.buffer), FP_OFF(v.buffer), FP_SEG(v.videoMem), FP_OFF(v.videoMem));
		vram_write(otherMem, v.buffer, v.bufferLength);
		flip_page();
	}
	else
	{
		logf("%u bytes write from %04X:%04X to %04X:%04X\n", v.bufferLength, FP_SEG(v.buffer), FP_OFF(v.buffer), FP_SEG(v.videoMem), FP_OFF(v.videoMem));
		vram_write(v.videoMem, v.buffer, v.bufferLength);
	}
	return true;
}

void put_char(uint16_t row, uint16_t col, char ch)
{
	v.buffer[row * GET_SCREEN_WIDTH() + col].ch = ch;
}

void put_color(uint16_t row, uint16_t col, uint8_t attr)
{
	v.buffer[row * GET_SCREEN_WIDTH() + col].attr = attr;
}

void put_cell(uint16_t row, uint16_t col, ScreenCell cell)
{
	v.buffer[row * GET_SCREEN_WIDTH() + col] = cell;
}

void put_str(uint16_t row, uint16_t col, char *str)
{
	size_t index = row * GET_SCREEN_WIDTH() + col;
	while(index < v.bufferLength && *str != '\0')
	{
		v.buffer[index++].ch = *str++;
	}
}

void put_cstr(uint16_t row, uint16_t col, char *str, uint8_t attr)
{
	size_t index = row * GET_SCREEN_WIDTH() + col;
	while(index < v.bufferLength && *str != '\0')
	{
		v.buffer[index].ch = *str++;
		v.buffer[index++].attr = attr;
	}
}


void get_video_state(VideoState *videoState)
{
	assert(videoState);
	
	videoState->card		= get_gfx_card();
	videoState->videoMem	= get_video_memory();
	
	if(videoState->bufferLength > 0)
	{
		videoState->buffer			= (ScreenCell far*)farcalloc(videoState->bufferLength, sizeof(ScreenCell));
		if(videoState->buffer != NULL)
		{
			vram_read(videoState->videoMem, videoState->buffer, videoState->bufferLength);
		}
		else
		{
			videoState->bufferLength	= 0;
		}
	}
	else
	{
		videoState->buffer			= NULL;
	}
	videoState->mode			= bios_get_video_mode();
	videoState->page			= bios_get_current_page();
	bios_get_cursor(&videoState->cur);
}

bool set_video_state(VideoState *videoState)
{
	if(videoState == NULL)
		return false;
	
	bios_set_video_mode((uint8_t)videoState->mode);
	if(videoState->buffer != NULL)
	{
		vram_write(videoState->videoMem, videoState->buffer, videoState->bufferLength);
	}
	bios_set_current_page(videoState->page);
	bios_set_cursor(&videoState->cur);
	return true;
}

void log_video_state(VideoState *videoState)
{
	uint16_t i = 0;
	if(videoState == NULL)
		return;
	logf("[+] Video Card: %s\n", GFX_CARD_NAMES[videoState->card]);
	logf("[+] Video Memory: %04X:%04X\n", FP_SEG(videoState->videoMem), FP_OFF(videoState->videoMem));
	logf("[+] Video Buffer: %04X:%04X\n", FP_SEG(videoState->buffer), FP_OFF(videoState->buffer));
	logf("[+] Video Buffer Length: %u\n", videoState->bufferLength);
	logf("[+] Video Mode: %u\n", videoState->mode);
	logf("[+] Current Page: %u\n", (uint16_t)videoState->page);
	logf("[+] Cursor Scan Line Start: %02X, End: %02X\n", videoState->cur.scanLines >> 8, videoState->cur.scanLines & 0x00FF);
	for(i = 0; i < MAX_PAGE_COUNT; i++)
	{
		logf("[+] Cursor Position in page %u is Row: %u, Column: %u\n", i, videoState->cur.position[i] >> 8, videoState->cur.position[i] & 0x00FF);
	}
}



bool console_init(void)
{
	v.card = get_gfx_card();
	
	
	/* Check video card */
	if(v.card == GFX_UNKNOWN)
	{
		logf("[-] Graphics card not detected!\n");
		return false;
	}
	else
	{
		logf("[+] Graphics type: %s\n", GFX_CARD_NAMES[v.card]);
	}
	
	logf("[?] Attempting to create double buffering memory . . .\n");
	v.bufferLength		= GET_PAGE_SIZE()>>1;
	if((v.buffer = farcalloc(v.bufferLength, sizeof(ScreenCell))) == NULL)
	{
		logf("[-] Double buffering memory creation failed!\n");
		return false;
	}
	else
	{
		logf("[+] Double buffering memory creation succeeded (%u byte).\n", v.bufferLength * sizeof(ScreenCell));
	}
	
	
	/* save old video state */
	oldState.bufferLength = oldVideoBufferSize>>1;
	get_video_state(&oldState);
	/*log_video_state(&oldState);*/
	
	
	if(v.card <= GFX_HERCULES)
	{
		v.mode = VIDEO_TEXT_MONO;
		if(usePageFlipping == true)
		{
			logf("[-] Page flipping disabled due to hardware limitation.\n");
			usePageFlipping = false;
		}
	}
	else
	{
		v.mode = VIDEO_TEXT_COLOR;
	}
	bios_set_video_mode(v.mode);
	logf("[+] Video mode set as %d\n", v.mode);
	
	/* test pages, if page flipping enabled */
	if(usePageFlipping)
	{
		bios_set_current_page(firstPageNumber);
		firstPageMem = get_video_memory() + GET_CURRENT_PAGE_OFFSET();
		bios_set_current_page(secondPageNumber);
		secondPageMem = get_video_memory() + (GET_CURRENT_PAGE_OFFSET()>>1);
		
		logf("[+] %uth page memory: %04X:%04X\n", firstPageNumber, FP_SEG(firstPageMem), FP_OFF(firstPageMem));
		logf("[+] %uth page memory: %04X:%04X\n", secondPageNumber, FP_SEG(secondPageMem), FP_OFF(secondPageMem));
	}
	else
	{
		firstPageMem = get_video_memory() + GET_CURRENT_PAGE_OFFSET();
		secondPageMem = firstPageMem;
	}
	
	
	v.page 			= firstPageNumber;
	v.cur.scanLines	= CURSOR_INVISIBLE;
	
	
	bios_set_current_page(v.page);
	bios_set_cursor(&v.cur);
	
	v.videoMem			= firstPageMem;
	
	log_video_state(&v);
	
	
	return true;
}

bool console_close(void)
{
	bool retVal = set_video_state(&oldState);
	if(v.buffer != NULL)
	{
		logf("[+] Video buffer freed.\n");
		farfree(v.buffer);
		v.buffer = NULL;
		v.bufferLength = 0;
	}
	
	if(oldState.buffer != NULL)
	{
		farfree(oldState.buffer);
		oldState.buffer = NULL;
		oldState.bufferLength = 0;
	}
	return retVal;
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

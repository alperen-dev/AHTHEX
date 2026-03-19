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

#ifndef DOSPROC_H
#define DOSPROC_H

#include "ahtdefs.h"
#include <dos.h>

#if defined(COMP_WATCOM)
	#include <malloc.h>
	#define farmalloc	_fmalloc
	#define farcalloc	_fcalloc
	#define farrealloc	_frealloc
	#define farfree		_ffree
#endif

#define BDA_SEGMENT	0x0040U


/* monochrome ports */
#define PORT_MONO_CRTC_INDEX			0x03B4
#define PORT_MONO_CRTC_DATA				0x03B5

/* VGA ports */
#define PORT_ATTRIBUTE_CONTROLLER_INDEX	0x03C0	/* read/write index, write data, to reset index/data sequence, Color: in(0x03DA), Mono: in(0x03BA) */
#define PORT_ATTRIBUTE_CONTROLLER_DATA	0x03C1	/* read only register (data) */

#define PORT_MISCELLANEOUS_INPUT_STATUS	0x03C2	/* read only register (status) */	

#define PORT_MISCELLANEOUS_OUTPUT_WRITE	0x03C2	/* write only register (miscelleneous) */
#define PORT_MISCELLANEOUS_OUTPUT_READ	0x03CC	/* read only register (miscelleneous) */

#define PORT_SEQUENCER_INDEX			0x03C4
#define PORT_SEQUENCER_DATA				0x03C5

#define PORT_GDC_INDEX					0x03CE	/* GDC: Graphics Data Controller */
#define PORT_GDC_DATA					0x03CF	/* GDC: Graphics Data Controller */

#define PORT_COLOR_CRTC_INDEX			0x03D4
#define PORT_COLOR_CRTC_DATA			0x03D5


#define CURSOR_INVISIBLE				0x2000
#define CURSOR_VISIBLE					0x0607

#define MAX_PAGE_COUNT					8


#define PEEKB(s, o) (*((uint8_t far*)MK_FP(s, o)))
#define PEEKW(s, o) (*((uint16_t far*)MK_FP(s, o)))
#define POKEB(s, o, v) (*((uint8_t far*)MK_FP(s, o)) = (v))
#define POKEW(s, o, v) (*((uint16_t far*)MK_FP(s, o)) = (v))

#define GET_SCREEN_WIDTH()				(PEEKB(BDA_SEGMENT, 0x004A))
#define GET_SCREEN_HEIGHT()				(PEEKB(BDA_SEGMENT, 0x0084) + 1)
#define GET_CURRENT_PAGE()				(PEEKB(BDA_SEGMENT, 0x0062))
#define GET_PAGE_SIZE()					(PEEKW(BDA_SEGMENT, 0x004C))
#define GET_CURRENT_PAGE_OFFSET()		(PEEKW(BDA_SEGMENT, 0x004E))
#define GET_VIDEO_MODE()				(PEEKB(BDA_SEGMENT, 0x0049))
#define GET_CURSOR_COL()				(PEEKB(BDA_SEGMENT, 0x0050 + GET_CURRENT_PAGE() * 2))
#define GET_CURSOR_ROW()				(PEEKB(BDA_SEGMENT, 0x0050 + GET_CURRENT_PAGE() * 2 + 1))
#define GET_CRTC_PORT()					(PEEKW(BDA_SEGMENT, 0x0063))
#define GET_MODE_SELECT_REGISTER()		(PEEKB(BDA_SEGMENT, 0x0065))


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
	GFX_UNKNOWN = -1,
	GFX_MDA = 0,
	GFX_HERCULES,
	GFX_CGA,
	GFX_EGA,
	GFX_MCGA,
	GFX_VGA,
	GFX_SVGA
}GfxCard;

#pragma pack(push, 1)

typedef struct ScreenCell
{
	uint8_t		ch;
	uint8_t		attr;
}ScreenCell;

#pragma pack(pop)

typedef uint16_t	VideoMode;
typedef uint8_t		VideoPage;


typedef struct HwCursor
{
	uint16_t	position[MAX_PAGE_COUNT];
	uint16_t	scanLines;
}HwCursor;


typedef struct VideoState
{
	GfxCard 		card;
	ScreenCell far* videoMem;
	
	ScreenCell far*	buffer;
	uint16_t		bufferLength;
	
	VideoMode		mode;
	VideoPage		page;
	HwCursor		cur;
	
}VideoState;


extern const char *GFX_CARD_NAMES[];




#endif /* DOSPROC_H */

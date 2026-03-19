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

#ifndef CONSOLE_H
#define CONSOLE_H

#include "ahtdefs.h"

#if defined(OS_DOS)
	#include "dosproc.h"
#elif defined(OS_WIN)
	#include "winproc.h"
#elif defined(OS_UNIX)
	#include "unixproc.h"
#endif

typedef struct VirtualCursor
{
	int16_t		x;	/* column */
	int16_t		y;	/* row */
}VirtualCursor;

/* Box characters */
/* ANSI */
#define ANSI_DOUBLY_TOP_LEFT		"\xC9" /* 201 */
#define ANSI_DOUBLY_TOP_RIGHT		"\xBB" /* 187 */
#define ANSI_DOUBLY_BOTTOM_LEFT		"\xC8" /* 200 */
#define ANSI_DOUBLY_BOTTOM_RIGHT	"\xBC" /* 188 */
#define ANSI_DOUBLY_VERTICAL		"\xBA" /* 186 */
#define ANSI_DOUBLY_HORIZONTAL		"\xCD" /* 205 */

#define ANSI_DOUBLY_TOP_CROSS		"\xCB" /* 203 */
#define ANSI_DOUBLY_BOTTOM_CROSS	"\xCA" /* 202 */
#define ANSI_DOUBLY_LEFT_CROSS		"\xCC" /* 204 */
#define ANSI_DOUBLY_RIGHT_CROSS		"\xB9" /* 185 */
#define ANSI_DOUBLY_CROSS			"\xCE" /* 206 */

#define ANSI_SINGLY_TOP_LEFT		"\xDA" /* 218 */
#define ANSI_SINGLY_TOP_RIGHT		"\xBF" /* 191 */
#define ANSI_SINGLY_BOTTOM_LEFT		"\xC0" /* 192 */
#define ANSI_SINGLY_BOTTOM_RIGHT	"\xD9" /* 217 */
#define ANSI_SINGLY_VERTICAL		"\xB3" /* 179 */
#define ANSI_SINGLY_HORIZONTAL		"\xC4" /* 196 */

#define ANSI_SINGLY_TOP_CROSS		"\xC2" /* 194 */
#define ANSI_SINGLY_BOTTOM_CROSS	"\xC1" /* 193 */
#define ANSI_SINGLY_LEFT_CROSS		"\xC3" /* 195 */
#define ANSI_SINGLY_RIGHT_CROSS		"\xB4" /* 180 */
#define ANSI_SINGLY_CROSS			"\xC5" /* 197 */

/* UTF-8 */
#define UTF_DOUBLY_TOP_LEFT		"\xE2\x95\x94" /* ╔ */
#define UTF_DOUBLY_TOP_RIGHT	"\xE2\x95\x97" /* ╗ */
#define UTF_DOUBLY_BOTTOM_LEFT	"\xE2\x95\x9A" /* ╚ */
#define UTF_DOUBLY_BOTTOM_RIGHT	"\xE2\x95\x9D" /* ╝ */
#define UTF_DOUBLY_VERTICAL		"\xE2\x95\x91" /* ║ */
#define UTF_DOUBLY_HORIZONTAL	"\xE2\x95\x90" /* ═ */

#define UTF_DOUBLY_TOP_CROSS	"\xE2\x95\xA6" /* ╦ */
#define UTF_DOUBLY_BOTTOM_CROSS	"\xE2\x95\xA9" /* ╩ */
#define UTF_DOUBLY_LEFT_CROSS	"\xE2\x95\xA0" /* ╠ */
#define UTF_DOUBLY_RIGHT_CROSS	"\xE2\x95\xA3" /* ╣ */
#define UTF_DOUBLY_CROSS		"\xE2\x95\xAC" /* ╬ */

#define UTF_SINGLY_TOP_LEFT		"\xE2\x94\x8C" /* ┌ */
#define UTF_SINGLY_TOP_RIGHT	"\xE2\x94\x90" /* ┐ */
#define UTF_SINGLY_BOTTOM_LEFT	"\xE2\x94\x94" /* └ */
#define UTF_SINGLY_BOTTOM_RIGHT	"\xE2\x94\x98" /* ┘ */
#define UTF_SINGLY_VERTICAL		"\xE2\x94\x82" /* │ */
#define UTF_SINGLY_HORIZONTAL	"\xE2\x94\x80" /* ─ */

#define UTF_SINGLY_TOP_CROSS	"\xE2\x94\xAC" /* ┬ */
#define UTF_SINGLY_BOTTOM_CROSS	"\xE2\x94\xB4" /* ┴ */
#define UTF_SINGLY_LEFT_CROSS	"\xE2\x94\x9C" /* ├ */
#define UTF_SINGLY_RIGHT_CROSS	"\xE2\x94\xA4" /* ┤ */
#define UTF_SINGLY_CROSS		"\xE2\x94\xBC" /* ┼ */


/* Keyboard map */
#if defined(OS_DOS)
	#define KB_ESC 27
	#define KB_CONTROL 0
	#define KB_UP 72
	#define KB_DOWN 80
	#define KB_LEFT 75
	#define KB_RIGHT 77
	#define KB_RETURN 13
	#define KB_CTRL_RETURN 10
	#define KB_PG_UP 73
	#define KB_PG_DOWN 81
	#define KB_HOME 71
	#define KB_END 79
	#define KB_CTRL_F 6
	#define KB_CTRL_RIGHT 116
	#define KB_CTRL_LEFT 115
	#define KB_CTRL_S 19
	#define KB_ALT_S 31
	#define KB_CTRL_X 24
	#define KB_CTRL_N 14
	#define KB_CTRL_O 15
	#define KB_ALT_O 24
	#define KB_BACKSPACE 8
	#define KB_DEL 83 /* control */
#elif defined(OS_WIN)
	#define KB_ESC 27
	#define KB_CONTROL 224
	#define KB_UP 72
	#define KB_DOWN 80
	#define KB_LEFT 75
	#define KB_RIGHT 77
	#define KB_RETURN 13
	#define KB_CTRL_RETURN 10
	#define KB_PG_UP 73
	#define KB_PG_DOWN 81
	#define KB_HOME 71
	#define KB_END 79
	#define KB_CTRL_F 6
	#define KB_CTRL_RIGHT 116
	#define KB_CTRL_LEFT 115
	#define KB_CTRL_S 19
	#define KB_ALT_S 115
	#define KB_CTRL_X 24
	#define KB_CTRL_N 14
	#define KB_CTRL_O 15
	#define KB_ALT_O 111
	#define KB_BACKSPACE 8
	#define KB_DEL 83 /* control */
#endif

#define ANSI_CLEAR_SCREEN "\x1B[2J"



typedef enum AnsiSupport
{
	ANSI_SUPPORT_UNKNOWN,
	ANSI_SUPPORT_YES,
	ANSI_SUPPORT_NO
}AnsiSupport;

extern AnsiSupport ansiSupport;

extern bool console_init(void);
extern bool console_close(void);

extern void put_char(uint16_t row, uint16_t col, char ch);
extern void put_color(uint16_t row, uint16_t col, uint8_t attr);
extern void put_cell(uint16_t row, uint16_t col, ScreenCell cell);
extern void put_str(uint16_t row, uint16_t col, char *str);
extern void put_cstr(uint16_t row, uint16_t col, char *str, uint8_t attr);
extern bool update(void);

#endif /* CONSOLE_H */

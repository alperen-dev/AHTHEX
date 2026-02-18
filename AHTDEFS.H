/*
	Name: AHTDEFS.H
	Copyright: All Rights Free.
	Author: AHT
	Created date: 30.05.2024 17.14
	Modify date: 05.10.2025 18.57
	Description: Types and defines.
*/

#ifndef _AHTDEFS_H_
#define _AHTDEFS_H_

/* Detect Processor architecture */
#if defined(__x86_64__) || defined(_M_X64)
	#define ARCH_X64
	#define BITS_64
#elif defined(__386__) || defined(__i386__) || defined(_M_IX86)
	#define ARCH_X86
	#define BITS_32
#elif defined(__I86__) || defined(_M_I86)
	#define ARCH_X86_16
	#define BITS_16
#elif defined(__aarch64__) || defined(_M_ARM64)
	#define ARCH_ARM64
	#define BITS_64
#elif defined(__arm__) || defined(_M_ARM)
	#define ARCH_ARM
	#define BITS_32
#else
	#define ARCH_UNKNOWN
	#warning Unknown CPU architecture
#endif
/* Detecting processor architecture ended */

/* Detect OS */
#if defined(__MSDOS__) || defined(MSDOS) || defined(__DOS__)
	#define OS_DOS
	#if defined(BITS_16)
		#define OS_DOS_16
	#elif defined(BITS_32)
		#define OS_DOS_32
	#endif
#elif defined(__WINDOWS__) || defined(_WINDOWS) || defined(__WIN32__) || defined(__WIN64__) || defined(WIN32) || defined(WIN64) || defined(__NT__) || defined(_WIN32)
	#define OS_WIN
	#if defined(__WIN64__) || defined (WIN64) || defined(_WIN64)
		#define OS_WIN_64
	#elif defined(__WIN32__) || defined(WIN32) || defined(_WIN32)
		#define OS_WIN_32
	#else
		#define OS_WIN_16
	#endif
#elif defined(__unix__) || defined(__UNIX__) || defined(__LINUX__) || defined (__linux__) || defined(__APPLE__) || defined(__FreeBSD__)
	#define OS_UNIX
#else
	#error Add your OS flag to one of the above
#endif
/* OS detection end */

#define CONST const

#ifdef OS_DOS_16
	#define FAR far
	#define NEAR near
	#define HUGE huge
#else
	#define FAR
	#define NEAR
	#define HUGE
#endif

/* define bool */
#if !defined(__cplusplus) 
	#if (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L) || (defined(_MSC_VER) && _MSC_VER >= 1800)
		#include <stdbool.h>
	#elif !defined(bool) && !defined(__bool_true_false_are_defined)
		#define bool unsigned char
		#define true 1
		#define false 0
		#define __bool_true_false_are_defined 1
	#endif
#endif
/* defining bool ended */

/* TUI */
#if defined(OS_DOS)
	#define DOUBLY_TOP_LEFT 	((unsigned char)201)
	#define DOUBLY_TOP_RIGHT 	((unsigned char)187)
	#define DOUBLY_BOTTOM_LEFT 	((unsigned char)200)
	#define DOUBLY_BOTTOM_RIGHT ((unsigned char)188)
	#define DOUBLY_VERTICAL		((unsigned char)186)
	#define DOUBLY_HORIZONTAL	((unsigned char)205)
	
	#define DOUBLY_TOP_CROSS	((unsigned char)203)
	#define DOUBLY_BOTTOM_CROSS	((unsigned char)202)
	#define DOUBLY_LEFT_CROSS	((unsigned char)204)
	#define DOUBLY_RIGHT_CROSS	((unsigned char)185)
	#define DOUBLY_CROSS		((unsigned char)206)
	
	#define SINGLY_TOP_LEFT 	((unsigned char)218)
	#define SINGLY_TOP_RIGHT 	((unsigned char)191)
	#define SINGLY_BOTTOM_LEFT 	((unsigned char)192)
	#define SINGLY_BOTTOM_RIGHT ((unsigned char)217)
	#define SINGLY_VERTICAL		((unsigned char)179)
	#define SINGLY_HORIZONTAL	((unsigned char)196)
	
	#define SINGLY_TOP_CROSS	((unsigned char)194)
	#define SINGLY_BOTTOM_CROSS	((unsigned char)193)
	#define SINGLY_LEFT_CROSS	((unsigned char)195)
	#define SINGLY_RIGHT_CROSS	((unsigned char)180)
	#define SINGLY_CROSS		((unsigned char)197)
	
#elif defined OS_WIN
	#define DOUBLY_TOP_LEFT 	((unsigned char)201)
	#define DOUBLY_TOP_RIGHT 	((unsigned char)187)
	#define DOUBLY_BOTTOM_LEFT 	((unsigned char)200)
	#define DOUBLY_BOTTOM_RIGHT ((unsigned char)188)
	#define DOUBLY_VERTICAL		((unsigned char)186)
	#define DOUBLY_HORIZONTAL	((unsigned char)205)
	
	#define DOUBLY_TOP_CROSS	((unsigned char)203)
	#define DOUBLY_BOTTOM_CROSS	((unsigned char)202)
	#define DOUBLY_LEFT_CROSS	((unsigned char)204)
	#define DOUBLY_RIGHT_CROSS	((unsigned char)185)
	#define DOUBLY_CROSS		((unsigned char)206)
	
	#define SINGLY_TOP_LEFT 	((unsigned char)218)
	#define SINGLY_TOP_RIGHT 	((unsigned char)191)
	#define SINGLY_BOTTOM_LEFT 	((unsigned char)192)
	#define SINGLY_BOTTOM_RIGHT ((unsigned char)217)
	#define SINGLY_VERTICAL		((unsigned char)179)
	#define SINGLY_HORIZONTAL	((unsigned char)196)
	
	#define SINGLY_TOP_CROSS	((unsigned char)194)
	#define SINGLY_BOTTOM_CROSS	((unsigned char)193)
	#define SINGLY_LEFT_CROSS	((unsigned char)195)
	#define SINGLY_RIGHT_CROSS	((unsigned char)180)
	#define SINGLY_CROSS		((unsigned char)197)
#endif

#endif /* _AHTDEFS_H_ */

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

// define true-false value

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef true
#define true 1
#endif

#ifndef false
#define false 0
#endif

// define true-false end

#define CONST const

#ifdef __MSDOS__
	#define FAR huge
	#define NEAR near
	#define HUGE huge
#else
	#define far
	#define FAR
	#define NEAR
	#define HUGE
#endif

// define bool
#ifndef __cplusplus
	#if __STDC_VERSION__ >= 199901L
		#include <stdbool.h>
	#elif !defined bool
		typedef int bool;
		#define true  1
		#define false 0
	#endif
#endif

typedef unsigned char 		UCHAR;

#ifdef __MSDOS__
typedef char				CHAR;
typedef int 				INT;
typedef long 				LONG;
typedef short				SHORT;
typedef void 				VOID;
typedef float				FLOAT;

typedef const char 			CCHAR;

typedef unsigned int		UINT;
typedef unsigned long 		ULONG;
typedef unsigned short		USHORT;

typedef unsigned long 		DWORD;
typedef unsigned int 		WORD;
typedef unsigned char 		BYTE;

typedef int 				BOOL;
typedef int 				BOOLEAN;

typedef unsigned char		COLORREF; // ???
typedef long 				LRESULT;

typedef void (FAR *FARPROC)(void);
typedef void (NEAR *PROC)(void);

typedef BOOL* 				PBOOL;
typedef BOOLEAN* 			PBOOLEAN;
typedef CCHAR*				PCCH, PCSTR;
typedef CHAR* 				PCH, PCHAR, PSTR, PSZ;
typedef FLOAT*				PFLOAT;
typedef INT* 				PINT;
typedef LONG* 				PLONG;
typedef SHORT*				PSHORT;
typedef VOID* 				PVOID;
typedef DWORD*				PDWORD;
typedef WORD* 				PWORD;
typedef BYTE* 				PBYTE;
typedef UCHAR*				PUCHAR;
typedef UINT* 				PUINT;
typedef ULONG*				PULONG;
typedef USHORT* 			PUSHORT;

typedef FAR BOOL* 			LPBOOL;
typedef FAR CCHAR* 			LPCCH, LPCSTR;
typedef FAR CHAR* 			LPCH;
typedef FAR COLORREF* 		LPCOLORREF;
typedef FAR DWORD* 			LPDWORD;
typedef FAR INT* 			LPINT;
typedef FAR LONG* 			LPLONG;
typedef FAR CHAR* 			LPSTR;
typedef FAR VOID* 			LPVOID;
typedef FAR WORD* 			LPWORD;
typedef FAR BYTE* 			LPBYTE;
#endif // __MSDOS__

// TUI
#ifdef __MSDOS__
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
	
#elif defined __WINDOWS__ || defined __WIN32__ || defined __WIN64__ || defined WIN32 || defined WIN64
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

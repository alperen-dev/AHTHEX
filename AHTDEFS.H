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

#define TRUE 1
#define FALSE 0

#define CONST const

#ifdef __MSDOS__
	#define FAR huge
	#define NEAR near
	#define HUGE huge
#else
	#define FAR
	#define NEAR
	#define HUGE
#endif

typedef char				CHAR;
typedef int 				INT;
typedef long 				LONG;
typedef short				SHORT;
typedef void 				VOID;
typedef float				FLOAT;

typedef const char 			CCHAR;

typedef unsigned char 		UCHAR;
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

typedef NEAR BOOL* 			PBOOL;
typedef NEAR BOOLEAN* 		PBOOLEAN;
typedef NEAR CCHAR*			PCCH, PCSTR;
typedef NEAR CHAR* 			PCH, PCHAR, PSTR, PSZ;
typedef NEAR FLOAT*			PFLOAT;
typedef NEAR INT* 			PINT;
typedef NEAR LONG* 			PLONG;
typedef NEAR SHORT*			PSHORT;
typedef NEAR VOID* 			PVOID;
typedef NEAR DWORD*			PDWORD;
typedef NEAR WORD* 			PWORD;
typedef NEAR BYTE* 			PBYTE;
typedef NEAR UCHAR*			PUCHAR;
typedef NEAR UINT* 			PUINT;
typedef NEAR ULONG*			PULONG;
typedef NEAR USHORT* 		PUSHORT;

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

#endif /* _AHTDEFS_H_ */

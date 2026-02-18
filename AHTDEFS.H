#ifndef __AHTDEFS_H__
#define __AHTDEFS_H__

#include <limits.h>

/* Detect Processor architecture */
#if defined(__x86_64__) || defined(_M_X64)
	#define ARCH_X64
	#define BITS_64
#elif defined(__I86__) || defined(_M_I86) || defined(__TURBOC__) /* Borland's DOS compiler */
	#define ARCH_X86_16
	#define BITS_16
#elif defined(__386__) || defined(__i386__) || defined(_M_IX86)
	#define ARCH_X86
	#define BITS_32
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



/* Detect OS and pointer size */
#if defined(__MSDOS__) || defined(MSDOS) || defined(__DOS__)
	#define OS_DOS
	#if defined(BITS_16)
		#define OS_DOS_16
		#if defined(__HUGE__) || defined(__LARGE__) || defined(__COMPACT__)
			#define PTR_SIZE 4
		#elif defined(__MEDIUM__) || defined(__SMALL__) || defined(__TINY__)
			#define PTR_SIZE 2
		#endif
	#elif defined(BITS_32)
		#define OS_DOS_32
		#define PTR_SIZE 4
	#endif
#elif defined(__WINDOWS__) || defined(_WINDOWS) || defined(__WIN32__) || defined(__WIN64__) || defined(WIN32) || defined(WIN64) || defined(__NT__) || defined(_WIN32)
	#define OS_WIN
	#if defined(__WIN64__) || defined (WIN64) || defined(_WIN64)
		#define OS_WIN_64
		#define PTR_SIZE 8
	#elif defined(__WIN32__) || defined(WIN32) || defined(_WIN32)
		#define OS_WIN_32
		#define PTR_SIZE 4
	#elif defined(_WINDOWS) && defined(BITS_16) /* Watcom */
		#define OS_WIN_16
		#if defined(__HUGE__) || defined(__LARGE__) || defined(__COMPACT__)
			#define PTR_SIZE 4
		#elif defined(__MEDIUM__) || defined(__SMALL__) || defined(__TINY__)
			#define PTR_SIZE 2
		#endif
	#endif
#elif defined(__unix__) || defined(__UNIX__) || defined(__LINUX__) || defined (__linux__) || defined(__APPLE__) || defined(__FreeBSD__)
	#define OS_UNIX
	#if defined(BITS_64)
		#define PTR_SIZE 8
	#elif defined(BITS_32)
		#define PTR_SIZE 4
	#elif defined(BITS_16)
		#define PTR_SIZE 2
	#endif
#else
	#error Add your OS flag to one of the above
#endif
/* OS and pointer size detection end */



#if defined(OS_DOS_16) || defined(OS_WIN_16)
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
	#if (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L) || (defined(_MSC_VER) && _MSC_VER >= 1800) /* VS 2013 */
		#include <stdbool.h>
	#elif !defined(bool) && !defined(__bool_true_false_are_defined)
		#define bool 							unsigned char
		#define true							1
		#define false							0
		#define __bool_true_false_are_defined	1
	#endif
#endif
/* defining bool ended */

/* define standart types */
#if (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L) || (defined(_MSC_VER) && _MSC_VER >= 1600) /* VS 2010 */
	#include <stdint.h>
#else
	typedef signed char					int8_t;
	#define INT8_MAX					(127)
	#define INT8_MIN					(-128)
	
	typedef unsigned char				uint8_t;
	#define UINT8_MAX					(255)
	
	typedef signed short				int16_t;
	#define INT16_MAX					(32767)
	#define INT16_MIN					(-32767-1)
	
	typedef unsigned short				uint16_t;
	#if defined(BITS_16)
		#define UINT16_MAX				(65535U)
	#elif defined(BITS_32) || defined(BITS_64)
		#define UINT16_MAX				(65535)
	#endif
	
	#if defined(BITS_16)
		typedef signed long				int32_t;
		#define INT32_MAX				(2147483647L)
		#define INT32_MIN				(-2147483647L-1)
		
		typedef unsigned long			uint32_t;
		#define UINT32_MAX				(4294967295UL)
	#elif defined(BITS_32) || defined(BITS_64)
		typedef signed int				int32_t;
		#define INT32_MAX				(2147483647)
		#define INT32_MIN				(-2147483647-1)
		
		typedef unsigned int			uint32_t;
		#define UINT32_MAX				(4294967295U)
	#endif
	
	#if defined(_MSC_VER) && (_MSC_VER >= 1200) /* VS 6.0 (1998) */
		typedef __int64					int64_t;
		#define INT64_MIN				(-9223372036854775807i64-1)
		#define INT64_MAX				(9223372036854775807i64)
		
		typedef unsigned __int64		uint64_t;
		#define HAS_INT64
	#elif (defined(__WATCOMC__) && __WATCOMC__ >= 1100) || (defined(__GNUC__) && __GNUC__ >= 2)
		typedef signed long long		int64_t;
		#define INT64_MIN				(-9223372036854775807LL-1)
		#define INT64_MAX				(9223372036854775807LL)
		
		typedef unsigned long long		uint64_t;
		#define UINT64_MAX				(18446744073709551615ULL)
		#define HAS_INT64
	#endif
	
	/* set intmax_t as int64_t if platform support 64 bit integer */
	#if defined(HAS_INT64)
		
		typedef int64_t			intmax_t;
		#define INTMAX_MAX		INT64_MAX
		#define INTMAX_MIN		INT64_MIN
		
		typedef uint64_t		uintmax_t;
		#define UINTMAX_MAX		UINT64_MAX
	#else
		typedef int32_t			intmax_t;
		#define INTMAX_MAX		INT32_MAX
		#define INTMAX_MIN		INT32_MIN
		
		typedef uint32_t		uintmax_t;
		#define UINTMAX_MAX		UINT32_MAX
	#endif
	
	#if (PTR_SIZE == 8)
		typedef int64_t			intptr_t;
		#define INTPTR_MAX		INT64_MAX
		#define INTPTR_MIN		INT64_MIN
		
		typedef uint64_t		uintptr_t;
		#define UINTPTR_MAX		UINT64_MAX
	#elif (PTR_SIZE == 4)
		typedef int32_t			intptr_t;
		#define INTPTR_MAX		INT32_MAX
		#define INTPTR_MIN		INT32_MIN
		
		typedef uint32_t		uintptr_t;
		#define UINTPTR_MAX		UINT32_MAX
	#elif (PTR_SIZE == 2)
		typedef int16_t			intptr_t;
		#define INTPTR_MAX		INT16_MAX
		#define INTPTR_MIN		INT16_MIN
		
		typedef uint16_t		uintptr_t;
		#define UINTPTR_MAX		UINT16_MAX
	#endif
	
#endif



/* TUI */
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

#endif /* __AHTDEFS_H__ */

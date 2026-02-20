#ifndef AHTDEFS_H
#define AHTDEFS_H

#include <limits.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif



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
	#define BITS_32 /* generic pointer size, not best practice */
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
	#define OS_UNKNOWN
	#warning Unknwon OS
#endif
/* OS and pointer size detection end */



/* Detect Endianness (Little Endian vs Big Endian) */
#if defined(__BYTE_ORDER__) && defined(__ORDER_LITTLE_ENDIAN__) && (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__) /*  GCC/Clang macros */
	#define ARCH_LITTLE_ENDIAN
#elif defined(__BYTE_ORDER__) && defined(__ORDER_BIG_ENDIAN__) && (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
	#define ARCH_BIG_ENDIAN
/* Some compilers have special Little-Endian macros. */
#elif defined(__LITTLE_ENDIAN__) || defined(_LITTLE_ENDIAN) || defined(__ARMEL__) || defined(__THUMBEL__) || defined(__AARCH64EL__)
	#define ARCH_LITTLE_ENDIAN
/* Some compilers have special Big-Endian macros. (SPARC, PowerPC) */
#elif defined(__BIG_ENDIAN__) || defined(_BIG_ENDIAN) || defined(__ARMEB__) || defined(__THUMBEB__) || defined(__AARCH64EB__) || defined(__sparc) || defined(__sparc__) || defined(_POWER) || defined(__powerpc__) || defined(__ppc__)
	#define ARCH_BIG_ENDIAN
/* detect from os, architecture */
#elif defined(ARCH_X86) || defined(ARCH_X86_16) || defined(ARCH_X64) || defined(OS_WIN) || defined(OS_DOS)
	/* x86, x64 ailesi ve DOS/Windows tabanli sistemler donanimsal olarak Little-Endian'dir */
	#define ARCH_LITTLE_ENDIAN
#else
	#define ARCH_UNKNOWN_ENDIAN
	#warning Endianness could not be determined at compile time
#endif
/* Endianness detection ended */



/* Detect compiler */
#if defined(__clang__)
	#define COMP_CLANG
#elif defined(__INTEL_COMPILER) || defined(__INTEL_LLVM_COMPILER) || defined(__ICL)
	#define COMP_INTEL
#elif defined(__GNUC__) || defined(__GNUG__)
	#define COMP_GCC
#elif defined(_MSC_VER)
	#define COMP_MSVC
#elif defined(__BORLANDC__) || defined(__TURBOC__)
	#define COMP_BORLAND
#elif defined(__WATCOMC__)
	#define COMP_WATCOM
#elif defined(__TINYC__)
	#define COMP_TCC
#elif defined(__DMC__) || defined(__SC__) || defined(__ZTC__) /* Digital Mars / Symantec / Zortech */
	#define COMP_DMC
#elif defined(__SUNPRO_C) || defined(__SUNPRO_CC)
	#define COMP_SUN
#elif defined(__IBMC__) || defined(__IBMCPP__) || defined(__xlC__)
	#define COMP_IBM
#elif defined(__CC_ARM) || defined(__ARMCC_VERSION)
	#define COMP_ARM
#elif defined(__TI_COMPILER_VERSION__)
	#define COMP_TI
#elif defined(__PGI)
	#define COMP_PGI
#else
	#define COMP_UNKNOWN
	#warning Unknown compiler
#endif
/* Compiler detection ended */

#if defined(COMP_WATCOM)
	#define asm _asm
#endif




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
	#if !defined(_EXACT_WIDTH_INTS)
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
		
		#if !defined(_INTPTR_T_DEFINED)
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
			#define _INTPTR_T_DEFINED /* to avoid watcom library definition conflict */
		#endif /* _INTPTR_T_DEFINED */
		#define _EXACT_WIDTH_INTS /* to avoid watcom library definition conflict */
	#endif /* _EXACT_WIDTH_INTS */
	
#endif


#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define MIN(a, b) (((a) < (b)) ? (a) : (b))


#ifdef __cplusplus
}
#endif

#endif /* AHTDEFS_H */

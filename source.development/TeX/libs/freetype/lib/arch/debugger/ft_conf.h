/*******************************************************************
 *
 *  ft_conf.h for a debugging build
 *
 *
 *  Copyright 1996-2002 by
 *  David Turner, Robert Wilhelm, and Werner Lemberg
 *
 *  This file is part of the FreeType project, and may only be used
 *  modified and distributed under the terms of the FreeType project
 *  license, LICENSE.TXT.  By continuing to use, modify, or distribute
 *  this file you indicate that you have read the license and
 *  understand and accept it fully.
 *
 ******************************************************************/


/* we need the following because there are some typedefs in this file */

#ifndef FT_CONF_H
#define FT_CONF_H

/* Define to empty if the keyword does not work.  */
/* #undef const */

/* Define if you have the <stdlib.h> header file.  */
#define HAVE_STDLIB_H

/* Define if you have the memcpy function.  */
#define HAVE_MEMCPY

/* Define if you have the memmove function.  */
#define HAVE_MEMMOVE

/* command.com can't pipe stderr into a file; so any message will */
/* be redirected into stdout (which is manageable).               */
#define HAVE_PRINT_FUNCTION 1

#define Print( format, ap )  vfprintf( stdout, (format), (ap) )


/* The number of bytes in a int. We use the ANSI header file limits.h */
/* for determining it since there is no easy way to guess.            */
/*                                                                    */
#include <limits.h>
#if   UINT_MAX == 0xFFFF
#define SIZEOF_INT  2
#elif UINT_MAX == 0xFFFFFFFF
#define SIZEOF_INT  4
#else
#error "Unsupported number of bytes in `int' type!"
#endif

/* We now try to guess the size of longs in the same way */
/*                                                       */
#if   ULONG_MAX == 0xFFFFFFFF
#define SIZEOF_LONG 4
#elif ULONG_MAX == 0xFFFFFFFFFFFFFFFF
#define SIZEOF_LONG 8
#else
#error "Unsupported number of bytes in `long' type!"
#endif


/* Define if you have the <libintl.h> header file.  */
/* #undef HAVE_LIBINTL_H */

/* Define if you have the <locale.h> header file. */ 
/* Only meaningful if you have <libintl.h>.       */
#define HAVE_LOCALE_H

/**********************************************************************/
/*                                                                    */
/*  The following configuration macros can be tweaked manually by     */
/*  a developer to turn on or off certain features or options in the  */
/*  TrueType engine. This may be useful to tune it for specific       */
/*  purposes..                                                        */
/*                                                                    */
/**********************************************************************/


/*************************************************************************/
/* Define this if the underlying operating system uses a different       */
/* character width than 8bit for file names.  You must then also supply  */
/* a typedef declaration for defining 'TT_Text'.  Default is off.        */

/* #define HAVE_TT_TEXT */


/*************************************************************************/
/* Define this if you want to generate code to support engine extensions */
/* Default is on, but if you're satisfied by the basic services provided */
/* by the engine and need no extensions, undefine this configuration     */
/* macro to save a few more bytes.                                       */

#define  TT_CONFIG_OPTION_EXTEND_ENGINE


/*************************************************************************/
/* Define this if you want to generate code to support gray-scaling,     */
/* a.k.a. font-smoothing or anti-aliasing. Default is on, but you can    */
/* disable it if you don't need it.                                      */

#define  TT_CONFIG_OPTION_GRAY_SCALING


/*************************************************************************/
/* Define this if you want to compile a bytecode interpreter in the      */
/* TrueType driver.  Note that there are important patent issues related */
/* to the use of the interpreter.                                        */
/*                                                                       */
/* By undefining this, you will only compile the code necessary to load  */
/* TrueType glyphs without hinting.                                      */
/*                                                                       */
/* The debugger is useless (and can't be compiled) without the bytecode  */
/* interpreter. But we require TT_CONFIG_OPTION_BYTECODE_INTERPRETER to  */
/* be passed explicitly to avoid disputes with the lawyers.              */

/* #undef TT_CONFIG_OPTION_BYTECODE_INTERPRETER */


/*************************************************************************/
/* Define this if you want to use a big 'switch' statement within the    */
/* bytecode interpreter. Because some non-optimizing compilers are not   */
/* able to produce jump tables from such statements, undefining this     */
/* configuration macro will generate the appropriate C jump table in     */
/* ttinterp.c. If you use an optimizing compiler, you should leave it    */
/* defined for better performance and code compactness..                 */
/*                                                                       */
/* For debugging, we use a jump table; this allows some interesting      */
/* things during development.                                            */

#undef   TT_CONFIG_OPTION_INTERPRETER_SWITCH


/*************************************************************************/
/* Define this if you want to build a `static' version of the TrueType   */
/* bytecode interpreter.  This will produce much bigger code, which      */
/* _may_ be faster on some architectures.                                */
/*                                                                       */
/* Do NOT DEFINE THIS is you build a thread-safe version of the engine.  */
/*                                                                       */
/* We use a static interpreter in the debugger; this helps in setting    */
/* data breakpoints to fixed memory addresses.                           */

#undef  TT_CONFIG_OPTION_STATIC_INTERPRETER

/* an extra for the debugger */
#define DEBUG_INTERPRETER

/*************************************************************************/
/* Define this if you want to build a 'static' version of the scan-line  */
/* converter (the component which in charge of converting outlines into  */
/* bitmaps). This will produce a bigger object file for "ttraster.c",    */
/* which _may_ be faster on some architectures..                         */
/*                                                                       */
/* Do NOT DEFINE THIS is you build a thread-safe version of the engine   */

#undef  TT_CONFIG_OPTION_STATIC_RASTER


/*************************************************************************/
/* Define TT_CONFIG_THREAD_SAFE if you want to build a thread-safe       */
/* version of the library.                                               */

#undef  TT_CONFIG_OPTION_THREAD_SAFE


/**********************************************************************/
/*                                                                    */
/*  The following macros are used to define the debug level, as well  */
/*  as individual tracing levels for each component. There are        */
/*  currently three modes of operation :                              */
/*                                                                    */
/*  - trace mode (define DEBUG_LEVEL_TRACE)                           */
/*                                                                    */
/*      The engine prints all error messages, as well as tracing      */
/*      ones, filtered by each component's level                      */
/*                                                                    */
/*  - debug mode (define DEBUG_LEVEL_ERROR)                           */
/*                                                                    */
/*      Disable tracing, but keeps error output and assertion         */
/*      checks.                                                       */
/*                                                                    */
/*  - release mode (don't define anything)                            */
/*                                                                    */
/*      Don't include error-checking or tracing code in the           */
/*      engine's code. Ideal for releases.                            */
/*                                                                    */
/* NOTE :                                                             */
/*                                                                    */
/*   Each component's tracing level is defined in its own source.     */
/*                                                                    */
/**********************************************************************/

/* Define if you want to use the tracing debug mode. */
/* Set to DEBUG_LEVEL_TRACE for the debugger.        */
#define  DEBUG_LEVEL_TRACE

/* Define if you want to use the error debug mode - ignored if */
/* DEBUG_LEVEL_TRACE is defined                                */
#undef  DEBUG_LEVEL_ERROR


/**************************************************************************/
/* Definition of various integer sizes. These types are used by ttcalc    */
/* and ttinterp (for the 64-bit integers) only..                          */

#if SIZEOF_INT == 4

  typedef signed int      TT_Int32;
  typedef unsigned int    TT_Word32;

#elif SIZEOF_LONG == 4

  typedef signed long     TT_Int32;
  typedef unsigned long   TT_Word32;

#else
#error "no 32bit type found"
#endif

#if SIZEOF_LONG == 8

/* LONG64 must be defined when a 64-bit type is available */
/* INT64 must then be defined to this type..              */
#define LONG64
#define INT64   long

/* Note that contrary to the `ansi' target, there is no test against C99   */
/* here, because we really want to control whether we use 64-bit type or   */
/* not. So you have to use TT_USE_LONG_LONG to use it on a 32-bit machine. */
#else

/* GCC provides the non-ANSI 'long long' 64-bit type.  You can activate    */
/* by defining the TT_USE_LONG_LONG macro in 'ft_conf.h'.  Note that this  */
/* will produce many -ansi warnings during library compilation.            */
#ifdef TT_USE_LONG_LONG

#define LONG64
#define INT64   long long

#endif /* TT_USE_LONG_LONG */
#endif

#endif /* FT_CONF_H */


/* END */

/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#if !defined( ideEnums__ )
#define ideEnums__

typedef enum
{
  COMPILE_C,
  COMPILE_CC,
  COMPILE_ASM,
  COMPILE_FLEX,
  COMPILE_BISON,
  COMPILE_LINK,
  COMPILE_ARCHIVE,
  COMPILE_UNKNOWN,
  COMPILE_NONE,
  COMPILE_OBJC,
  COMPILE_FLEX_TO_C,
  COMPILE_FLEX_TO_CC,
  COMPILE_BISON_TO_C,
  COMPILE_BISON_TO_CC,
  COMPILE_PROJECT,
  COMPILE_PASCAL,
  COMPILE_USER,
  COMPILE_LINK_PASCAL_AUTOMAKE,
  COMPILE_C_LIKE,
  COMPILE_CC_LIKE,
  COMPILE_FORTRAN,
  COMPILE_NASM,
  COMPILE_ADA,
  COMPILE_LINK_ADA,
  COMPILE_FPC,
  COMPILE_LINK_FPC_AUTOMAKE,
  COMPILE_TEX,
  COMPILE_LINK_DLL
}
COMPILE_ID;

typedef enum
{
  FILE_C_SOURCE,                // *.c
  FILE_CC_SOURCE,               // *.cc *.cpp *.cxx *.C
  FILE_FLEX_SOURCE,             // *.l
  FILE_BISON_SOURCE,            // *.y
  FILE_ASM_SOURCE,              // *.s *.S
  FILE_OBJECT,                  // *.o
  FILE_LIBRARY,                 // *.a
  FILE_COFF,                    // *.
  FILE_HEADER,                  // *.h *.hpp
  FILE_UNKNOWN,                 // any other not mentioned here
  FILE_EXE,                     // *.exe
  FILE_OBJC_SOURCE,             // *.m
  FILE_PROJECT,                 // *.gpr
  FILE_PASCAL_SOURCE,           // *.p *.pas
  FILE_C_SOURCE_I,              // *.i
  FILE_CC_SOURCE_II,            // *.ii
  FILE_FORTRAN_SOURCE,          // *.f *.for
  FILE_FORTRAN_SOURCE_PRE,      // *.F *.fpp
  FILE_NASM_SOURCE,             // *.asm *.nsm
  FILE_NOTHING,                 // empty filenames
  FILE_ADA_SOURCE,
  FILE_ADA_SPEC,
  FILE_FPC_SOURCE,              // *.pp *.pas *.pas depends on use_fpc flag
  FILE_TEX_SOURCE,
  FILE_DVI,
  FILE_DLL                      // *.so *.dll
}
FILE_TYPE;

typedef enum
{
  COMPILER_AUTO,
  COMPILER_USER,
  COMPILER_NONE,
  COMPILER_C,
  COMPILER_CC,
  COMPILER_ASM,
  COMPILER_GPC,
  COMPILER_FORTRAN,
  COMPILER_NASM,
  COMPILER_FPC
}
COMPILER_TYPE;

typedef enum
{
  ERROR_AUTO,
  ERROR_USER,
  ERROR_NONE,
  ERROR_BUILTIN_C,
  ERROR_BUILTIN_ASM,
  ERROR_BUILTIN_LINK,
  ERROR_RETVAL,
  ERROR_BUILTIN_FPC
}
ERROR_TYPE;

#endif

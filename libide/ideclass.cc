/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#include <libtvuti.h>

#define Uses_ideFunctions
#define Uses_ideEnums
#include <libide.h>

#include <string.h>
#include <ctype.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>

extern "C" time_t _file_time_stamp(unsigned int);

long
time_of_file(const char *fname)
{
  long retval;
  struct stat s;

  if (stat(fname, &s) == 0)
    retval = s.st_mtime;
  else
    retval = 0;
  return retval;
}

FILE_TYPE
get_file_type(const char *_ext)
{
  if (!_ext)
    return FILE_NOTHING;
  char *ext = strrchr(_ext, '.');

  if (!ext)
  {
    if (!*_ext)
      return FILE_NOTHING;
    return FILE_EXE;
  }
  if (!strcmp(ext, ".c"))
    return FILE_C_SOURCE;
  if (!strcmp(ext, ".i"))
    return FILE_C_SOURCE_I;
  if (!strcmp(ext, ".cc"))
    return FILE_CC_SOURCE;
  if (!strcmp(ext, ".cpp"))
    return FILE_CC_SOURCE;
  if (!strcmp(ext, ".cxx"))
    return FILE_CC_SOURCE;
  if (!strcmp(ext, ".ii"))
    return FILE_CC_SOURCE_II;
  if (!strcmp(ext, ".C"))
    return FILE_CC_SOURCE;      // note: capital C
  if (!strcmp(ext, ".o"))
    return FILE_OBJECT;
  if (!strcmp(ext, ".s"))
    return FILE_ASM_SOURCE;
  if (!strcmp(ext, ".S"))
    return FILE_ASM_SOURCE;
  if (!strcmp(ext, ".l"))
    return FILE_FLEX_SOURCE;
  if (!strcmp(ext, ".y"))
    return FILE_BISON_SOURCE;
  if (!strcmp(ext, ".a"))
    return FILE_LIBRARY;
  if (!strcmp(ext, ".exe"))
    return FILE_EXE;
  if (!strcmp(ext, ".h"))
    return FILE_HEADER;
  if (!strcmp(ext, ".hpp"))
    return FILE_HEADER;
  if (!strcmp(ext, ".ha"))
    return FILE_HEADER;
  if (!strcmp(ext, ".hd"))
    return FILE_HEADER;
  if (!strcmp(ext, ".m"))
    return FILE_OBJC_SOURCE;
  if (!strcasecmp(ext, ".gpr"))
    return FILE_PROJECT;        // ignore case
  if (!strcmp(ext, ".p"))
    return FILE_PASCAL_SOURCE;
  if (!strcmp(ext, ".pp"))
    return FILE_FPC_SOURCE;
  if (!strcmp(ext, ".pas"))
    return FILE_PASCAL_SOURCE;
  if (!strcmp(ext, ".f"))
    return FILE_FORTRAN_SOURCE;
  if (!strcmp(ext, ".for"))
    return FILE_FORTRAN_SOURCE;
  if (!strcmp(ext, ".F"))
    return FILE_FORTRAN_SOURCE_PRE;
  if (!strcmp(ext, ".fpp"))
    return FILE_FORTRAN_SOURCE_PRE;
  if (!strcmp(ext, ".asm"))
    return FILE_NASM_SOURCE;
  if (!strcmp(ext, ".nsm"))
    return FILE_NASM_SOURCE;
  if (!strcmp(ext, "."))
    return FILE_COFF;
  if (!strcmp(ext, ".adb"))
    return FILE_ADA_SOURCE;
  if (!strcmp(ext, ".ads"))
    return FILE_ADA_SPEC;
  if (!strcmp(ext, ".tex"))
    return FILE_TEX_SOURCE;
  if (!strcmp(ext, ".dvi"))
    return FILE_DVI;
  if (!strcmp(ext, ".so"))
    return FILE_DLL;
  if (!strcmp(ext, ".dll"))
    return FILE_DLL;
  return FILE_UNKNOWN;
}

COMPILE_ID
how_to_compile(FILE_TYPE from, FILE_TYPE to)
{
  if (from == to && from != FILE_UNKNOWN)
    return COMPILE_NONE;
  switch (from)
  {
    case FILE_TEX_SOURCE:
      switch (to)
      {
        case FILE_DVI:
          return COMPILE_TEX;
        default:
          break;
      }
    case FILE_ADA_SOURCE:
      switch (to)
      {
        case FILE_OBJECT:
          return COMPILE_ADA;
        default:
          break;
      }
    case FILE_NASM_SOURCE:
      switch (to)
      {
        case FILE_OBJECT:
          return COMPILE_NASM;
        default:
          break;
      }
    case FILE_FORTRAN_SOURCE:
    case FILE_FORTRAN_SOURCE_PRE:
      switch (to)
      {
        case FILE_OBJECT:
          return COMPILE_FORTRAN;
        default:
          break;
      }
    case FILE_PROJECT:
      return COMPILE_PROJECT;
    case FILE_FLEX_SOURCE:
      switch (to)
      {
        case FILE_OBJECT:
          return COMPILE_FLEX;
        case FILE_C_SOURCE:
          return COMPILE_FLEX_TO_C;
        case FILE_CC_SOURCE:
          return COMPILE_FLEX_TO_CC;
        default:
          break;
      }
    case FILE_BISON_SOURCE:
      switch (to)
      {
        case FILE_OBJECT:
          return COMPILE_BISON;
        case FILE_C_SOURCE:
          return COMPILE_BISON_TO_C;
        case FILE_CC_SOURCE:
          return COMPILE_BISON_TO_CC;
        default:
          break;
      }
    case FILE_OBJC_SOURCE:
      switch (to)
      {
        case FILE_OBJECT:
          return COMPILE_OBJC;
        default:
          break;
      }
    case FILE_CC_SOURCE:
    case FILE_CC_SOURCE_II:
      switch (to)
      {
        case FILE_OBJECT:
          return COMPILE_CC;
        case FILE_ASM_SOURCE:
          return COMPILE_CC_LIKE;
        default:
          break;
      }
    case FILE_C_SOURCE:
    case FILE_C_SOURCE_I:
      switch (to)
      {
        case FILE_OBJECT:
          return COMPILE_C;
        case FILE_ASM_SOURCE:
          return COMPILE_C_LIKE;
        default:
          break;
      }
    case FILE_ASM_SOURCE:
      switch (to)
      {
        case FILE_OBJECT:
          return COMPILE_ASM;
        default:
          break;
      }
    case FILE_PASCAL_SOURCE:
      switch (to)
      {
        case FILE_OBJECT:
          return COMPILE_PASCAL;
        case FILE_COFF:
        case FILE_EXE:
          return COMPILE_LINK_PASCAL_AUTOMAKE;
          break;
        default:
          break;
      }
    case FILE_FPC_SOURCE:
      switch (to)
      {
        case FILE_OBJECT:
          return COMPILE_FPC;
        case FILE_COFF:
        case FILE_EXE:
          return COMPILE_LINK_FPC_AUTOMAKE;
          break;
        default:
          break;
      }
    case FILE_UNKNOWN:
    case FILE_OBJECT:
      switch (to)
      {
        case FILE_COFF:
        case FILE_EXE:
          return COMPILE_LINK;
        case FILE_LIBRARY:
          return COMPILE_ARCHIVE;
	case FILE_DLL:
	  return COMPILE_LINK_DLL;
        default:
          break;
      }
    default:
      break;
  }
  if (from == FILE_NOTHING || to == FILE_NOTHING)
    return COMPILE_NONE;
  return COMPILE_UNKNOWN;
}

COMPILE_ID
how_to_compile(const char *ext_from, const char *ext_to)
{
  FILE_TYPE from = get_file_type(ext_from), to = get_file_type(ext_to);

  return how_to_compile(from, to);
}

/* Copyright (C) 1996-2000 Robert H”hne, see COPYING.RHU for details */
#include <limits.h>
#include <string.h>
#include <rhutils.h>

#ifdef __DJGPP__
#include <sys/stat.h>
#else
#define _fixpath __fixpath
#endif

void
FExpand(char *&name, int new_alloc)
{
  char _static_buffer[PATH_MAX];
  char *fname = _static_buffer;

  _fixpath(name, fname);
  if (new_alloc)
  {
    string_free(name);
    string_dup(name, fname);
  }
  else
    strcpy(name, fname);
}

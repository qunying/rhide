/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
/*
  $Id$
*/
#include <rhutils.h>

char *expand_variables(char *&x)
{
  char *var,*var_end,*env;
  char *buffer;
  char *xptr,*bptr;
  if (!strchr(*x,'$')) return *x;
  buffer = _static_buffer;
  xptr = *x;
  bptr = buffer;
  while (*xptr)
  {
    if (*xptr == '$' && xptr[1] == '(')
    {
      char old;
      var = xptr+2;
      var_end = var+1;
      while (*var_end && *var_end != ')') var_end++;
      old = *var_end;
      *var_end = 0;
      env = getenv(var);
      *var_end = old;
      if (env)
      {
        while (*env)
        {
          *bptr++ = *env++;
        }
        xptr = var_end+1;
        continue;
      }
      *bptr++ = *xptr++;
    }
    else
    {
      *bptr++ = *xptr++;
    }
  }
  *bptr = 0;
  string_free(*x);
  string_dup(*x,buffer);
  return *x;
}


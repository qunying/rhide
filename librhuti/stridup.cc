/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#include <rhutils.h>
#include <stdlib.h>
#include <string.h>

/*
   This function is similar to strdup, but it returns
   always an allocated string.
   You can call it with an empty string or NULL
*/
char *string_dup(const char *src)
{
  char *dest;
  if (!src || !*src)
  {
    dest = (char *)malloc(1);
    *dest = 0;
  }
  else dest = strdup(src);
  return dest;
}

char *string_dup(char *&dest, const char *src)
{
  return (dest = string_dup(src));
}



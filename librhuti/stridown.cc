/* Copyright (C) 1996,1997 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
/*
 $Id$
*/
#include <rhutils.h>
#include <ctype.h>

char *string_down(char * dest)
{
  if (!dest) return dest;
  while (*dest) *dest = tolower(*dest++);
  return dest;
}

char *string_up(char * dest)
{
  if (!dest) return dest;
  while (*dest) *dest = toupper(*dest++);
  return dest;
}



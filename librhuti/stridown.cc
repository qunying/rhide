/* Copyright (C) 1996-2000 Robert H”hne, see COPYING.RHU for details */
#include <rhutils.h>
#include <ctype.h>

char *
string_down(char *dest)
{
  if (!dest)
    return dest;
  while (*dest)
  {
    *dest = tolower((int) (unsigned char) *dest);
    dest++;
  }
  return dest;
}

char *
string_up(char *dest)
{
  if (!dest)
    return dest;
  while (*dest)
  {
    *dest = toupper((int) (unsigned char) *dest);
    dest++;
  }
  return dest;
}

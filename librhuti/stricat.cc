/* Copyright (C) 1996-2000 Robert H”hne, see COPYING.RHU for details */
#include <rhutils.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

char *
string_cat(char *&dest, const char *src)
{
  if (!dest)
  {
    string_dup(dest, src);
    return dest;
  }
  if (!src)
    return dest;
  int l1 = strlen(dest), l2 = strlen(src);

  dest = (char *) realloc(dest, l1 + l2 + 1);
  memcpy(dest + l1, src, l2 + 1);
  return dest;
}

static void
string_cat(char *&dest, va_list va)
{
  const char *arg;
  while ((arg = va_arg(va, const char *)) != NULL)
  {
    string_cat(dest, arg);
  }
}

char *
string_cat(char *&dest, const char *src, const char *src1 ...)
{
  va_list va;

  va_start(va, src1);
  string_cat(dest, src);
  string_cat(dest, src1);
  string_cat(dest, va);
  va_end(va);
  return dest;
}

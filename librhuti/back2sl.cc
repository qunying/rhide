/* Copyright (C) 1996-2000 Robert H”hne, see COPYING.RHU for details */
#include <rhutils.h>
#include <string.h>

void
BackslashToSlash(char *name)
{
  char *tmp;

  if (!name || !*name)
    return;
  while ((tmp = strchr(name, '\\')) != NULL)
    *tmp = '/';
}

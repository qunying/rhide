/* Copyright (C) 1996-2000 Robert H”hne, see COPYING.RHU for details */
#include <rhutils.h>
#include <stdlib.h>

void
string_free(char *&dest)
{
  if (dest)
    free(dest);
  dest = NULL;
}

/* Copyright (C) 1996,1997 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
/*
 $Id$
*/
#include <rhutils.h>
#include <stdlib.h>

void string_free(char * &dest)
{
  if (dest) free(dest);
  dest = NULL;
}



/* Copyright (C) 1996,1997 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
/*
 $Id$
*/
#include <rhutils.h>
#include <string.h>

void BackslashToSlash(char * name)
{
  char *tmp;
  if (!name || !*name) return;
  while ((tmp = strchr(name,'\\')) != NULL) *tmp = '/';
}




/* Copyright (C) 1996,1997 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
/*
 $Id$
*/
#include <rhutils.h>

#ifdef __DJGPP__
#include <sys/stat.h>
#else
#define _fixpath __fixpath
#endif

static char _static_buffer[2048]; // this should be enough

void FExpand(char * & name,int new_alloc)
{
  char *fname = _static_buffer;
  _fixpath(name,fname);
  if (new_alloc)
  {
    string_free(name);
    string_dup(name,fname);
  }
  else strcpy(name,fname);
}




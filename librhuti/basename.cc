/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#include <rhutils.h>
#include <string.h>
#include <stdlib.h>

/**[txh]********************************************************************

 Function: BaseName
 Include: rhutils.h
 Prototype: void BaseName(char *name, int with_suffix = 1)
 Description: Removes any directory part of 'name'. Set 'with_suffix'
 to 0 to remove also the suffix.

***************************************************************************/

void BaseName(char *name, int with_suffix)
{
  char *_name = strrchr(name,'/');
  if (!_name)
    _name = name;
  else
    _name++;
  if (!with_suffix)
  {
    char *ext = strrchr(_name,'.');
    if (ext)
      *ext = 0;
  }
  if (_name != name)
    strcpy(name,_name);
}

/**[txh]********************************************************************

 Function: BaseName
 Prototype: void BaseName(const char *name, char *&bname, int with_suffix)
 Description: The same as @x{BaseName}, but here the result is stored in the
 newly allocated 'bname'.

***************************************************************************/

void BaseName(const char *name, char *&bname, int with_suffix)
{
  const char *_name = strrchr(name,'/');
  if (!_name)
    _name = name;
  else
    _name++;
  const char *ext = _name + strlen(_name);
  if (!with_suffix)
  {
    const char *_ext = strrchr(_name,'.');
    if (_ext)
      ext = _ext;
  }
  int len = ext - _name;
  bname = (char *)malloc(len+1);
  memcpy(bname, _name, len);
  bname[len] = 0;
}




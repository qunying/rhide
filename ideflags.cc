/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#define Uses_TSCollection
#define Uses_TDirList
#define Uses_TParamList
#include <libtvuti.h>

#include <rhutils.h>

#define Uses_TProject
#define Uses_TOptions
#define Uses_TFlagEntry
#define Uses_TFlagCollection
#define Uses_TDepCollection
#include <libide.h>

#include "rhide.h"

void
AddLibraries(char *&retval)
{
#if 0
  int iostr_flag = 0;
  int bison_flag = 0;
  int flex_flag = 0;
  int objc_flag = 0;
  int gpc_flag = 0;
  int m_flag = 0;
  int f2c_flag = 0;
#endif
  int i;

  retval = NULL;
  for (i = 0; i < Options.user_libs->getCount(); i++)
  {
    if (Options.libs & (1 << i))
    {
      if (retval)
        string_cat(retval, " ");
      string_cat(retval, (char *) (Options.user_libs->at(i)));
    }
  }
#if 0
  if (NoStdLib)
    return;
  for (i = 0; i < Project.dependencies->getCount(); i++)
  {
    switch (((TDependency *) Project.dependencies->at(i))->compile_id)
    {
      case COMPILE_FORTRAN:
        m_flag = 1;
        f2c_flag = 1;
        break;
      case COMPILE_PASCAL:
        gpc_flag = 1;
        m_flag = 1;
        break;
      case COMPILE_CC:
        iostr_flag = 1;
        break;
      case COMPILE_BISON:
        bison_flag = 1;
        break;
      case COMPILE_FLEX:
        flex_flag = 1;
        break;
      case COMPILE_OBJC:
        objc_flag = 1;
      default:
        break;
    }
  }
  if (objc_flag)
  {
    if (retval)
      string_cat(retval, " ");
    string_cat(retval, "objc");
  }
  if (bison_flag)
  {
    if (retval)
      string_cat(retval, " ");
    string_cat(retval, "bison");
  }
  if (flex_flag)
  {
    if (retval)
      string_cat(retval, " ");
    string_cat(retval, "fl");
  }
  if (gpc_flag)
  {
    if (retval)
      string_cat(retval, " ");
    string_cat(retval, "gpc");
  }
  if (iostr_flag)
  {
    if (retval)
      string_cat(retval, " ");
#ifdef __DJGPP__
    string_cat(retval, "iostr");
#else
#if 0
    string_cat(retval, "g++");
#else
    string_cat(retval, "stdc++");
#endif
#endif
  }
  if (f2c_flag)
  {
    if (retval)
      string_cat(retval, " ");
    string_cat(retval, "f2c");
  }
  if (m_flag)
  {
    if (retval)
      string_cat(retval, " ");
    string_cat(retval, "m");
  }
#endif
}

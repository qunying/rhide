/* Copyright (C) 1996,1997 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
/*
 $Id$
*/
#define Uses_MsgBox

#define Uses_TDirList
#define Uses_tvutilFunctions
#include <libtvuti.h>

#include <rhutils.h>

void editDirList(TDirList *dirlist,
                 const char *title,int hist_id)
{
  ushort result;
  char *tmp,temp[1000];
  dirlist->ToString(tmp);
  strcpy(temp,tmp);
  string_free(tmp);
  result = HistinputBox(title,_("~P~athlist"),temp,999,hist_id);
  if (result == cmOK)
  {
    BackslashToSlash(temp);
    dirlist->FromString(temp);
  }
}


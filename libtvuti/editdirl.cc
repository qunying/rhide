/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#include <string.h>
#define Uses_MsgBox

#define Uses_TDirList
#define Uses_tvutilFunctions
#include <libtvuti.h>

#include <rhutils.h>

void
editDirList(TDirList * dirlist, const char *title, int hist_id)
{
  ushort result;
  char *tmp;

  dirlist->ToString(tmp);
  int l = strlen(tmp);
  if (l < 1024)
    l = 1024;
  char temp[l*2+1];
  strcpy(temp, tmp);
  string_free(tmp);
  result = HistinputBox(title, _("~P~athlist"), temp, l*2, hist_id);
  if (result == cmOK)
  {
    BackslashToSlash(temp);
    dirlist->FromString(temp);
  }
}

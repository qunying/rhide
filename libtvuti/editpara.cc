/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#include <string.h>
#define Uses_MsgBox

#define Uses_TParamList
#define Uses_tvutilFunctions
#include <libtvuti.h>

#include <rhutils.h>

ushort
editParamList(TParamList * paramlist, const char *title, int hist_id)
{
  ushort result;
  Boolean retval = False;
  char *tmp;
  TParamList *pl;

  paramlist->ToString(tmp);
  int l = strlen(tmp);
  if (l < 1024)
    l = 1024;
  char temp[l*2+1];
  strcpy(temp, tmp);
  string_free(tmp);
  pl = new TParamList();
  do
  {
    result = HistinputBox(title, _("~P~arameter"), temp, l*2, hist_id);
    if (result == cmOK)
    {
      retval = pl->FromString(temp);
      if (retval == False)
      {
        messageBox(_("There is an unmatched quote in your arguments"),
                   mfError | mfOKButton);
      }
    }
  }
  while (result == cmOK && retval == False);
  destroy(pl);
  if (result == cmOK)
    paramlist->FromString(temp);
  return result;
}

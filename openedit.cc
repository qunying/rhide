/* Copyright (C) 1996-2000 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#include <unistd.h>
#define Uses_MsgBox
#define Uses_TApplication
#define Uses_TDirList
#define Uses_tvutilCommands
#define Uses_tvutilFunctions
#define Uses_TIDEEditWindow
#define Uses_TProject
#define Uses_TOptions
#define Uses_ideFunctions
#include <libide.h>
#include <libtvuti.h>

#include <rhutils.h>

#include "rhide.h"
#include "ideapp.h"


// This is for the Ctrl+ENTER feature

void
OpenFileFromEditor(char *name)
{
  char *sname;
  TCEditWindow *ewindow = NULL;

  string_dup(sname, name);
  FExpand(sname);
  ewindow = is_on_desktop(sname);
  if (!ewindow)
  {
    if (!__file_exists(name))
    {
      char *_sname, *arg = NULL;
      char *bname;

      BaseName(name, bname);
      string_cat(arg, "$(word 1,$(foreach dir,$(RHIDE_STANDARD_INCLUDES),\
$(wildcard $(dir)/", bname, ")))", NULL);
      _sname = expand_rhide_spec(arg);
      if (*_sname)
      {
        string_free(sname);
        sname = _sname;
      }
      else
        string_free(_sname);
      string_free(bname);
      string_free(arg);
    }
    if (messageBox(mfConfirmation | mfYesButton | mfNoButton,
                   _("Open the file %s"), sname) == cmYes)
    {
      ewindow = ((IDE *) TProgram::application)->openEditor(sname, True);
    }
  }
  string_free(sname);
  if (ewindow)
    ewindow->select();
}

void
OpenEditor(const char *name, Boolean search, TCEditWindow * &ew)
{
  ew = OpenEditor(name, search);
  if (ew)
    AddReference((TWindow **) & ew);
}

TCEditWindow *
OpenEditor(const char *name, Boolean search)
{
  char *sname;
  TCEditWindow *ewindow;

  if (search == True)
  {
    if (FindFile(name, sname) == False && UseRCS)
    {
      char *rcs_name = NULL;
      char *_sname = NULL;

      if (FindRCSFile(name, rcs_name, _sname) == True)
      {
        CheckoutRCSFile(rcs_name, sname, 1);
        string_free(rcs_name);
        string_free(sname);
        sname = _sname;
      }
    }
  }
  else
  {
    string_dup(sname, name);
    FExpand(sname);
  }
  ewindow = is_on_desktop(sname);
  if (!ewindow)
  {
    ewindow = ((IDE *) TProgram::application)->openEditor(sname, True);
  }
  string_free(sname);
  ewindow->select();
  return ewindow;
}

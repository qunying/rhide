/* Copyright (C) 1996-2000 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#define Uses_TIDEEditWindow
#define Uses_TProgram
#define Uses_TApplication
#define Uses_TEvent
#define Uses_TKeys

#define Uses_TProjectListBox
#define Uses_TDependency
#define Uses_TDepCollection
#define Uses_ideCommands
#define Uses_ideFunctions
#include <libide.h>

#define Uses_tvutilCommands
#include <libtvuti.h>

#include <string.h>

TProjectListBox::TProjectListBox(const TRect & bounds, ushort aNumCols,
TScrollBar * aScrollBar):
TSortedListBox(bounds, aNumCols, aScrollBar)
{
  helpCtx = hcProjectListBox;
}

void
TProjectListBox::getText(char *dest, ccIndex item, short maxlen)
{
  strncpy(dest, FName(((TDependency *) list()->at(item))->source_name),
          maxlen);
  dest[maxlen] = EOS;
}

void
TProjectListBox::selectItem(ccIndex item)
{
  TDependency *rec;

  if (!list())
    return;
  message(owner, evBroadcast, cmListItemSelected, list());
  rec = (TDependency *) list()->at(item);
  if (!rec->source_name)
    return;
  switch (rec->source_file_type)
  {
    case FILE_OBJECT:
    case FILE_EXE:
    case FILE_COFF:
    case FILE_LIBRARY:
      break;
    case FILE_PROJECT:
    {
      TEvent event;

      event.what = evCommand;
      event.message.command = cmSelectProject;
      event.message.infoPtr = rec;
      putEvent(event);
      break;
    }
    default:
      if (message
          (TProgram::application, evBroadcast, cmOpenWindow,
           (void *) FName(rec->source_name)))
      {
        message(TProgram::application, evBroadcast, cmGotoWindowLine,
                (void *) 0);
      }
      break;
  }
}

void
TProjectListBox::handleEvent(TEvent & event)
{
  TSortedListBox::handleEvent(event);
  if (event.what == evKeyDown)
  {
    switch (event.keyDown.keyCode)
    {
      case kbEnter:
        if (!list() || list()->getCount() == 0)
          break;
        selectItem(focused);
        clearEvent(event);
        break;
    }
  }
}

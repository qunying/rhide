/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#define Uses_TEvent
#define Uses_TKeys
#define Uses_TProgram
#define Uses_TDeskTop

#define Uses_TMsgListBox
#define Uses_TMsgCollection
#define Uses_MsgRec
#define Uses_tvutilCommands
#include <libtvuti.h>

#include <rhutils.h>

#include <stdio.h>
#include <string.h>

TMsgListBox::TMsgListBox(const TRect & bounds, ushort aNumCols,
                         TScrollBar * aScrollBar):
TEnterListBox(bounds, aNumCols, aScrollBar)
{
  helpCtx = hcMsgListBox;
}

void
TMsgListBox::handleEvent(TEvent & event)
{
  MsgRec *rec;
  ccIndex index;

  TEnterListBox::handleEvent(event);
  if (!list())
    return;
  switch (event.what)
  {
    case evCommand:
      switch (event.message.command)
      {
        case cmNextMsg:
          for (index = focused + 1; index < range; index++)
          {
            rec = (MsgRec *) list()->at(index);
            if (rec->filename && rec->lineno > 0)
            {
              selectItem(index);
              clearEvent(event);
              break;
            }
          }
          break;
        case cmPrevMsg:
          for (index = focused - 1; index >= 0; index--)
          {
            rec = (MsgRec *) list()->at(index);
            if (rec->filename && rec->lineno > 0)
            {
              selectItem(index);
              clearEvent(event);
              break;
            }
          }
          break;
        default:
          break;
      }
    default:
      break;
  }
}

static int in_focus = 0;
static int in_select = 0;

void
TMsgListBox::focusItem(ccIndex item)
{
  in_focus++;
  MsgRec *rec = (MsgRec *) list()->at(item);

  TEnterListBox::focusItem(item);
  in_focus--;
  if (in_focus || in_select)
    return;
  if (!rec->filename || rec->lineno == 0)
    return;
  if (message
      (TProgram::application, evBroadcast, cmFocusWindow, rec->filename))
  {
    message(TProgram::application, evBroadcast, cmGotoWindowLineColumn,
            (void *) rec);
  }
}

void
TMsgListBox::selectItem(ccIndex item)
{
  in_select++;
  MsgRec *rec = (MsgRec *) list()->at(item);

  TEnterListBox::focusItem(item);
  TEnterListBox::selectItem(item);
  in_select--;
  if (in_select)
    return;
  if (!rec->filename || rec->lineno == 0)
    return;
  if (message
      (TProgram::application, evBroadcast, cmOpenWindow, rec->filename))
  {
    message(TProgram::application, evBroadcast, cmShowWindowMsg,
            (void *) rec->msg);
    message(TProgram::application, evBroadcast, cmGotoWindowLineColumn,
            (void *) rec);
  }
}

const char *
TMsgListBox::getText(ccIndex item)
{
  static char *dest = NULL;
  char temp[10];

  if (!list())
    return "";
  string_free(dest);
  dest = string_dup("");
  MsgRec *rec = (MsgRec *) list()->at(item);

  if (!rec->filename)
  {
    switch (rec->type)
    {
      case msgError:
        string_cat(dest, _("Error: "));
        break;
      case msgWarning:
        string_cat(dest, _("Warning: "));
        break;
      default:
        break;
    }
    if (rec->msg)
      string_cat(dest, rec->msg);
  }
  else
  {
    if (rec->lineno > 0)
    {
      string_cat(dest, rec->filename);
      string_cat(dest, "(");
      sprintf(temp, "%d", rec->lineno);
      string_cat(dest, temp);
      if (rec->column > 1)
      {
        sprintf(temp, ",%d", rec->column);
        string_cat(dest, temp);
      }
      string_cat(dest, ") ");
      switch (rec->type)
      {
        case msgError:
          string_cat(dest, _("Error: "));
          break;
        case msgWarning:
          string_cat(dest, _("Warning: "));
          break;
        default:
          break;
      }
    }
    if (rec->msg)
      string_cat(dest, rec->msg);
  }
  return dest;
}

void
TMsgListBox::getText(char *dest, ccIndex item, short maxLen)
{
  const char *tmp = getText(item);

  strncpy(dest, tmp, maxLen);
  dest[maxLen] = 0;
}

void
TMsgListBox::shutDown()
{
  destroy(list());
  TEnterListBox::shutDown();
}

void
TMsgListBox::setState(ushort aState, Boolean enable)
{
  TEnterListBox::setState(aState, enable);
#if 0
  static int focus_called = 0;

  if (aState & (sfSelected | sfActive) && enable == True && range > 0)
  {
    if (!focus_called && getState(sfActive))
    {
      focus_called++;           // this is needed, because focusItem calls select()
      focusItem(focused);
      focus_called--;
    }
  }
#endif
}

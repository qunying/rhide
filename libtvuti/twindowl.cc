/* Copyright (C) 1996-2000 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#define Uses_TKeys
#define Uses_TEvent
#define Uses_TWindow
#define Uses_TCollection

#define Uses_tvutilCommands
#define Uses_TWindowList
#define Uses_TSCollection
#include <libtvuti.h>

#include <rhutils.h>

#include <string.h>
#include <stdio.h>

TWindowList::TWindowList(const TRect & bounds, ushort aNumCols,
                         TScrollBar * aScrollBar,
                         TSCollection * _open_windows,
TSCollection * _closed_windows):
TEnterListBox(bounds, aNumCols, aScrollBar)
{
  helpCtx = hcWindowList;
  update(_open_windows, _closed_windows);
}

void
TWindowList::selectItem(ccIndex item)
{
  TEnterListBox::selectItem(item);
  if (item >= open_count)
    endModal(cmSelectClosedWindow);
  else
    endModal(cmSelectWindow);
}

void
TWindowList::handleEvent(TEvent & event)
{
  TEnterListBox::handleEvent(event);
  switch (event.what)
  {
    case evCommand:
      switch (event.message.command)
      {
        case cmSelectWindow:
          if (range > 0)
          {
            if (focused >= open_count)
              event.message.command = cmSelectClosedWindow;
            endModal(event.message.command);
          }
          clearEvent(event);
          break;
        case cmDeleteWindow:
          if (range > 0)
          {
            if (focused >= open_count)
              event.message.command = cmDeleteClosedWindow;
            endModal(event.message.command);
          }
          clearEvent(event);
          break;
        default:
          break;
      }
      break;
    default:
      break;
  }
}

void
TWindowList::update(TSCollection * _open_windows,
                    TSCollection * _closed_windows)
{
  ow = _open_windows;
  cw = _closed_windows;
  open_count = ow ? ow->getCount() : 0;
  closed_count = cw ? cw->getCount() : 0;
  setRange(open_count + closed_count);
  if (focused < range)
    focusItem(focused);
  else if (range > 0)
    focusItem(range - 1);
}

void
TWindowList::getText(char *dest, ccIndex item, short maxlen)
{
  if (range == 0)
    strcpy(dest, " ");
  else
  {
    char *_dest = NULL;

    if (item < open_count)
    {
      TWindow *window = ((DeskTopWindow *) ow->at(item))->window;

      if (window->number != wnNoNumber)
      {
        sprintf(dest, "%d ", window->number);
        string_cat(_dest, dest);
      }
      else
        string_cat(_dest, "  ");
      string_cat(_dest, window->getTitle(1000));
    }
    else
    {
      string_cat(_dest, _("Closed"), " (",
                 ((closed_window *) cw->at(item - open_count))->name,
                 ")", NULL);
    }
    strncpy(dest, _dest, maxlen);
    dest[maxlen] = 0;
    string_free(_dest);
  }
}

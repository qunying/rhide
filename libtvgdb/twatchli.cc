/* Copyright (C) 1996-2000 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#define Uses_MsgBox
#define Uses_TCollection
#define Uses_TEvent
#define Uses_TKeys
#define Uses_TStreamable
#define Uses_TScrollBar

#define Uses_tvgdbCommands
#define Uses_TWatchListBox
#define Uses_tvgdbFunctions
#include <libtvgdb.h>
#include <tvgdbhis.h>

#define Uses_TSCollection
#define Uses_tvutilFunctions
#include <libtvuti.h>

#include <librhgdb.h>

#include <string.h>
#include <stdlib.h>

struct TWatchEntry
{
  char *variable;
  char *expr;
};

void
TWatchListBox::GetVariable(ccIndex item, char *buffer, int maxlen)
{
  strncpy(buffer, ((TWatchEntry *) list()->at(item))->variable, maxlen);
  buffer[maxlen] = 0;
}

void
TWatchListBox::SetVariable(ccIndex item, char *buffer)
{
  free(((TWatchEntry *) list()->at(item))->variable);
  ((TWatchEntry *) list()->at(item))->variable = strdup(buffer);
  free(Watches.at(item));
  Watches.atPut(item, strdup(buffer));
  update();
}

void
TWatchListBox::update()
{
  int i, count = list()->getCount();
  TWatchEntry *e;
  int len = 0, maxlen = 0;
  char *invalid = _("not available");
  char *expr;

  for (i = 0; i < count; i++)
  {
    e = (TWatchEntry *) (list()->at(i));
    if (e->expr)
      free(e->expr);
    expr = EvaluateWatch(e->variable, 0);
    if (!expr)
      expr = invalid;
    e->expr = strdup(expr);
    if (hScrollBar)
    {
      len = strlen(e->variable) + strlen(e->expr) + 2;
      if (len > maxlen)
        maxlen = len;
    }
  }
  if (hScrollBar)
  {
    maxlen = maxlen - hScrollBar->size.x;
    if (maxlen < 0)
      maxlen = 0;
    hScrollBar->setRange(0, maxlen);
  }
  draw();
}

void
TWatchListBox::AddVariable(char *var)
{
  TWatchEntry *e;

  e = new TWatchEntry;
  e->variable = strdup(var);
  e->expr = NULL;
  list()->atInsert(focused, e);
  setRange(range + 1);
  update();
}

void
TWatchListBox::RemoveVariable(ccIndex item)
{
  TWatchEntry *e = (TWatchEntry *) (list()->at(item));

  free(e->variable);
  if (e->expr)
    free(e->expr);
  list()->atRemove(item);
  free(e);
  setRange(range - 1);
  Watches.atFree(item);
  update();
}

void
TWatchListBox::selectItem(ccIndex item)
{
  char buffer[256];

  if (item + 1 < range)
  {
    GetVariable(item, buffer, 255);
    if (HistinputBox(_("Change the expression to watch"), _("~E~xpression"),
                     buffer, 255, tvgdb_History_Watch_Expression) == cmOK)
    {
      SetVariable(item, buffer);
      update();
    }
  }
}

void
TWatchListBox::handleEvent(TEvent & event)
{
  switch (event.what)
  {
    case evKeyDown:
      switch (event.keyDown.keyCode)
      {
        case kbEnd:
          hScrollBar->setValue(hScrollBar->maxVal);
          clearEvent(event);
          break;
        case kbHome:
          hScrollBar->setValue(0);
          clearEvent(event);
          break;
        default:
          break;
      }
    default:
      break;
  }
  TEnterListBox::handleEvent(event);
  switch (event.what)
  {
    case evCommand:
      switch (event.message.command)
      {
        case cmEditWatch:
          if (focused + 1 < range)
            selectItem(focused);
          clearEvent(event);
          break;
        case cmRemoveWatch:
          if (focused + 1 < range)
            RemoveVariable(focused);
          clearEvent(event);
          break;
        default:
          break;
      }
    default:
      break;
  }
}

TWatchListBox::TWatchListBox(const TRect & bounds,
                             int numCols, TScrollBar * vbar,
                             TScrollBar * hbar):
TEnterListBox(bounds, numCols, vbar)
{
  helpCtx = hcWatchListBox;
  newList(new TSCollection());
  if (!hbar)
    return;
  hbar->setStep(size.x / numCols, 1);
  hScrollBar = hbar;
  setRange(1);
}

TWatchListBox::~TWatchListBox()
{
  newList(NULL);
}

void
TWatchListBox::getText(char *dest, ccIndex index, short maxlen)
{
  int len;

  if (index + 1 == range)
  {
    *dest = 0;
    return;
  }
  TWatchEntry *e = (TWatchEntry *) list()->at(index);

  strncpy(dest, e->variable, maxlen);
  len = strlen(dest);
  if (len >= maxlen)
    return;
  strncat(dest, ": ", 2);
  len += 2;
  if (len >= maxlen)
    return;
  if (e->expr)
    strncat(dest, e->expr, maxlen - len);
}

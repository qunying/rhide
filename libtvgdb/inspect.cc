/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#define Uses_TScrollBar

#define Uses_TInspector
#define Uses_TInspectList
#define Uses_tvgdbCommands
#include <libtvgdb.h>

TInspector::TInspector(const TRect & bounds, const char *Title):TDialog(bounds, Title),
TWindowInit(TInspector::initFrame)
{
  TRect
    r = bounds;

  r.grow(-1, -1);
  list = new TInspectList(r);
  //list->helpCtx = hcInspector;
  insert(list);
  list->hScrollBar = standardScrollBar(sbHorizontal | sbHandleKeyboard);
  list->vScrollBar = standardScrollBar(sbVertical | sbHandleKeyboard);

  flags |= wfGrow;
}

void
TInspector::update(const char *expr)
{
  list->update(expr);
}

void
TInspector::changeBounds(const TRect & bounds)
{
  TDialog::changeBounds(bounds);
  TRect r = getExtent();

  r.move(-r.a.x, -r.a.y);
  r.grow(-1, -1);
  list->changeBounds(r);
}

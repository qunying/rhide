/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#define Uses_TScrollBar

#define Uses_TProjectWindow
#define Uses_TProjectListBox
#define Uses_TDepCollection
#define Uses_ideCommands
#include <libide.h>

#define COLS (3)

TProjectWindow::TProjectWindow(const TRect & rect,
                               TDepCollection * deps, const char *tit):
TDialog(rect, tit), TWindowInit(TProjectWindow::initFrame)
{
  TRect r = getExtent();
  TScrollBar *scrollbar;

  r.grow(-1, -1);
  scrollbar = standardScrollBar(sbVertical | sbHandleKeyboard);
  liste = new TProjectListBox(r, COLS, scrollbar);
  growMode = gfGrowLoY | gfGrowHiX | gfGrowHiY;
  liste->growMode = gfGrowHiX | gfGrowHiY;
  liste->newList(deps);
  insert(liste);
  flags |= wfGrow | wfZoom;
  helpCtx = hcProjectWindow;
  options |= ofFirstClick;
}

/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#define Uses_TProgram
#define Uses_TDeskTop
#define Uses_TRect

#define Uses_tvutilFunctions
#include <libtvuti.h>

#define Uses_TGDBWindow
#define Uses_TFileViewer
#include <libtvgdb.h>

#include <string.h>

#define VIEWER (GDBOUTWIN->viewer)

static TGDBWindow *GDBOUTWIN = NULL;

TGDBWindow::TGDBWindow(const char *fileName):
TWindow(TProgram::deskTop->getExtent(), fileName, 0),
TWindowInit(&TGDBWindow::initFrame)
{
  options |= ofTileable;
  TRect r(getExtent());

  r.grow(-1, -1);
  viewer = new TFileViewer(r,
                           standardScrollBar(sbHorizontal | sbHandleKeyboard),
                           standardScrollBar(sbVertical | sbHandleKeyboard),
                           fileName);
  insert(viewer);
  viewer->growMode = gfGrowHiX | gfGrowHiY;
  growMode = gfGrowLoY | gfGrowHiX | gfGrowHiY;
  flags |= wfGrow | wfZoom;
}

void
UpdateGDBOutWin(char *buffer)
{
  char *tmp;

  if (!GDBOUTWIN)
    return;
  if (!buffer || !*buffer)
    return;
  while (buffer && *buffer)
  {
    tmp = strchr(buffer, '\n');
    if (!tmp)
    {
      VIEWER->insertLine(strdup(buffer));
      buffer = NULL;
    }
    else
    {
      char c = *tmp;

      *tmp = 0;
      VIEWER->insertLine(strdup(buffer));
      *tmp = c;
      buffer = tmp + 1;
    }
  }
  int count = VIEWER->Count();

  if (count > 0)
    VIEWER->scrollTo(0, count - 1);
  else
    return;
  VIEWER->drawView();
  VIEWER->show();
}

void
InitGDBOutWin()
{
  TGDBWindow *window;

  if (GDBOUTWIN)
    return;
  TRect rect = TProgram::deskTop->getExtent();

  rect.a.y = rect.b.y - 7;
  window = new TGDBWindow("/dev/null");
  delete(char *) (window->title);
  (char *&) window->title = strdup(_("Output from GDB"));
  window->palette = wpGrayWindow;
  window->locate(rect);
  GDBOUTWIN = window;
  window->hide();
  AddWindow(window, (TWindow **) & GDBOUTWIN);
}

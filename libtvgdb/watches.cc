/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#define Uses_TProgram
#define Uses_TDeskTop
#define Uses_TRect
#define Uses_MsgBox
#define Uses_TInputLine

#define Uses_TSCollection
#define Uses_tvutilFunctions
#include <libtvuti.h>

#define Uses_TWatchDialog
#define Uses_TWatchListBox
#define Uses_tvgdbFunctions
#include <libtvgdb.h>
#include <tvgdbhis.h>

#include <librhgdb.h>

#include <string.h>

class TWatchList:public TDialog
{
public:
  TWatchList(const TRect & bounds, const char *aTitle):TDialog(bounds,
                                                               aTitle),
    TWindowInit(TWatchList::initFrame)
  {
  }
  virtual void shutDown();
  virtual void changeBounds(const TRect &);
};

TWatchListBox *watches = NULL;
TDialog *watchwindow = NULL;
TSCollection Watches;
TRect WatchWindowRect(-1, -1, -1, -1);

void
TWatchList::changeBounds(const TRect & r)
{
  TDialog::changeBounds(r);
  WatchWindowRect = r;
}

void
TWatchList::shutDown()
{
  TDialog::shutDown();
  watches = NULL;
}

void
OpenWatchWindow()
{
  TRect r;
  int i;

  if (WatchWindowRect.a.x == -1)
  {
    WatchWindowRect = TProgram::deskTop->getExtent();
    WatchWindowRect.a.y = WatchWindowRect.b.y - 7;
  }
  watchwindow = new TWatchList(WatchWindowRect, _("Watch"));
  r = watchwindow->getExtent();
  r.grow(-1, -1);
  watches = new TWatchListBox(r, 1,
                              watchwindow->
                              standardScrollBar(sbVertical |
                                                sbHandleKeyboard),
                              watchwindow->
                              standardScrollBar(sbHorizontal |
                                                sbHandleKeyboard));
  watchwindow->insert(watches);
  for (i = 0; i < Watches.getCount(); i++)
  {
    watches->AddVariable((char *) Watches.at(i));
  }
  watches->growMode = gfGrowHiX | gfGrowHiY;
  watchwindow->growMode = gfGrowLoY | gfGrowHiX | gfGrowHiY;
  watchwindow->flags |= wfGrow | wfZoom;
  AddWindow(watchwindow, (TWindow **) & watchwindow);
}

void
Evaluate(char *watch)
{
  TWatchDialog *d;

  d =
    new TWatchDialog(TRect(10, 2, 70, 14), _("Evaluate and modify"),
                     watch ? watch : "");
  TProgram::deskTop->execView(d);
  destroy(d);
}

void
AddWatch(char *buffer, Boolean show_window)
{
  if ((show_window == True) && !watchwindow)
    OpenWatchWindow();
  Watches.insert(strdup(buffer));
  if (watchwindow)
    watches->AddVariable(buffer);
}

void
AddWatchEntry(char *_buffer)
{
  TWatchDialog *d;

  d = new TWatchDialog(TRect(10, 2, 70, 10), _("Add to watch list"),
                       _buffer ? _buffer : "", 0);
  if (TProgram::deskTop->execView(d) == cmOK)
    AddWatch(d->input->data);
  destroy(d);
}

void
DeleteAllWatches()
{
  int i = Watches.getCount();

  while (i > 0)
  {
    i--;
    if (watches)
      watches->RemoveVariable(i);
    Watches.atFree(i);
  }
}

/* Copyright (C) 1996-2000 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#include <sys/stat.h>

#define Uses_TWindow
#define Uses_TProgram
#define Uses_TDeskTop
#define Uses_TDialog
#define Uses_TScrollBar
#define Uses_TWindowList
#define Uses_TSCollection
#define Uses_tvutilCommands
#define Uses_tvutilFunctions
#define Uses_TMemoryStream
#include <libtvuti.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <rhutils.h>

TSCollection *windows = NULL;
TSCollection *closed_windows = NULL;
int max_closed = 5;

#define WHERE TProgram::deskTop

class TWindowWatcher:public TView
{
public:
  TWindowWatcher();
  virtual void handleEvent(TEvent &);
};

TWindowWatcher *windowwatcher;

TWindowWatcher::TWindowWatcher():TView(TRect(0, 0, 1, 1))
{
  options |= ofPreProcess;
}

void
TWindowWatcher::handleEvent(TEvent & event)
{
  if (owner->phase != phPreProcess)
    return;
  if (event.what == evCommand && event.message.command == cmClose)
  {
    TWindow *window = (TWindow *) TProgram::deskTop->current;
    int found = 0;
    int i;

    for (i = 0; i < windows->getCount(); i++)
    {
      if (window == DESKTOPWINDOW(i))
      {
        found = 1;
        break;
      }
    }
    if (!found)
      return;
    clearEvent(event);
    if (window->valid(cmClose) == True)
    {
      RemoveWindow(window);
      window->close();
      return;
    }
  }
}

static char *numbers = 0;
static int numbers_count = 0;
static int numbers_size = 0;

#define numbers_block_size 120

void
AddWindow(TWindow * window, TWindow ** ref, Boolean before,
          Boolean remember_closed, Boolean is_removable, int old_number)
{
  int i, count, Number;
  struct stat st;
  DeskTopWindow *window_rec;

  if (!windows)
  {
    windows = new TSCollection();
    TProgram::deskTop->insert((windowwatcher = new TWindowWatcher()));
    windowwatcher->hide();
  }
  count = windows->getCount();
  Number = -1;
  if (old_number && ((old_number > numbers_count) ||
                     (numbers[old_number - 1] == 0)))
  {
    if (old_number > numbers_count)
    {
      while (numbers_size < old_number)
        numbers_size += numbers_block_size;
      numbers = (char *) realloc(numbers, sizeof(char) * numbers_size);;
      memset(numbers + numbers_count, 0, numbers_size - numbers_count);
      numbers_count = old_number;
    }
    numbers[old_number - 1] = 1;
    Number = old_number;
  }
  else
    for (i = 0; i < numbers_count; i++)
    {
      if (!numbers[i])
      {
        Number = i + 1;
        numbers[i] = 1;
        break;
      }
    }
  if (Number == -1)
  {
    if (numbers_count >= numbers_size)
    {
      numbers_size += numbers_block_size;
      numbers = (char *) realloc(numbers, sizeof(char) * numbers_size);
    }
    numbers[numbers_count] = 1;
    Number = numbers_count + 1;
    numbers_count++;
  }
  window->number = Number;
  window_rec = new DeskTopWindow;
  window_rec->window = window;
  window_rec->ref = ref;
  window_rec->full_name = strdup(window->getTitle(1000));
  window_rec->remember_closed = remember_closed;
  window_rec->is_removable = is_removable;
  if (stat(window_rec->full_name, &st) == 0)
  {
    window_rec->dev = st.st_dev;
    window_rec->inode = st.st_ino;
  }
  else
  {
    window_rec->dev = 0;
    window_rec->inode = 0;
  }
  BaseName(window_rec->full_name, window_rec->base_name);
  windows->insert(window_rec);
  if (before == True)
    WHERE->insertBefore(window, WHERE->last);
  else
    WHERE->insert(window);
}

void
UpdateWindow(TWindow * window)
{
  int i, count;

  if (!windows)
    return;
  if (!window)
    return;
  count = windows->getCount();
  for (i = 0; i < count; i++)
  {
    DeskTopWindow *w = (DeskTopWindow *) windows->at(i);

    if (w->window == window)
    {
      string_free(w->full_name);
      string_free(w->base_name);
      string_dup(w->full_name, window->getTitle(1000));
      BaseName(w->full_name, w->base_name);
    }
  }
}

void
AddReference(TWindow ** window)
{
  int i, count;

  if (!windows)
    return;
  if (!window)
    return;
  count = windows->getCount();
  for (i = 0; i < count; i++)
  {
    DeskTopWindow *w = (DeskTopWindow *) windows->at(i);

    if (w->window == *window)
    {
      w->ref = window;
      break;
    }
  }
}

static void
RemoveClosedWindow(int nr)
{
  closed_window *cw = (closed_window *) closed_windows->at(nr);

  string_free(cw->name);
  free(cw->saved_window);
  free(cw);
  closed_windows->atRemove(nr);
}

static void
AddClosedWindow(DeskTopWindow * w)
{
  /*
     Do not add closed 'Untitled' windows 
   */
  if (strcmp(w->window->getTitle(1000), _("Untitled")) == 0)
    return;
  closed_window *cw = new closed_window;

  cw->name = string_dup(w->window->getTitle(1000));
  TMemoryStream ms;

  ms << w->window;
  cw->size = ms.getSize();
  cw->saved_window = (void *) malloc(cw->size);
  cw->is_removable = w->is_removable;
  memcpy(cw->saved_window, ms.getBuffer(), cw->size);
  if (!closed_windows)
    closed_windows = new TSCollection();
  closed_windows->atInsert(0, cw);
  if (max_closed == -1)
    return;
  ccIndex count = closed_windows->getCount();
  ccIndex _count = count;

  while (count > max_closed && _count > 0)
  {
    _count--;
    if (((closed_window *) closed_windows->at(_count))->is_removable == True)
    {
      RemoveClosedWindow(_count);
      count--;
    }
  }
}

void
RemoveWindow(TWindow * window, Boolean remember_closed)
{
  int i, count;

  if (!windows)
    return;
  if (!window)
    return;
  count = windows->getCount();
  for (i = 0; i < count; i++)
  {
    DeskTopWindow *w = (DeskTopWindow *) windows->at(i);

    if (w->window == window)
    {
      numbers[window->number - 1] = 0;
      windows->atRemove(i);
      if (w->remember_closed == True && remember_closed == True)
        AddClosedWindow(w);
      string_free(w->full_name);
      string_free(w->base_name);
      if (w->ref)
        *(w->ref) = NULL;
      delete(w);
      return;
    }
  }
}

void
ClearWindowList()
{
  int i, count;

#if 0
  if (windows)
  {
    count = windows->getCount();
    for (i = count; i > 0; i--)
    {
      DeskTopWindow *w = (DeskTopWindow *) windows->at(i - 1);

      RemoveWindow(w->window, False);
    }
  }
#endif
  if (closed_windows)
  {
    count = closed_windows->getCount();
    for (i = count; i > 0; i--)
    {
      RemoveClosedWindow(i - 1);
    }
  }
}

TWindow *
OpenClosedWindow(int nr)
{
  TWindow *window;
  closed_window *cw = (closed_window *) closed_windows->at(nr);
  TMemoryStream ms(cw->saved_window, cw->size);

  ms >> window;
  AddWindow(window, NULL, False, True);
  RemoveClosedWindow(nr);
  return window;
}

int
IsClosedWindow(const char *name)
{
  if (!closed_windows)
    return -1;
  if (!name)
    return -1;
  int i, count = closed_windows->getCount();

  for (i = 0; i < count; i++)
  {
    if (strcmp(((closed_window *) closed_windows->at(i))->name, name) == 0)
      return i;
  }
  return -1;
}

void
ShowWindowList(const char *title)
{
  TDialog *dialog;
  TWindowList *list;
  TScrollBar *scrollbar;
  int i;
  TWindow *window;

  dialog = new TDialog(TRect(0, 0, 75, 16), title);
  dialog->options |= ofCentered;
  scrollbar = new TScrollBar(TRect(72, 1, 73, 14));
  list = new TWindowList(TRect(1, 1, 72, 14), 1, scrollbar,
                         windows, closed_windows);
  dialog->insert(list);
  dialog->insert(scrollbar);
  do
  {
    TProgram::deskTop->insert(dialog);
    dialog->setState(sfModal, True);
    i = dialog->execute();
    TProgram::deskTop->remove(dialog);
    switch (i)
    {
      case cmSelectClosedWindow:
        window = OpenClosedWindow(list->focused - windows->getCount());
        window->select();
        window->show();
        message(TProgram::application, evBroadcast, cmWindowOpened,
                (void *) window->getTitle(1000));
        i = cmCancel;
        break;
      case cmSelectWindow:
        if (list->range == 0)
          break;
        window = DESKTOPWINDOW(list->focused);
        window->select();
        window->show();
        i = cmCancel;
        break;
      case cmDeleteWindow:
      {
        if (list->range == 0)
          break;
        window = DESKTOPWINDOW(list->focused);
        if (window->valid(cmClose) == True)
        {
          RemoveWindow(window);
          destroy(window);
          list->update(windows, closed_windows);
        }
        break;
      }
      case cmDeleteClosedWindow:
      {
        int window_count = 0;

        if (list->range == 0)
          break;
        if (!closed_windows)
          break;
        if (windows)
          window_count = windows->getCount();
        RemoveClosedWindow(list->focused - window_count);
        list->update(windows, closed_windows);
        break;
      }
      default:
        i = cmCancel;
    }
  }
  while (i != cmCancel);
  destroy(dialog);
}

static ushort closed_window_version = 0;

static void
SaveClosedWindow(int nr, opstream & os)
{
  closed_window *cw = (closed_window *) closed_windows->at(nr);

  os << closed_window_version;
  os.writeString(cw->name);
  os << cw->size;
  os.writeBytes(cw->saved_window, cw->size);
}

static ushort closed_version;

void
SaveClosedWindows(opstream & os)
{
  ccIndex count = 0;
  int i;

  os << closed_version;
  if (closed_windows)
    count = closed_windows->getCount();
  os << count;
  for (i = 0; i < count; i++)
  {
    SaveClosedWindow(i, os);
  }
}

static void
LoadClosedWindow(ipstream & is)
{
  ushort version;

  is >> version;
  closed_window *cw = new closed_window;

  cw->name = is.readString();
  is >> cw->size;
  cw->saved_window = (void *) malloc(cw->size);
  is.readBytes(cw->saved_window, cw->size);
  closed_windows->insert(cw);
}

void
LoadClosedWindows(ipstream & is)
{
  ushort version;
  int i;

  is >> version;
  ccIndex count;

  if (!closed_windows)
    closed_windows = new TSCollection();
  is >> count;
  for (i = 0; i < count; i++)
  {
    LoadClosedWindow(is);
  }
}


/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#define Uses_tvgdbFunctions
#include <libtvgdb.h>

#define Uses_TDialog
#define Uses_TProgram
#define Uses_TDeskTop
#define Uses_TScrollBar

#define Uses_TEnterListBox
#define Uses_tvutilCommands
#define Uses_tvutilFunctions
#include <libtvuti.h>

#include <rhutils.h>

#include <librhgdb.h>

#include <stdio.h>

class TCallStackListBox:public TEnterListBox
{
public:
  TCallStackListBox(const TRect & bounds, ushort numcols,
                    TScrollBar * vbar):TEnterListBox(bounds, numcols, vbar)
  {
  }
  virtual void selectItem(ccIndex item);
  virtual void focusItem(ccIndex item);
  virtual void getText(char *dest, ccIndex item, short maxLen);
  void update();
};

static int in_select = 0;
static int in_focus = 0;

void
TCallStackListBox::selectItem(ccIndex item)
{
  frame_entry *fe;

  if (item >= frame_count)
    return;
  SetFrame(item);
  in_select++;
  TEnterListBox::focusItem(item);
  TEnterListBox::selectItem(item);
  in_select--;
  if (in_select)
    return;
  fe = frames[item];
  if (!fe->file_name)
    return;
  if (message
      (TProgram::application, evBroadcast, cmOpenWindow, fe->file_name))
  {
    if (fe->line_number >= 0)
    {
      message(TProgram::application, evBroadcast, cmGotoWindowLine,
              (void *) fe->line_number);
    }
  }
}

void
TCallStackListBox::focusItem(ccIndex item)
{
  frame_entry *fe;

  if (item >= frame_count)
    return;
  SetFrame(item);
  in_focus++;
  TEnterListBox::focusItem(item);
  in_focus--;
  if (in_focus || in_select)
    return;
  fe = frames[item];
  if (!fe->file_name)
    return;
  if (message
      (TProgram::application, evBroadcast, cmFocusWindow, fe->file_name))
  {
    if (fe->line_number >= 0)
    {
      message(TProgram::application, evBroadcast, cmGotoWindowLine,
              (void *) fe->line_number);
    }
  }
}

void
TCallStackListBox::getText(char *dest, ccIndex item, short maxlen)
{
  int len = 0;
  char *tmp;
  frame_entry *fe;

  if (item >= frame_count)
  {
    *dest = 0;
    return;
  }
  fe = frames[item];
  if (fe->file_name)
  {
    char *bname;

    BaseName(fe->file_name, bname);
    if (fe->line_number >= 0)
    {
      char temp[16];

      sprintf(temp, ":%d: ", fe->line_number);
      string_cat(bname, temp);
    }
    else
    {
      char temp[16];

      sprintf(temp, ":0x%08lx: ", fe->address);
      string_cat(bname, temp);
    }
    tmp = bname;
    while (*tmp && len < maxlen)
    {
      dest[len++] = *tmp++;
    }
    dest[len] = 0;
    string_free(bname);
  }
  else
  {
    char temp[16];

    sprintf(temp, "0x%08lx: ", fe->address);
    tmp = temp;
    while (*tmp && len < maxlen)
    {
      dest[len++] = *tmp++;
    }
    dest[len] = 0;
  }
  if (fe->function_name)
  {
    tmp = fe->function_name;
    while (*tmp && len < maxlen)
    {
      dest[len++] = *tmp++;
    }
    dest[len] = 0;
  }
  if (fe->args)
  {
    tmp = fe->args;
    while (*tmp && len < maxlen)
    {
      dest[len++] = *tmp++;
    }
    dest[len] = 0;
  }
}

void
TCallStackListBox::update()
{
  BackTrace();
// should we do  SetFrame(0);
  setRange(frame_count);
//  if (frame_count > 0)
//    focusItem(0);
  drawView();
}

class TCallStackWindow:public TDialog
{
public:
  TCallStackWindow(const TRect &);
  TCallStackListBox *list;
  virtual void changeBounds(const TRect &);
};

TCallStackWindow::TCallStackWindow(const TRect & r):
TDialog(r, _("Function call stack")), TWindowInit(TCallStackWindow::initFrame)
{
  TRect rr = getExtent();
  TScrollBar *bar;

  rr.grow(-1, -1);
  bar = standardScrollBar(sbVertical | sbHandleKeyboard);
  list = new TCallStackListBox(rr, 1, bar);
  insert(list);
  growMode = gfGrowLoY | gfGrowHiX | gfGrowHiY;
  list->growMode = gfGrowHiX | gfGrowHiY;
  flags |= wfGrow | wfZoom;
}

TDialog *CallStackWindow = NULL;
TRect CallStackWindowRect(-1, -1, -1, -1);

void
TCallStackWindow::changeBounds(const TRect & r)
{
  TDialog::changeBounds(r);
  CallStackWindowRect = r;
}

void
ShowCallStackWindow()
{
  if (!CallStackWindow)
  {
    if (CallStackWindowRect.a.x == -1)
    {
      CallStackWindowRect = TProgram::deskTop->getExtent();
      CallStackWindowRect.a.y = CallStackWindowRect.b.y - 7;
    }
    CallStackWindow = new TCallStackWindow(CallStackWindowRect);
    AddWindow(CallStackWindow, (TWindow **) & CallStackWindow);
  }
  UpdateCallStackWindow();
  CallStackWindow->select();
}

void
UpdateCallStackWindow()
{
  if (!CallStackWindow)
    return;
  ((TCallStackWindow *) CallStackWindow)->list->update();
}

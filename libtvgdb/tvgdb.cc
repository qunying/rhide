/* Copyright (C) 1996-2000 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#define Uses_tvutilFunctions
#include <libtvuti.h>

#define Uses_TDataWindow
#include <libtvgdb.h>

void
AddDataWindow()
{
  TDataWindow *data_win;

  if ((data_win = TDataWindow::createNew()))
    AddWindow(data_win);
}

static TDataWindow *stack_win = NULL;

void
ShowStackWindow()
{
  if (!stack_win)
  {
    if ((stack_win = TDataWindow::stackWindow()))
      AddWindow(stack_win, (TWindow **) & stack_win);
  }
  else
    stack_win->select();
}

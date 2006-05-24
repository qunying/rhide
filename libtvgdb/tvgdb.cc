/* Copyright (C) 1996-2000 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#define Uses_tvutilFunctions
#include <libtvuti.h>

#define Uses_MsgBox
#define Uses_TDataWindow
#include <libtvgdb.h>
#include <sys/wait.h> // WIFEXITED

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

void ShowExitCode(int exit_code)
{
  #if defined(WIFEXITED)
  if (WIFEXITED(exit_code))
    messageBox(mfInformation | mfOKButton,
    _("Program exited normally with code: %d"), WEXITSTATUS(exit_code));
  else if (WIFSIGNALED(exit_code))
    messageBox(mfInformation | mfOKButton,
    _("Program was terminated by signal: %d"), WTERMSIG(exit_code));
  else if (WIFSTOPPED(exit_code))
    messageBox(mfInformation | mfOKButton,
    _("Program was stopped by signal: %d"), WSTOPSIG(exit_code));
  #else
  messageBox(mfInformation | mfOKButton,
             _("Program exit code: %d (0x%04x)"), exit_code, exit_code);
  #endif
}


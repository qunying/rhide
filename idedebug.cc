/* Copyright (C) 1996-2000 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#define Uses_TEventQueue
#define Uses_TProject
#define Uses_TProgram
#define Uses_TDeskTop
#define Uses_TIDEEditWindow
#define Uses_TIDEFileEditor
#define Uses_editCommands
#define Uses_TOptions
#define Uses_TDirList
#define Uses_TParamList
#define Uses_TScreen
#define Uses_TFileViewer
#define Uses_TWindow
#define Uses_MsgBox
#define Uses_TListBox
#define Uses_TFileDialog
#define Uses_TFileInputLine
#define Uses_ideFunctions
#define Uses_ideCommands
#include <libide.h>

#define Uses_TSCollection
#define Uses_BigMessageBox
#define Uses_tvutilFunctions
#include <libtvuti.h>

#include <rhutils.h>

#define Uses_IDEConst
#include "rhide.h"
#include "rhidehis.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

TCEditWindow *current_editor;
uint32 CPULine;

#ifdef INTERNAL_DEBUGGER
#define Uses_tvgdbFunctions
#include <librhgdb.h>

#define Uses_TWatchListBox
#define Uses_TDisassemblerWindow
#define Uses_TDataWindow
#include <libtvgdb.h>

static void RemoveSessionTempFiles(void);

TDisassemblerWindow *dis_win;

static void
UserScreen()
{
  TProgram::application->suspend();
}

static void
DebuggerScreen()
{
  TProgram::application->resume();
  Repaint();
}

void
OpenDisWin(int force_open)
{
  if (!dis_win && (force_open || AutomaticOpenDisass))
  {
    extern unsigned long stop_pc;
    TRect r(1, 1, 79, 23);

    dis_win = new TDisassemblerWindow(r, _("Disassembler window"));
    AddWindow(dis_win, (TWindow **) & dis_win);
    if (debugger_started())
      dis_win->update(stop_pc);
  }
  else if (dis_win)
    if (TProgram::application->deskTop->indexOf(dis_win))
    {
      dis_win->select();
    }
}

static void
select_source_line(char *fname, int line, char *dirname, char *fullname)
{
  if (!debugger_started())
    return;
  int select_dis_win = dis_win && TProgram::deskTop->current == dis_win;
  char *full_name = NULL;
  static char *lastSkippedName = 0;

  TProgram::deskTop->lock();
  // remove at first the last CPU line
  if (current_editor)
  {
    CPULine = (uint32) - 1;
    current_editor->editor->update(ufView);
  }
  if (!fname)
  {
    OpenDisWin(force_disassembler_window);
    force_disassembler_window = 0;
    if (!dis_win)
      goto end;
    select_dis_win = 1;
  }
  else
  {
    bool found = false;
    if ((found = FindFile(fullname, full_name)) == false)
    {
      string_free(full_name);
      found = FindFile(fname, full_name);
    }
    if (!found && dirname)
    {
      char *tmp = string_dup(dirname);
      string_cat(tmp, fname);
      found = FindFile(tmp, full_name);
      string_free(tmp);
      if (!found)
        string_free(full_name);
    }
    current_editor = is_on_desktop(fname, False);
    if (!current_editor && !found)
    {
      if (lastSkippedName)
      {
        if (strcmp(lastSkippedName, fname) == 0)
        {
          OpenDisWin(force_disassembler_window);
          goto end;
        }
        else
        {
          string_free(lastSkippedName);
          lastSkippedName = 0;
        }
      }
      ushort result;
      char *bname;

      BaseName(fname, bname);
      TProgram::deskTop->unlock();
      messageBox(mfError | mfOKButton, _("Could not find the source "
                                         "file %s."), bname);
      string_free(full_name);
      TFileDialog *dialog;

      InitHistoryID(RHIDE_History_source_file);
      string_cat(bname, "*");   /*
                                   Let's add wildcard not let TFileDialog  
                                 */
      /*
         to exit when simply enter is pressed  
       */
      dialog = new TFileDialog(bname, _("Open a file"),
                               _("~N~ame"), fdOpenButton,
                               RHIDE_History_source_file);
      result = TProgram::deskTop->execView(dialog);
      TProgram::deskTop->lock();
      string_free(bname);
      if (result != cmCancel)
      {
        full_name = (char *) malloc(PATH_MAX);
        dialog->getData(full_name);
        FExpand(full_name);
        char *dir, *name, *ext;

        split_fname(full_name, dir, name, ext);
        string_free(name);
        string_free(ext);
        if (*dir)
          dir[strlen(dir) - 1] = 0;
        TProgram::deskTop->unlock();
        if (messageBox(mfConfirmation | mfYesNoCancel,
                       _("Add '%s' to the search path for source files"),
                       dir) == cmYes)
        {
          Options.SrcDirs->insert(strdup(dir));
        }
        TProgram::deskTop->lock();
        string_free(dir);
        if (lastSkippedName)
        {
          string_free(lastSkippedName);
          lastSkippedName = NULL;
        }
      }
      else
      {
        current_editor = NULL;
        if (AutomaticOpenDisass)
        {
          lastSkippedName = strdup(fname);
          OpenDisWin(force_disassembler_window);
        }
        goto end;
      }
    }
    if (!current_editor)
      OpenEditor(full_name, True, current_editor);
    current_editor->select();
    string_free(full_name);
    CPULine = line - 1;
    current_editor->editor->MoveCursorTo(0, CPULine);
    current_editor->editor->trackCursor(False);
    /*
       If the line is at the first or last screen line, try to
       center it 
     */
    current_editor->editor->update(ufView);
    if (current_editor->editor->cursor.y == 0 ||
        current_editor->editor->cursor.y ==
        current_editor->editor->size.y - 1)
    {
      current_editor->editor->trackCursor(True);
      current_editor->editor->update(ufView);
    }
  }
end:
  if (dis_win)
  {
    extern unsigned long stop_pc;

    dis_win->update(stop_pc);
  }
  if (select_dis_win)
    dis_win->select();
  TProgram::deskTop->unlock();
}

const char *
WhereIsCursor(int &line, int &column, char *&bname, TCEditWindow * &ew)
{
  const char *file;

  ew = (TCEditWindow *) TProgram::deskTop->current;
  if (!ew)
    return NULL;
  TEvent event;

  event.what = evBroadcast;
  event.message.command = cmEditorAnswer;
  ew->handleEvent(event);
  if (event.what != evNothing)
    return NULL;
  file = ew->editor->fileName;
  BaseName(file ? file : _("Untitled"), bname);
  line = ew->editor->curPos.y + 1;
  column = ew->editor->curPos.x + 1;
  return file;
}

void
GOTO(int _switch_to_user)
{
  int line, column;
  const char *file;
  char *bname;
  TCEditWindow *ew;

  if (dis_win && TProgram::deskTop->current == dis_win)
  {
    unsigned long address = dis_win->focused_address();

    switch_to_user = _switch_to_user;
    GoToAddress(address);
  }
  else
  {
    file = WhereIsCursor(line, column, bname, ew);
    if (!file)
      return;
    switch_to_user = _switch_to_user;
    GoToLine(bname, line);
  }
}

void
RESET()
{
  ResetDebugger();

  if (ShowStdout)
    close_stdout();
  if (ShowStderr)
    close_stderr();
  RemoveSessionTempFiles();

  if (project_directory)
    chdir(project_directory);
  Repaint();
}

int
DEBUGGER_STARTED()
{
  return debugger_started();
}

void
TRACE(int _switch_to_user)
{
  TWindow *w = (TWindow *) TProgram::deskTop->current;

  switch_to_user = _switch_to_user;
  if (dis_win && TProgram::deskTop->current == dis_win)
  {
    Trace_i();
  }
  else
    Trace();
  if (w && w == watchwindow)
    watchwindow->select();
}

void
STEP(int _switch_to_user)
{
  TWindow *w = (TWindow *) TProgram::deskTop->current;

  switch_to_user = _switch_to_user;
  if (dis_win && TProgram::deskTop->current == dis_win)
  {
    Step_i();
  }
  else
    Step();
  if (w && w == watchwindow)
    watchwindow->select();
}

void
CONTINUE()
{
  switch_to_user = 1;
  Continue();
}

void
FINISH()
{
  switch_to_user = 1;
  Finish();
}

static unsigned long last_modified = 0;

static int
DoMake()
{
  if (debugger_started())
  {
    if (!hasmodified && already_maked)
    {
      last_modified = 0;
      return 1;
    }
    if (already_maked && hasmodified)
    {
      ushort retval;

      if (last_modified == hasmodified)
        return 1;
      retval = messageBox(_("Source has been modified. Rebuild it?"),
                          mfWarning | mfYesButton | mfNoButton);
      if (retval == cmNo)
      {
        last_modified = hasmodified;
        return 1;
      }
      else if (retval == cmCancel)
      {
        return 0;
      }
    }
  }
  last_modified = 0;
  ShowMessages(NULL, True);
  return (Make(False) == False) ? 0 : 1;
}

static char *
GetProgName()
{
  return (char *) FName(Project.dest_name);
}

static char **
GetSourceDirectories(int *count)
{
  *count = 0;
  return NULL;
}

static char **args = NULL;

static char **
GetProgramArguments(int *count)
{
  int i;

  *count = Options.ProgArgs->getCount();
  if (*count == 0)
    return NULL;
  args = (char **) realloc(args, *count * sizeof(char *));

  for (i = 0; i < *count; i++)
  {
    args[i] = (char *) Options.ProgArgs->at(i);
  }
  return args;
}

static void
UPDATE_WATCH()
{
  if (watchwindow)
    watches->update();
  UpdateCallStackWindow();
  UpdateDataWindows();
}

int dual_display_supported();

static void
StartSession()
{
  if (!dual_display && UseDualDisplay && dual_display_supported())
  {
    TMouse::suspend();
    TScreen::suspend();
    dual_display = 1;
    TScreen::resume();
    TMouse::resume();
    TProgram::application->setScreenMode(Project.screen_mode);
  }
  if (ShowStdout)
    cpp_outname = open_stdout();
  if (ShowStderr)
    cpp_errname = open_stderr();
  if (watchwindow)
    watchwindow->select();
}

static void
RemoveSessionTempFiles(void)
{
  RemoveStdout();
  RemoveStderr();
}

static void
EndSession(int exit_code)
{
  if (dual_display && UseDualDisplay)
  {
    TMouse::suspend();
    TScreen::suspend();
    dual_display = 0;
    TScreen::resume();
    TMouse::resume();
    TProgram::application->setScreenMode(Project.screen_mode);
  }

  if (ShowStdout)
  {
    close_stdout();
  }
  if (ShowStderr)
  {
    close_stderr();
  }
  if (!DontShowExitCode)
    messageBox(mfInformation | mfOKButton,
               _("Program exit code: %d (0x%04x)"), exit_code, exit_code);

  if (ShowUserAfterExit)
  {
    TEvent event;

    event.what = evCommand;
    event.message.command = cmUserScreen;
    TProgram::application->handleEvent(event);
  }

  if (ShowStdout)
  {
    CheckStdout(false);
  }
  if (ShowStderr)
  {
    CheckStderr(false);
  }
  SetPostCommandHook(RemoveSessionTempFiles);
  Repaint();
  external_program_executed = 1;
  if (project_directory)
    chdir(project_directory);
}

static void
BreakSession()
{
  if (dual_display && UseDualDisplay)
  {
    TMouse::suspend();
    TScreen::suspend();
    dual_display = 0;
    TScreen::resume();
    TMouse::resume();
    TProgram::application->setScreenMode(Project.screen_mode);
  }
  if (ShowStdout)
  {
    close_stdout();
    CheckStdout();
  }
  if (ShowStderr)
  {
    close_stderr();
    CheckStderr();
  }
  Repaint();
  external_program_executed = 1;
}

static char *
GetMainFunction()
{
  return Project.main_function;
}

void
InitDebuggerInterface()
{
  _select_source_line = select_source_line;
  _UserWarning = UserWarning;
  _DebuggerScreen = DebuggerScreen;
  _UserScreen = UserScreen;
  _GetProgramArguments = GetProgramArguments;
  _GetSourceDirectories = GetSourceDirectories;
  _GetProgName = GetProgName;
  _DoMake = DoMake;
  _InitGDBOutWin = InitGDBOutWin;
  _UpdateGDBOutWin = UpdateGDBOutWin;
  _StartSession = StartSession;
  _EndSession = EndSession;
  _BreakSession = BreakSession;
  _GetMainFunction = GetMainFunction;
  post_command_hook = UPDATE_WATCH;
}

void
ToggleBreak()
{
  const char *fname;
  char *bname;
  int break_number, line, column;
  TCEditWindow *ew;

  if ((fname = WhereIsCursor(line, column, bname, ew)) == NULL)
    return;
  break_number = IsBreakPointLine(bname, line);
  if (break_number >= 0)
    RemoveBreakPoint(break_number);
  else
    AddBreakPointLine(bname, line);
  Repaint();
}

void CLEARSYMBOLS()
{
  ClearSymbols();
}

#else
void
GOTO(int)
{
}

void
STEP(int)
{
}
void
TRACE(int)
{
}
void
RESET()
{
  if (project_directory)
    chdir(project_directory);
}

void
CONTINUE()
{
}
void
FINISH()
{
}
int
DEBUGGER_STARTED()
{
  return 0;
}
void
CLEARSYMBOLS()
{
}
#endif


/* Copyright (C) 1996-2002 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#ifdef __DJGPP__
#include <go32.h>
#include <dpmi.h>
#include <dos.h>
#include <setjmp.h>
#include <sys/farptr.h>
#include <sys/exceptn.h>
#include <sys/movedata.h>
#include <crt0.h>
#endif // __DJGPP__
#include <dirent.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <setjmp.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <locale.h>
#include <time.h>
#include <glob.h>

#define Uses_TApplication
#define Uses_TMenuBar
#define Uses_TStaticText
#define Uses_TKeys
#define Uses_editCommands
#define Uses_TDeskTop
#define Uses_fpstream
#define Uses_TDialog
#define Uses_MsgBox
#define Uses_TScreen
#define Uses_TMouse
#define Uses_TFileDialog
#define Uses_TFileInputLine
#define Uses_TValidator
#define Uses_TDirList
#define Uses_TButton
#define Uses_TEventQueue
#define Uses_TFileViewer
#define Uses_TSortedListBox
#define Uses_TStringCollection
#define Uses_TGKey
#define Uses_TFileCollection

#define Uses_TProject
#define Uses_TOptions
#define Uses_TDepCollection
#define Uses_TMemInfo
#define Uses_IDEConst
#define Uses_TIDEEditWindow
#define Uses_TIDEFileEditor
#define Uses_ideFunctions
#define Uses_ideCommands
#define Uses_TCEditor_Internal
#define Uses_TCEditor_Commands
#define Uses_TStringCollection
#define Uses_TNoCaseStringCollection
#define Uses_TDialog
#define Uses_TScroller
#include <libide.h>
#include <inf.h>

#define Uses_TInputLinePiped
#define Uses_TCEditor
#include <ceditor.h>
#include <edprint.h>
#if (TCEDITOR_VERSION >= 0x000447UL)
#include <loadshl.h>
#endif

#ifdef INTERNAL_DEBUGGER
#include <librhgdb.h>

#define Uses_tvgdbFunctions
#define Uses_tvgdbCommands
#define Uses_TInspector
#include <libtvgdb.h>
#include <tvgdbhis.h>
#endif

#define Uses_TSCollection
#define Uses_TWindowList
#define Uses_tvutilCommands
#define Uses_TCheckDialog
#define Uses_THintStatusLine
#define Uses_TMsgCollection
#define Uses_MsgRec
#define Uses_tvutilFunctions
#include <libtvuti.h>

#include <rhutils.h>

#include "rhide.h"
#include "rhidehis.h"

#include <libtvdem.h>

#include "ideapp.h"
#ifdef __linux__
//FIXME: on my Linux the symbol ERR is already defined
// in sys/ucontext.h, but since curses.h is needed only
// for timeout(), I think I can live with it.
#undef ERR
#include <curses.h>
#endif

#include <slpinter.h>
#include <loadkbin.h>
#define KeyBindFName "rhide.key"

#include <edhists.h>

void SaveScreen();
void RestoreScreen();
static int keep_temp_dir = 0;

#define DELTA(x) (*((long *)(x)))

static void
IDEPrintEditor(TCEditor * e)
{
  if (e->IslineInEdition)
    e->MakeEfectiveLineInEdition();
  e->buffer[e->bufLen] = 0;
  char *s = strrchr(e->fileName, '/');

  if (!s)
    s = e->fileName;
  else
    s++;
  PrintSource(e->buffer, s, e->tabSize);
}

int global_argc = 0;
char **global_argv = NULL;

TIDEMemInfo *mem_info = NULL;

#define MemInfoWidth 10

static void
start_mem_info()
{
  TRect r = TProgram::menuBar->getExtent();
  int start = r.a.x;

  r.a.x = r.b.x - MemInfoWidth;
  mem_info = new TIDEMemInfo(r);
  mem_info->growMode = gfGrowLoX;
  TProgram::application->insert(mem_info);
  // Reduce the menubar
  r.b.x = r.a.x;
  r.a.x = start;
  TProgram::menuBar->changeBounds(r);
}

static void
end_mem_info()
{
  TProgram::application->remove(mem_info);
  destroy(mem_info);
  mem_info = NULL;
  // Enlarge the menuBar again
  TRect r = TProgram::menuBar->getExtent();

  r.b.x += MemInfoWidth;
  TProgram::menuBar->changeBounds(r);
}

void
update_mem(int force = 0)
{
  if (!project)
    return;
  if (!ShowMem && mem_info)
  {
    end_mem_info();
  }
  if (ShowMem)
  {
    if (!mem_info)
    {
      start_mem_info();
    }
    mem_info->update(force);
  }
}


void
IDE::changeBounds(const TRect & bounds)
{
  TApplication::changeBounds(bounds);
  if (mem_info)
  {
    end_mem_info();
    start_mem_info();
  }
}

static void
update_words()
{
  int i, count, repaint = 0;

  if (!project)
    return;
  if (c_words_changed)
  {
    if (ReservedWords)
      destroy(ReservedWords);
    count = CReservedWords->getCount();
    ReservedWords = new TStringCollection(count, 1);
    for (i = 0; i < count; i++)
    {
      ReservedWords->insert(strdup((char *) CReservedWords->at(i)));
    }
    repaint = 1;
  }
  if ((gpc_words_changed) && (!UseFPC))
  {
    if (PascalRWords)
      destroy(PascalRWords);
    count = GPCReservedWords->getCount();
    PascalRWords = new TStringCollection(count, 1);
    for (i = 0; i < count; i++)
    {
      char *tmp = strdup((char *) GPCReservedWords->at(i));

      string_down(tmp);
      PascalRWords->insert(tmp);
    }
    repaint = 1;
  }
  if ((fpc_words_changed) && (UseFPC))
  {
    if (PascalRWords)
      destroy(PascalRWords);
    count = FPCReservedWords->getCount();
    PascalRWords = new TStringCollection(count, 1);
    for (i = 0; i < count; i++)
    {
      char *tmp = strdup((char *) FPCReservedWords->at(i));

      string_down(tmp);
      PascalRWords->insert(tmp);
    }
    repaint = 1;
  }
  if (user_words_changed)
  {
    if (UserWords)
      destroy(UserWords);
    count = RHIDEUserWords->getCount();
    UserWords = new TStringCollection(count, 1);
    for (i = 0; i < count; i++)
    {
      UserWords->insert(strdup((char *) RHIDEUserWords->at(i)));
    }
    repaint = 1;
  }
  if (c_words_changed || gpc_words_changed ||
      fpc_words_changed || user_words_changed)
  {
    CreateSHShortCutTables();
  }
  c_words_changed =
    gpc_words_changed = fpc_words_changed = user_words_changed = 0;
  if (repaint)
    Repaint();
}

int external_program_executed = 0;

void
check_for_external_modifications()
{
  if (windows)
  {
    int count = windows->getCount();
    int i;
    TWindow *window;

    for (i = 0; i < count; i++)
    {
      TEvent event;

      window = DESKTOPWINDOW(i);
      event.what = evBroadcast;
      event.message.command = cmEditorAnswer;
      window->handleEvent(event);
      if (event.what == evNothing)
      {
        unsigned long ft;

#define IE ((TIDEFileEditor *)(((TCEditWindow *)window)->editor))
        ft = time_of_file(IE->fileName);
        if (ft > IE->edittime)
        {
          if (messageBox(mfYesNoCancel | mfInformation,
                         _("%s has been modified! Reload it?"),
                         IE->fileName) == cmYes)
          {
            IE->lock();
            IE->loadFile();
            IE->update(ufView);
            IE->edittime = ft;
            already_maked = 0;
            IE->unlock();
          }
        }
#undef IE
      }
    }
  }
}

#define D(x) disableCommand(x)
#define E(x) enableCommand(x)

static int should_update = 1;
static unsigned long last_clock;
static int last_diff = 1;

void
IDE::update()
{
  unsigned i;
  struct
  {
    unsigned _has_editors:1;
    unsigned _has_project:1;
    unsigned _current_is_editor:1;
    unsigned _current_is_clip:1;
    unsigned _current_is_fileview:1;
    unsigned _current_is_project:1;
    unsigned _target_is_exec:1;
    unsigned _current_is_help:1;
    unsigned _has_primary_file:1;
    unsigned _dummy:23;
  }
  _flags;

  *((uint32 *) & _flags) = 0;
#define has_editors _flags._has_editors
#define has_project _flags._has_project
#define current_is_editor _flags._current_is_editor
#define current_is_clip _flags._current_is_clip
#define current_is_fileview _flags._current_is_fileview
#define current_is_project _flags._current_is_project
#define target_is_exec _flags._target_is_exec
#define current_is_help _flags._current_is_help
#define has_primary_file _flags._has_primary_file
  unsigned project_count = 0;
  unsigned has_windows = 0;
  unsigned has_closed_windows = 0;
  TEvent event;
  unsigned long _clock = time(NULL);

  if (!should_update)
  {
    if (difftime(_clock, last_clock) > last_diff)
    {
      update_mem();
      last_clock = _clock;
      if (last_diff < 10)
        last_diff++;
    }
    return;
  }
  else
  {
    last_clock = _clock;
    last_diff = 1;
  }
  update_mem();
  update_words();

  has_primary_file = project ? Project.source_name != NULL : 0;

  if (windows)
    has_windows = windows->getCount();
  if (closed_windows)
    has_closed_windows = closed_windows->getCount();
  if (has_windows)
  {
    for (i = 0; i < has_windows; i++)
    {
      event.what = evBroadcast;
      event.message.command = cmEditorAnswer;
      DESKTOPWINDOW(i)->handleEvent(event);
      if (event.what == evNothing)
      {
        has_editors = 1;
      }
    }
    // clipwindow ???
    if ((TView *) TProgram::deskTop->current == (TView *) clipWindow)
      current_is_clip = 1;
    else if ((TView *) TProgram::deskTop->current == (TView *) project_window)
      current_is_project = 1;
    else
    {
      event.what = evBroadcast;
      event.message.command = cmEditorAnswer;
      TProgram::deskTop->current->handleEvent(event);
      if (event.what == evNothing)
        current_is_editor = 1;
      else
      {
        event.what = evBroadcast;
        event.message.command = cmFileViewAnswer;
        TProgram::deskTop->current->handleEvent(event);
        if (event.what == evNothing)
          current_is_fileview = 1;
        else
        {
          event.what = evBroadcast;
          event.message.command = cmInfoAnswer;
          TProgram::deskTop->current->handleEvent(event);
          if (event.what == evNothing)
            current_is_help = 1;
        }
      }
    }
  }
  if (project)
    if (Project.dest_file_type == FILE_COFF ||
        Project.dest_file_type == FILE_EXE || has_primary_file)
      target_is_exec = 1;
  if (project_name)
  {
    has_project = 1;
    if (Project.dependencies)
      project_count = Project.dependencies->getCount();
  }

  if (!has_project)
  {
    D(cmCloseProject);
    D(cmAddProjectItem);
    D(cmDelProjectItem);
    D(cmLocalOptions);
    D(cmShowIncludes);
    D(cmClearDependencies);
    D(cmMakeClear);
    D(cmShowProject);
    D(cmWriteMake);
  }
  else
  {
    E(cmShowProject);
    E(cmCloseProject);
    E(cmAddProjectItem);
    E(cmWriteMake);
    if (!project_count)
    {
      D(cmDelProjectItem);
      D(cmLocalOptions);
      D(cmShowIncludes);
      D(cmClearDependencies);
      D(cmMakeClear);
    }
    else
    {
      E(cmDelProjectItem);
      E(cmLocalOptions);
      E(cmShowIncludes);
      E(cmClearDependencies);
      E(cmMakeClear);
    }
  }

  if (!has_windows)
  {
    D(cmTile);
    D(cmCascade);
    D(cmNext);
    D(cmPrev);
    if (!has_closed_windows)
      D(cmShowWindowList);
    else
      E(cmShowWindowList);
  }
  else
  {
    E(cmTile);
    E(cmCascade);
    E(cmNext);
    E(cmPrev);
    E(cmShowWindowList);
  }

  if (!target_is_exec)
  {
    D(cmRun);
    D(cmStep);
    D(cmTrace);
    D(cmGoto);
    D(cmStepNo);
    D(cmTraceNo);
    D(cmGotoNo);
    D(cmFinish);
    D(cmReset);
    D(cmProgArgs);
    D(cmToggleBreak);
    D(cmAddWatchEntry);
    D(cmBreakPoints);
    D(cmFunctionList);
    D(cmCallStack);
    D(cmInspect);
    D(cmAddDataWindow);
    D(cmShowStackWindow);
  }
  else
  {
    if (current_is_editor)
    {
      E(cmToggleBreak);
      E(cmGoto);
      E(cmGotoNo);
      E(cmcProfileEditor);
      /*
         Rectangular block commands  
       */
      E(cmcSelRectStart);
      E(cmcSelRectEnd);
      E(cmcSelRectHide);
      E(cmcSelRectCopy);
      E(cmcSelRectPaste);
      E(cmcSelRectCut);
      E(cmcSelRectDel);
      E(cmcSelRectMove);
    }
    else
    {
      D(cmToggleBreak);
      D(cmGoto);
      D(cmGotoNo);
      D(cmcProfileEditor);

      /*
         Rectangular block commands  
       */
      D(cmcSelRectStart);
      D(cmcSelRectEnd);
      D(cmcSelRectHide);
      D(cmcSelRectCopy);
      D(cmcSelRectPaste);
      D(cmcSelRectCut);
      D(cmcSelRectDel);
      D(cmcSelRectMove);
    }
    if (!has_project && !has_editors)
    {
      D(cmAddWatchEntry);
      D(cmBreakPoints);
      D(cmRun);
      D(cmStep);
      D(cmTrace);
      D(cmGoto);
      D(cmStepNo);
      D(cmTraceNo);
      D(cmGotoNo);
      D(cmFinish);
      D(cmReset);
      D(cmProgArgs);
      D(cmFunctionList);
      D(cmInspect);
      D(cmAddDataWindow);
      D(cmShowStackWindow);
    }
    else
    {
      E(cmAddWatchEntry);
      E(cmBreakPoints);
      E(cmRun);
      E(cmStep);
      E(cmTrace);
      E(cmGoto);
      E(cmStepNo);
      E(cmTraceNo);
      E(cmGotoNo);
      E(cmFinish);
      E(cmReset);
      E(cmProgArgs);
      E(cmFunctionList);
      E(cmCallStack);
      E(cmInspect);
      E(cmAddDataWindow);
      E(cmShowStackWindow);
    }
  }

  if (!has_primary_file &&
      ((!has_project && !has_editors) || (has_project && !project_count)))
  {
    D(cmMake);
    D(cmBuild);
    D(cmLink);
    D(cmRun);
    D(cmGDB);
    D(cmFSDB);
  }
  else
  {
    E(cmMake);
    E(cmBuild);
    E(cmLink);
    if (!target_is_exec)
    {
      D(cmRun);
      D(cmGDB);
      D(cmFSDB);
    }
    else
    {
      E(cmRun);
      E(cmGDB);
      E(cmFSDB);
    }
  }

  if (current_is_editor)
  {
    E(cmSaveEditor);
    E(cmJumpToFunction);
    E(cmPrint);
  }
  else
  {
    D(cmSaveEditor);
    D(cmJumpToFunction);
    D(cmPrint);
  }

  if (current_is_editor || current_is_clip)
  {
    E(cmSyntaxHelp);
    E(cmcExpandAllTabs);
    E(cmcCompactBuffer);
    E(cmcFind);
    E(cmcReplace);
    E(cmcSearchAgain);
    E(cmGotoLine);
    E(cmcJumpToFunction);
    E(cmcRecordMacro);
    E(cmcStopMacro);
    E(cmcPlayMacro);
    if (clipWindow->editor->hasSelection())
    {
      E(cmcPaste);
    }
    else
    {
      D(cmcPaste);
    }
    if (((TCEditWindow *) TProgram::deskTop->current)->editor->hasSelection())
    {
      E(cmcCut);
      E(cmcCopy);
      E(cmcClear);
    }
    else
    {
      D(cmcCut);
      D(cmcCopy);
      D(cmcClear);
    }
  }
  else
  {
    D(cmSyntaxHelp);
    D(cmcExpandAllTabs);
    D(cmcCompactBuffer);
    D(cmcFind);
    D(cmcReplace);
    D(cmcSearchAgain);
    D(cmGotoLine);
    D(cmcJumpToFunction);
    D(cmcRecordMacro);
    D(cmcStopMacro);
    D(cmcPlayMacro);
    D(cmcPaste);
    D(cmcCut);
    D(cmcCopy);
    D(cmcClear);
  }

  if (current_is_help)
  {
#define IW (((TInfWindow *)TProgram::deskTop->current)->viewer)
    if (IW->selRowEnd > IW->selRowStart ||
        (IW->selRowEnd == IW->selRowStart && IW->selColEnd > IW->selColStart))
#undef IW
    {
      E(cmcCopy);
    }
    else
    {
      D(cmcCopy);
    }
    E(cmcFind);
    E(cmcSearchAgain);
  }

  if (has_editors)
  {
    E(cmSaveAll);
  }
  else
  {
    D(cmSaveAll);
    D(cmcUndo);
    D(cmcRedo);
  }

  if (msg_window)
  {
    E(cmNextMessage);
    E(cmPrevMessage);
  }
  else
  {
    D(cmNextMessage);
    D(cmPrevMessage);
  }

  if (current_is_editor || (current_is_project && project_count))
  {
    E(cmCompile);
  }
  else
  {
    D(cmCompile);
  }

  if (dual_display)
  {
    D(cmUserScreen);
  }
  else
  {
    E(cmUserScreen);
  }
#ifdef INTERNAL_DEBUGGER
  if (debugger_started())
  {
    E(cmModifyRegister);
    E(cmGotoDisass);
  }
  else
  {
    D(cmModifyRegister);
    D(cmGotoDisass);
  }
#endif
  if (external_program_executed)
  {
    external_program_executed = 0;
    check_for_external_modifications();
  }
  should_update = 0;
}

#undef D
#undef E

int update_flag = 1;

void
IDE::idle()
{
  TApplication::idle();
#ifdef __DJGPP__
  __dpmi_yield();
#else
  if (inIdleTime > 10)
    usleep(1);
#endif
  if (update_flag > 0)
    update();
}

void
SetMainTargetName(const char *name, TProject * _prj)
{
  TProject *prj = _prj ? _prj : project;

  if ((!_prj) && (!prj->dest_name || strcmp(name, FName(prj->dest_name))))
    already_maked = 0;
  if (prj->dest_name)
    delete prj->dest_name;

  InitFName(prj->dest_name, name);
  prj->dest_file_type = get_file_type(name);
  if (prj->source_name)
  {
    prj->source_file_type = get_file_type(FName(prj->source_name));
    prj->compile_id = how_to_compile(prj->source_file_type,
                                     prj->dest_file_type);
    string_free(prj->main_function);
    switch (prj->compile_id)
    {
      default:
        prj->main_function = string_dup("main");
        break;
      case COMPILE_LINK_FPC_AUTOMAKE:
        prj->main_function = string_dup("PASCALMAIN");
        break;
      case COMPILE_LINK_PASCAL_AUTOMAKE:
        prj->main_function = string_dup("pascal_main_program");
        break;
    }
    return;
  }
  prj->source_file_type = FILE_UNKNOWN;
  prj->compile_id = how_to_compile(FILE_OBJECT, prj->dest_file_type);
}

void
TargetName()
{
  char buffer[PATH_MAX];

  buffer[0] = 0;
  if (Project.dest_name)
    strcpy(buffer, FName(Project.dest_name));
  if (HistinputBox(_("Name of the main target"), _("~N~ame"),
                   buffer, 255, RHIDE_History_main_targetname) == cmOK)
  {
    BackslashToSlash(buffer);
    SetMainTargetName(buffer);
  }
}

static void
GotoLine(TIDEFileEditor * editor)
{
  int line = editor->curPos.y + 1;
  char temp[10];

  if (editor->IslineInEdition)
    editor->MakeEfectiveLineInEdition();
  sprintf(temp, "%d", line);
  if (ValidInputBox(_("Goto the line"), _("line ~n~umber"),
                    temp, 10, new TRangeValidator(1, editor->limit.y == 0 ?
                                                  1 : editor->limit.y)) ==
      cmOK)
  {
    sscanf(temp, "%d", &line);
    goto_line(editor, line);
  }
}

void
Repaint()
{
  if (NoShadows)
    DisableShadows();
  else
    EnableShadows();
  TProgram::deskTop->redraw();
  TProgram::application->Redraw();
  message(TProgram::application, evBroadcast, cmRedraw, NULL);
  update_mem(1);
}

static void
About()
{
  TDialog *dialog;
  TStaticText *text;
  char buffer[1000];

  dialog = new TDialog(TRect(0, 0, 60, 19), _("About RHIDE"));
  dialog->options |= ofCentered;
  sprintf(buffer, "\003%s\n"
          "\003(%s)\n"
          "\003\n"
          "\003%s\n"
          "\003%s\n\003\n"
          "\003%s, %d-%d\n"
          "\003\n%s%s\n"
          "%s%s\n"
          "%s%s\n"
          "Editor  : %s\n"
#ifdef INTERNAL_DEBUGGER
          "Debugger: %s\n"
#endif
          ,
          IDEVersion,
          build_date,
          _("RHIDE is an Integrated Development Environment"),
#ifdef __DJGPP__
          _("for developing DJGPP apps"),
#elif __linux__
          _("for developing Linux apps"),
#else
          _("for developing apps"),
#endif
          _("Copyright (C) by Robert H”hne"),
          1996, 2002,
          _("Language: "), _("English"),
          _("Translated by: "), _("Nobody"),
          _("last updated: "), _("1998-11-29")
          ,TCEDITOR_VERSION_STR
#ifdef INTERNAL_DEBUGGER
          , gdb_version()
#endif
          );
  text = new TStaticText(TRect(0, 0, 58, 14), buffer);
  text->options |= ofCentered;
  dialog->insert(text);
  TRect r(25, dialog->size.y - 3, 35, dialog->size.y - 1);

  dialog->insert(new TButton(r, _("~O~K"), cmOK, bfDefault));
  TProgram::deskTop->execView(dialog);
  destroy(dialog);
}

static int help_request = 0;
static ushort help_ctx;

void
IDE::getEvent(TEvent & event)
{
  TApplication::getEvent(event);
  if (help_request && event.what == evNothing)
  {
    event.what = evCommand;
    event.message.command = cmHelp;
  }
  switch (event.what)
  {
    case evCommand:
      switch (event.message.command)
      {
        case cmEnter:
          event.what = evKeyDown;
          event.keyDown.keyCode = kbSpace;
          event.keyDown.charScan.charCode = ' ';
          break;
        case cmHelp:
          if (help_request)
          {
            help_request = 0;
            Help(help_ctx);
          }
          else
          {
            help_request = 1;
            help_ctx = getHelpCtx();
            event.message.command = 0xFFFF;
            break;
          }
          clearEvent(event);
          break;
        default:
          break;
      }
    default:
      break;
  }
  if (event.what != evNothing && event.what != evMouseMove)
    should_update = 1;
}

#define SC(command)\
   case cm##command:\
     command();\
     clearEvent(event);\
     break

#define SC2(command,function)\
   case cm##command:\
     function();\
     clearEvent(event);\
     break

#define SC3(command,function)\
   case cm##command:\
     function;\
     clearEvent(event);\
     break

/*
Return NULL or an allocated string.
*/

char *
WUC()
{
  char *word;
  TEvent event;

  if (!TProgram::deskTop->current)
    return NULL;
  event.what = evBroadcast;
  event.message.command = cmEditorAnswer;
  TProgram::deskTop->current->handleEvent(event);
  if (event.what != evNothing)
    return NULL;
#define E ((TCEditWindow *)TProgram::deskTop->current)->editor
  word = E->WordUnderCursor();
  if (!word)                    // try to get the word before the cursor
  {
    int x = E->curPos.x;

    if (x > 0)
    {
      E->MoveCursorTo(x - 1, E->curPos.y);
      word = E->WordUnderCursor();
      E->MoveCursorTo(x, E->curPos.y);
    }
  }
#undef E
  return word;
}

unsigned long hasmodified = 0;

int
SaveAll()
{
  int i;
  int failed = 0;
  TEvent event;

  if (hasmodified && windows)
  {
    int count = windows->getCount();
    TWindow *window;

    for (i = 0; i < count; i++)
    {
      window = DESKTOPWINDOW(i);
      event.what = evBroadcast;
      event.message.command = cmEditorAnswer;
      window->handleEvent(event);
      if (event.what == evNothing)
      {
        event.what = evCommand;
        event.message.command = cmSaveEditor;
        window->handleEvent(event);
        if (((TCEditWindow *) window)->editor->modified == True)
          // it was not saved
          failed++;
      }
    }
    if (!failed)
    {
      hasmodified = 0;
      // If the user calls this function direct, I should remember,
      // that the project should remake
      already_maked = 0;
    }
  }
  if (failed)
    return 0;
  else
    return 1;
}

static void
ShowEditor(char *name, int line, Boolean only_focus, char *msg, int column)
{
  TCEditWindow *ewindow;

  ewindow = is_on_desktop(name, False);
  if (!ewindow && only_focus == True)
    return;
  if (!ewindow)
  {
    ewindow = OpenEditor(name, True);
  }
  TProgram::deskTop->lock();
  TView *c = TProgram::deskTop->current;

  ewindow->select();
  if (only_focus == True)
    c->select();
  TProgram::deskTop->unlock();
  goto_line((TIDEFileEditor *) ewindow->editor, line, column);
  if (only_focus == False && msg)
  {
    ewindow->editor->setStatusLine(msg);
  }
}

#ifdef __DJGPP__
static char *
nl2crlf(char *t)
{
  int nl_count = 0;
  char *s = t;

  while ((s = strchr(s, '\n')) != NULL)
  {
    s++;
    nl_count++;
  }
  if (!nl_count)
    return t;
  s = (char *) malloc(strlen(t) + nl_count + 1);
  char *tt = t;
  char *ss = s;

  while (*tt)
  {
    if (*tt == '\n')
      *ss++ = '\r';
    *ss++ = *tt++;
  }
  *ss = 0;
  free(t);
  return s;
}
#endif

static void
BugReport(int full = 1)
{
  TCheckDialog *d = new TCheckDialog(TRect(10, 5, 40, 10), _("Please wait"));

  TProgram::deskTop->insert(d);
  d->update(_("Creating the Bug report"));
  TProgram::deskTop->lock();
  TCEditWindow *ew = App->openEditor(NULL, True);
  TCEditor *e = ew->editor;
  char *_text = create_bug_report(full);

#ifdef __DJGPP__
  _text = nl2crlf(_text);
#endif
  e->insertText(_text, strlen(_text), False);
  string_free(_text);
  e->trackCursor(True);
  TProgram::deskTop->unlock();
  destroy(d);
}

#ifdef INTERNAL_DEBUGGER
static void
MainFunction()
{
  char buffer[256];

  if (Project.main_function)
    strcpy(buffer, Project.main_function);
  else
    buffer[0] = 0;
  if (inputBox(_("Name of the main function"), _("~N~ame"),
               buffer, 255) == cmOK)
  {
    string_free(Project.main_function);
    string_dup(Project.main_function, buffer);
  }
}
#endif

#ifdef INTERNAL_DEBUGGER
static void
AddInspect(const char *expr)
{
  char buf[1000];

  buf[0] = 0;
  if (expr)
    strcpy(buf, expr);
  if (HistinputBox(_("Expression to inspect"), _("~E~xpression"),
                   buf, 999, RHIDE_History_Inspect) == cmOK)
  {
    TInspector *w = new TInspector(TProgram::deskTop->getExtent(), buf);

    w->update(buf);
    AddWindow(w, (TWindow **) & w);
  }
}
#endif

void
IDE::handleEvent(TEvent & event)
{
  static char *callstack_name = NULL;
  static char *WindowMsg = NULL;
  static Boolean Focus;

  switch (event.what)
  {
    case evMouseDown:
      switch (event.mouse.buttons)
      {
        case mbRightButton:
          event.mouse.buttons = mbLeftButton;
          handleEvent(event);
          if (TProgram::deskTop->current)
          {
            TEvent ev;

            ev.what = evBroadcast;
            ev.message.command = cmInfoAnswer;
            TProgram::deskTop->current->handleEvent(ev);
            if (ev.what == evNothing)
            {
              event.what = evKeyDown;
              event.keyDown.keyCode = kbCtrlS;
            }
            else
            {
              event.what = evBroadcast;
              event.message.command = cmEditorAnswer;
              TProgram::deskTop->current->handleEvent(event);
              if (event.what == evNothing)
              {
                event.what = evCommand;
                event.message.command = cmSyntaxHelp;
              }
              else
                clearEvent(event);
            }
          }
          else
            clearEvent(event);
          break;
        default:
          break;
      }
      break;
    case evBroadcast:
      switch (event.message.command)
      {
        case cmWindowOpened:
        {
          const char *name = (const char *) event.message.infoPtr;

          if (__file_exists(name))
            TimeOfFile(name, True);	// this removes the file from the hashtable
          clearEvent(event);
          break;
        }
        case cmFocusWindow:
          Focus = True;
        case cmOpenWindow:
          string_dup(callstack_name, (char *) event.message.infoPtr);
          clearEvent(event);
          break;
        case cmShowWindowMsg:
          string_dup(WindowMsg, (char *) event.message.infoPtr);
          clearEvent(event);
          break;
        case cmGotoWindowLineColumn:
        {
          MsgRec *rec;

          if (!callstack_name)
            break;
          rec = (MsgRec *) event.message.infoPtr;
          ShowEditor(callstack_name, rec->lineno, Focus, WindowMsg,
                     rec->column);
          string_free(callstack_name);
          string_free(WindowMsg);
          Focus = False;
          clearEvent(event);
          break;
        }
        case cmGotoWindowLine:
          if (!callstack_name)
            break;
          ShowEditor(callstack_name, event.message.infoLong, Focus, WindowMsg,
                     1);
          string_free(callstack_name);
          string_free(WindowMsg);
          Focus = False;
          clearEvent(event);
          break;
        case cmEditorFilenameChanged:
        {
          TIDEFileEditor *e = (TIDEFileEditor *) event.message.infoPtr;

          UpdateWindow((TWindow *) e->owner);	// update the windowlist
          TimeOfFile(e->fileName, True);
          clearEvent(event);
          break;
        }
        case cmEditorModified:
          hasmodified = (unsigned long) event.message.infoLong;
          clearEvent(event);
          break;
        default:
          break;
      }
      break;
    case evCommand:
      switch (event.message.command)
      {
        case cmEnter:
          event.what = evKeyDown;
          event.keyDown.keyCode = kbSpace;
          event.keyDown.charScan.charCode = ' ';
          break;
        default:
          break;
      }
      break;
    default:
      break;
  }
  TApplication::handleEvent(event);
  switch (event.what)
  {
    case evCommand:
      switch (event.message.command)
      {
        case cmQUIT:
          if (DEBUGGER_STARTED())
            RESET();
          if (CloseProject())
            endModal(cmQuit);
          clearEvent(event);
          break;
        case cmPrintSetup:
          PrintSetup();
          clearEvent(event);
          break;
        case cmPrint:
          IDEPrintEditor(((TCEditWindow *) TProgram::deskTop->current)->
                         editor);
          clearEvent(event);
          break;
        case cmJumpToFunction:
        {
          TView *ew = TProgram::deskTop->current;

          if (ew != NULL)
          {
            TIDEFileEditor *editor =

              (TIDEFileEditor *) ((TCEditWindow *) ew)->editor;
            if (editor)
            {
              event.message.command = cmcJumpToFunction;
              editor->handleEvent(event);
            }
          }
          clearEvent(event);
          break;
        }
        case cmSetGlobalOptions:
          TCEditor::SetGlobalOptions();
          clearEvent(event);
          break;
        case cmAscii:
          if (!AsciiWindow)
          {
            Ascii();
            AddWindow(AsciiWindow, &AsciiWindow);
          }
          AsciiWindow->select();
          clearEvent(event);
          break;
        case cmPuzzle:
          if (!PuzzleWindow)
          {
            Puzzle();
            AddWindow(PuzzleWindow, &PuzzleWindow);
          }
          PuzzleWindow->select();
          clearEvent(event);
          break;
        case cmCalendar:
          if (!CalendarWindow)
          {
            Calendar();
            AddWindow(CalendarWindow, &CalendarWindow);
          }
          CalendarWindow->select();
          clearEvent(event);
          break;
          SC(LastHelp);
          SC(SyntaxIndex);
          SC(SyntaxOptions);
          SC(BugReport);
        case cmBugReportSmall:
          BugReport(0);
          clearEvent(event);
          break;
          SC(MouseDlg);
          SC(SaveOptions);
          SC(LoadOptions);
          SC(FSDB);
          SC(GDB);
          SC(EditReserved);
          SC(EditGPCReserved);
          SC(About);
          SC(EditCFlags);
          SC(EditCXXFlags);
          SC(EditPascalFlags);
          SC(EditFortranFlags);
          SC(EditAdaFlags);
          SC(EditOptFlags);
          SC(EditWarnFlags);
          SC(EditDebugFlags);
          SC(Preferences);
          SC(GREP);
          SC(Repaint);
          SC(Colors);
          SC(WriteMake);
          SC(Libraries);
          SC(WarningFlags);
          SC(OptimizationFlags);
          SC(DebugFlags);
          SC(CFlags);
          SC(CXXFlags);
          SC(PascalFlags);
          SC(FortranFlags);
          SC(AdaFlags);
          SC(AddProjectItem);
          SC(DelProjectItem);
          SC(ShowProject);
          SC(TargetName);
          SC(ShowIncludes);
          SC(LocalOptions);
          SC(LinkerOptions);
          SC(CompilerOptions);
          SC(Make);
          SC(Build);
          SC(EditUserWords);
          SC(ClearDependencies);
          SC(CreateDependencies);
          SC(MakeClear);
          SC(HelpIndex);
          SC(SaveAll);
          SC2(OpenEditor, fileOpen);
          SC2(NewEditor, fileNew);
          SC(DosShell);
          SC(ShowClip);
          SC(Tile);
          SC(Cascade);
          SC(ShowCalculator);
          SC(ClearMessages);
          SC(ClearMessage);
        case cmShowMessages:
          ShowMessages(NULL, False, False, True);
          clearEvent(event);
          break;
#ifdef INTERNAL_DEBUGGER
          SC(AddDataWindow);
          SC(ShowStackWindow);
          SC2(ShowWatchWindow, OpenWatchWindow);
        case cmDisWindow:
          OpenDisWin(1);
          clearEvent(event);
          break;
        case cmCallStack:
          ShowCallStackWindow();
          clearEvent(event);
          break;
          SC2(Trace, TRACE);
          SC2(Step, STEP);
          SC2(Goto, GOTO);
          SC3(TraceNo, TRACE(0));
          SC3(StepNo, STEP(0));
          SC3(GotoNo, GOTO(0));
          SC2(Finish, FINISH);
          SC2(Reset, RESET);
          SC(MainFunction);
        case cmBreakPoints:
          BreakDialog();
          Repaint();
          break;
          SC(ToggleBreak);
        case cmEvaluate:
          Evaluate(WUC());
          clearEvent(event);
          break;
        case cmInspect:
          AddInspect(WUC());
          clearEvent(event);
          break;
        case cmAddWatchEntry:
          if ((TProgram::deskTop->current) &&
              ((get_file_type
                (((TIDEFileEditor
                   *) ((TCEditWindow *) (TProgram::deskTop->current))->
                  editor)->fileName) == FILE_PASCAL_SOURCE)
               ||
               (get_file_type
                (((TIDEFileEditor
                   *) ((TCEditWindow *) (TProgram::deskTop->current))->
                  editor)->fileName) == FILE_PASCAL_SOURCE)))
          {
            char *tmp = string_dup(WUC());

            string_up(tmp);
            AddWatchEntry(tmp);
            string_free(tmp);
          }
          else
            AddWatchEntry(WUC());
          clearEvent(event);
          break;
        case cmFunctionList:
        {
          char regex[256];
          function_entry *func;

          regex[0] = 0;
          if (HistinputBox(_("Regular expression to list functions"),
                           _("~E~xpression"), regex, 255,
                           tvgdb_History_Break_Function) == cmOK)
          {
            if (SelectFunction
                (_("List of functions"), regex, NULL, NULL, &func) == cmOK)
            {
              TCEditWindow *ew = is_on_desktop(func->file_name);

              if (!ew)
              {
                char *full_name;

                if (FindFile(func->file_name, full_name) == False)
                {
                  messageBox(mfError | mfOKButton,
                             _("Could not find the source file %s."),
                             func->file_name);
                  string_free(full_name);
                  full_name = (char *) malloc(PATH_MAX);
                  TFileDialog *dialog;

                  InitHistoryID(RHIDE_History_source_file);
                  dialog = new TFileDialog(func->file_name, _("Open a file"),
                                           _("~N~ame"), fdOpenButton,
                                           RHIDE_History_source_file);
                  if (TProgram::deskTop->execView(dialog) != cmCancel)
                  {
                    dialog->getData(full_name);
                    FExpand(full_name);
                  }
                  else
                    full_name = NULL;
                }
                if (full_name)
                  ew = OpenEditor(full_name, False);
              }
              if (ew)
              {
                goto_line((TIDEFileEditor *) ew->editor, func->line_number);
                ew->select();
              }
            }
          }
          clearEvent(event);
          break;
        }
#endif
        case cmShowWindowList:
          ShowWindowList(_("WindowList"));
          clearEvent(event);
          break;
        case cmSelectProject:
        {
          TDependency *dep = (TDependency *) event.message.infoPtr;
          char *pname = string_dup(FName(dep->source_name));

          AddToStack();
          if (!CloseProject())
            RemoveFromStack();
          else
            OpenProject(pname);
          string_free(pname);
          clearEvent(event);
          break;
        }
        case cmSyntaxHelp:
        {
          char *word = WUC();

          if (word)
            SyntaxHelp(word, NULL);
          else
            SyntaxHelp("", NULL);
          if (word)
            string_free(word);
          clearEvent(event);
          break;
        }
        case cmGotoLine:
        {
          TView *ew = TProgram::deskTop->current;

          event.what = evBroadcast;
          event.message.command = cmEditorAnswer;
          if (ew != NULL)
          {
            ew->handleEvent(event);
            if (event.what == evNothing)
              GotoLine((TIDEFileEditor *) ((TCEditWindow *) ew)->editor);
          }
          clearEvent(event);
          break;
        }
        case cmNextMessage:
          event.message.command = cmNextMsg;
          if (msg_window)
            msg_window->handleEvent(event);
          break;
        case cmPrevMessage:
          event.message.command = cmPrevMsg;
          if (msg_window)
            msg_window->handleEvent(event);
          break;
        case cmRun:
          if (DEBUGGER_STARTED())
          {
            CONTINUE();
            clearEvent(event);
            break;
          }
          else
          {
            TView *lasteditor = NULL;
            TView *v = TProgram::deskTop->current;

            if (v)
            {
              event.what = evBroadcast;
              event.message.command = cmEditorAnswer;
              v->handleEvent(event);
              if (event.what == evNothing)
              {
                if (v->state & sfSelected)
                {
                  lasteditor = TProgram::deskTop->current;
                }
              }
            }
            ShowMessages(NULL, True);
            if (Make(False) == True)
            {
              RunMainTarget();
              if (!DEBUGGER_STARTED() && lasteditor != NULL &&
                  !ShowStderr && !ShowStdout)
                lasteditor->select();
            }
            clearEvent(event);
            break;
          }
        case cmCompile:
          if (DEBUGGER_STARTED())
            RESET();
          ShowMessages(NULL, True);
          Compile();
          clearEvent(event);
          break;
        case cmLink:
          if (DEBUGGER_STARTED())
            RESET();
          ShowMessages(NULL, True);
          Compile(project);
          clearEvent(event);
          break;
        case cmStandardIncludeDir:
          EditDirList(Options.StdInc, _("Standard Include Directories"),
                      RHIDE_History_standard_include_directories);
          clearEvent(event);
          break;
        case cmIncludeDir:
          EditDirList(Options.include_path, _("Include Directories"),
                      RHIDE_History_include_directories);
          clearEvent(event);
          break;
        case cmLibDir:
          EditDirList(Options.library_path, _("Library Directories"),
                      RHIDE_History_library_directories);
          clearEvent(event);
          break;
        case cmObjDir:
          EditDirList(Options.ObjDirs, _("Object Directories"),
                      RHIDE_History_object_directories);
          clearEvent(event);
          break;
        case cmSrcDir:
          EditDirList(Options.SrcDirs, _("Source Directories"),
                      RHIDE_History_source_directories);
          clearEvent(event);
          break;
        case cmSyntaxFiles:
          EditParamList(Project.info_files, _("INFO files for syntaxhelp"),
                        RHIDE_History_info_files);
          clearEvent(event);
          break;
        case cmProgArgs:
          EditParamList(Options.ProgArgs, _("Program arguments"),
                        RHIDE_History_arguments);
          clearEvent(event);
          break;
        case cmOpenProject:
        {
          char *fileName = select_project(_("Select a project"));

          if (fileName)
          {
            if (CloseProject())
              OpenProject(fileName);
            string_free(fileName);
          }
          clearEvent(event);
          break;
        }
        case cmCloseProject:
          if (CloseProject())
          {
            if (!OpenFromStack())
              OpenProject(NULL);
          }
          clearEvent(event);
          break;
        case cmUserScreen:
        {
          int old_flag = update_flag;

          TMouse::suspend();
          update_flag = 0;
#ifdef __linux__
          RestoreScreen();
#else
          TScreen::suspend();
#endif
          do
          {
            idle();
#ifdef __linux__
            timeout(1);
#endif
            clearEvent(event);
            event.getKeyEvent();
#ifdef __DJGPP__
            __dpmi_yield();
#else
#ifdef __linux__
            timeout(0);
#endif
#endif
          }
          while (event.what == evNothing);
          clearEvent(event);
#ifndef __linux__
          TScreen::resume();
#endif
          update_flag = old_flag;
          TMouse::resume();
          Repaint();
          break;
        }
        case cmLibcHelp:
          SyntaxHelp("Top", "libc");
          clearEvent(event);
          break;
        case cmHelpHelp:
          SyntaxHelp("Top", "infview");
          clearEvent(event);
          break;
        case cmPrimaryFile:
        {
          char buffer[256];

          if (Project.source_name)
            strcpy(buffer, FName(Project.source_name));
          else
            buffer[0] = 0;
          if (inputBox(_("Name of the main target"), _("~N~ame"),
                       buffer, 255) == cmOK)
          {
            if ((strlen(buffer) > 0) &&
                (get_file_type(buffer) != FILE_PASCAL_SOURCE) &&
                (get_file_type(buffer) != FILE_FPC_SOURCE) &&
                (get_file_type(buffer) != FILE_TEX_SOURCE))
            {
              messageBox(mfError | mfOKButton, "%s %s",
                         _("You can give here only a valid "
                           "filename for a Pascal source file"),
                         _("or a TeX source file"));
            }
            else
            {
              if (!Project.source_name && strlen(buffer) > 0 ||
                  Project.source_name
                  && strcmp(FName(Project.source_name), buffer))
                already_maked = 0;
              if (Project.source_name)
              {
                delete Project.source_name;

                Project.source_name = NULL;
              }
              if (strlen(buffer) > 0)
              {
                BackslashToSlash(buffer);
                InitFName(Project.source_name, buffer);
              }
              SetMainTargetName(FName(Project.dest_name));
            }
          }
          clearEvent(event);
          break;
        case cmEditKeyBind:
          if (KeyBindEdit())
            SaveKeyBind(ExpandFileNameToThePointWhereTheProgramWasLoaded
                        (KeyBindFName));
          clearEvent(event);
          break;
        case cmSetUpAltKeys:
          if (AltKeysSetUp())
            SaveKeyBind(ExpandFileNameToThePointWhereTheProgramWasLoaded
                        (KeyBindFName));
          clearEvent(event);
          break;
        case cmKbBackDefault:
          if (KeyBackToDefault())
            SaveKeyBind(ExpandFileNameToThePointWhereTheProgramWasLoaded
                        (KeyBindFName));
          clearEvent(event);
          break;

        }
        default:
          break;
      }
      break;
  }
}

IDE *App;

static char *PRJNAME = NULL;
static char *EDITNAME = NULL;

static void usage() __attribute__ ((noreturn));
static void
usage()
{
  TEventQueue::suspend();
  TScreen::suspend();
  fprintf(stderr, _("usage: %s [options] [project-file] [edit-file]\n"),
          RHIDE_NAME);
  fprintf(stderr, _("options:    -d[actdf]\n"));
#ifdef __DJGPP__
  fprintf(stderr,
          _("            -c : show filename exactly (no case conversion)\n"));
  fprintf(stderr,
          _("            -y : force to use long filenames (Windows 95)\n"));
  fprintf(stderr,
          _("            -n : do not use long filenames (Windows 95)\n"));
  fprintf(stderr,
          _("            -b : use BIOS calls for toggle the blinkstate\n"));
  fprintf(stderr, _("            -p : do not convert the numpad keys\n"));
  fprintf(stderr,
          _("            -G num : Use methode <num> for screen swapping\n"));
  fprintf(stderr, _("            -K : Use the BIOS for keyboard input\n"));
  fprintf(stderr, _("            -M : Do not install the mouse handler\n"));
  fprintf(stderr, _("            -S : use slower (safer) screen output\n"));
#endif
#ifdef __linux__
  fprintf(stderr, _("            -K : Do not patch the keytable\n"));
  fprintf(stderr,
          _("            -H : Do not install console switch handler\n"));
#endif
  fprintf(stderr, _("            -L LANGUAGE : use language LANGUAGE\n"));
  fprintf(stderr, _("            -h : show this help\n"));
  fprintf(stderr,
          _("            -k KEY_FILE : use KEY_FILE for key bindings\n"));
  fprintf(stderr, _("            -E : Output all env. variables and exit\n"));
  fprintf(stderr, _("            -C : Disable SIGINT handling\n"));
#ifdef __DJGPP__
  fprintf(stderr, _("            -T : Disable the DOS-box title feature\n"));
#endif
  fprintf(stderr,
          _("            -P : don`t remove temp files when exiting\n"));
  fflush(stderr);
  exit(-1);
}

static char *keybindings = NULL;

static char *
next_option(char *&option, char *option_end, int &current_argc,
            int argc, char *argv[])
{
  static char arg[256];

  if (option && *option && option <= option_end)
  {
    char *tmp = arg;

    while (option <= option_end && *option != ' ')
    {
      *tmp++ = *option++;
    }
    *tmp = 0;
    option++;
    return arg;
  }
  if (current_argc >= argc)
    return NULL;
  current_argc++;
  return argv[current_argc - 1];
}

static int call_usage = 0;
static int dump_env = 0;
static int no_sigint = 0;

#ifdef __DJGPP__
static int no_title = 0;
#endif

#define Usage() do { call_usage = 1; return; } while (0)

static void
parse_commandline(int argc, char *argv[])
{
  /*
     At first look for an environment variable $(RHIDEOPT) and
     use the options from that variable as they were put at the
     beginning of the commandline, so they can be overwritten 
   */
  char *_rhide_opt = expand_rhide_spec("$(RHIDEOPT)");
  char *rhide_opt = NULL;

  TGKey::useBIOS = 0;

  if (*_rhide_opt)
  {
    rhide_opt = (char *) alloca(strlen(_rhide_opt));
    strcpy(rhide_opt, _rhide_opt);
  }
  string_free(_rhide_opt);
  char *rhide_opt_end = NULL;
  int i = 1;
  char *tmp, *a = "a";
  char *arg;

  if (rhide_opt && *rhide_opt)
  {
    // strip leading whitespaces
    while (*rhide_opt == ' ')
      rhide_opt++;
    if (*rhide_opt)
    {
      // skip trailing whitespaces
      rhide_opt_end = rhide_opt + strlen(rhide_opt) - 1;
      while (*rhide_opt_end == ' ')
        rhide_opt_end--;
    }
  }
  while ((arg = next_option(rhide_opt, rhide_opt_end, i, argc, argv)) != NULL)
  {
    if (arg[0] == '-')
    {
      switch (arg[1])
      {
        case 'P':
          keep_temp_dir = 1;
          break;
        case 'T':
#ifdef __DJGPP__
          no_title = 1;
#endif
          break;
        case 'C':
          no_sigint = 1;
          break;
        case 'H':
#ifdef __linux__
          extern int install_console_sigs;

          install_console_sigs = 0;
#endif
          break;
        case 'S':
#ifdef __DJGPP__
          extern int slow_screen;
          extern char useBIOS_VGA_State;
          useBIOS_VGA_State = 0;
          slow_screen = 1;
#endif
          break;
        case 'E':
          dump_env = 1;
          break;
        case 'M':
#ifdef __DJGPP__
          extern int use_mouse_handler;

          use_mouse_handler = 0;
#endif
          break;
        case 'K':
          TGKey::useBIOS = 1;
          break;
        case 'G':
          arg = next_option(rhide_opt, rhide_opt_end, i, argc, argv);
#ifdef __DJGPP__
          extern int screen_saving;

          if (!arg)
            Usage();
          screen_saving = atoi(arg);
#endif
          break;
        case 'p':
          TGKey::translateKeyPad = 0;
          break;
        case 'k':
          arg = next_option(rhide_opt, rhide_opt_end, i, argc, argv);
          if (!arg)
            Usage();
          keybindings = ExpandFileNameToThePointWhereTheProgramWasLoaded(arg);
          break;
        case 'b':
        {
#ifdef __DJGPP__
          extern int blink_use_bios;

          blink_use_bios = 1;
#endif
          break;
        }
        case 'c':
#ifdef __DJGPP__
          _crt0_startup_flags |= _CRT0_FLAG_PRESERVE_FILENAME_CASE;
#endif
          break;
        case 'y':
#ifdef __DJGPP__
          putenv("LFN=y");
#endif
          break;
        case 'n':
#ifdef __DJGPP__
          putenv("LFN=n");
          _crt0_startup_flags |= _CRT0_FLAG_NO_LFN;
#endif
          break;
        case 'L':
        {
          arg = next_option(rhide_opt, rhide_opt_end, i, argc, argv);
          if (!arg)
            Usage();
          /**
            I don't know, if it is a bug in the setlocale() function
            or in my mind, but I had massive problems, setting the
            language if there is no external env. variable set and
            if I use a short language name like `de`.

            I have now analyzed the following.
            I have the rhide.mo file in "/usr/share/locale/de/LC_MESSAGES/".
            In "/usr/lib/locale/" I have beside others the directory
            "de_DE/" but NOT "de/".

            If I set now the language to "de", it will fail, but
            if I set it to "de_DE" it will succeed and also will it
            work, if I create in "/usr/lib/locale/" a symbolic link
            from "de_DE/" to "de/".

            I don't know, why it works so, but it does so. That's
            why I use the trick below, when the first call to
            setlocale failed.
          */
            
          if (setlocale(LC_ALL, arg) == NULL)
          {
            char *new_arg = string_dup(arg);
            string_up(new_arg);
            char *temp_arg = string_dup(arg);
            string_cat(temp_arg, "_", new_arg, NULL);
            string_free(new_arg);
            char *res = setlocale(LC_ALL, temp_arg);
            string_free(temp_arg);
            if (res == NULL)
            {
              fprintf(stderr, "Could not switch to language `%s`.\n",
                      arg);
            }
          }
          break;
        }
        case 'h':
          Usage();
        case 'd':
        {
          int enable = 1;

          if (!arg[2])
            tmp = a;
          else
            tmp = &arg[2];
          while (*tmp)
          {
            switch (*tmp)
            {
              case '-':
                enable = 0;
                break;
              case '+':
                enable = 1;
                break;
              case 's':
                debug_specs = enable;
                break;
              case 'f':
                debug_files = enable;
                break;
              case 'd':
                debug_dependencies = enable;
                break;
              case 'c':
                debug_commands = enable;
                break;
              case 't':
                debug_tempfiles = enable;
                break;
              case 'a':
                debug_dependencies = enable;
                debug_commands = enable;
                debug_tempfiles = enable;
                debug_files = enable;
                debug_specs = enable;
                break;
              default:
                Usage();
            }
            tmp++;
          }
          break;
        }
        default:
          Usage();
      }
    }
    else
    {
      char *dir, *file, *ext;

      string_dup(PRJNAME, arg);
      split_fname(PRJNAME, dir, file, ext);
      string_free(PRJNAME);
      if (!ext || !*ext || strcasecmp(ext, PROJECT_EXT) == 0)
      {
        string_dup(PRJNAME, dir);
        string_cat(PRJNAME, file);
        string_cat(PRJNAME, PROJECT_EXT);
      }
      else
      {
        string_dup(EDITNAME, arg);
        FExpand(EDITNAME);
      }
      string_free(dir);
      string_free(file);
      string_free(ext);
    }
  }
#ifdef __DJGPP__
/* Select some switches on NT, because it is buggy and cannot
   handle correct some standard tricks */
  extern int nt_detected;

  if (nt_detected)
  {
    extern int use_mouse_handler;

    use_mouse_handler = 0;
    extern int slow_screen;

    slow_screen = 1;
  }
#endif
}

static void
find_project()
{
  char *spec = NULL;
  string_cat(spec, "$(strip $(subst ", RHIDE_OPTIONS_NAME, PROJECT_EXT,
             ",,$(wildcard *", PROJECT_EXT, ")))", NULL);
  char *files = expand_rhide_spec(spec);
  string_free(spec);
  if (!*files) // no .gpr file was found
  {
    string_free(files);
    return;
  }
  string_cat(spec, "$(word 2,", files, ")", NULL);
  char *file = expand_rhide_spec(spec);
  string_free(spec);
  if (!*file) // only one .gpr file was found
  {
    PRJNAME = files;
  }
  else
  {
    string_free(files);
    PRJNAME = select_project(_("Select a project"));
  }
  string_free(file);
}

static char *tmpdir = NULL;
static void remove_tmpdir();

static void
set_tmpdir()
{
  int mktmpdir_ret = -1;
  char temp_mask[256] = "RHXXXXXX";

#ifndef __DJGPP__
  char buf[256];
  time_t curtime;

  time(&curtime);
  strftime(buf, 256, "%Y%m%d%H%M%S", localtime(&curtime));
  sprintf(temp_mask, "RHIDE.%s.%d", buf, getpid());
#endif
  tmpdir = expand_rhide_spec("$(TMPDIR)");
  if (!*tmpdir)
  {
    string_free(tmpdir);
    tmpdir = expand_rhide_spec("$(TEMP)");
  }
  if (!*tmpdir)
  {
    string_free(tmpdir);
    tmpdir = expand_rhide_spec("$(TMP)");
  }
  if (!*tmpdir)
  {
    string_free(tmpdir);
#ifdef __DJGPP__
    tmpdir = NULL;
    tmpdir = getcwd(NULL, PATH_MAX);
#else
    string_dup(tmpdir, "/tmp");
#endif
  }
  BackslashToSlash(tmpdir);
  if (tmpdir[strlen(tmpdir) - 1] != '/'
#ifdef __DJGPP__
      && tmpdir[strlen(tmpdir) - 1] != '\\'
#endif
    )
    string_cat(tmpdir, "/");
  string_cat(tmpdir, temp_mask);
#ifdef __DJGPP__
  mktemp(tmpdir);
#endif
  if ((mktmpdir_ret = mkdir(tmpdir, 0755)) != 0)
  {
    string_free(tmpdir);
    tmpdir = getcwd(NULL, PATH_MAX);
    string_cat(tmpdir, "/");
    string_cat(tmpdir, temp_mask);
    int handle = mkstemp(tmpdir);
    if (handle != -1)
    {
      close(handle);
      unlink(tmpdir);
    }
    string_cat(tmpdir, ".tmp");
    mktmpdir_ret = mkdir(tmpdir, 0755);
  }
  char *tempdir = string_dup("TMPDIR=");

  string_cat(tempdir, tmpdir);
  putenv(tempdir);
  if (debug_tempfiles)
  {
    fprintf(stderr, _("using %s as temp directory\n"), tmpdir);
  }
  /*
     Only if temp directory successfully created before  
   */
  if (mktmpdir_ret == 0)
    atexit(remove_tmpdir);
}

static void
remove_tmpdir()
{
  if (!debug_tempfiles && tmpdir && !keep_temp_dir)
  {
    /*
       Removing temporary directory with force. Perhaps it's better
       not to do that as use program may leave temporary files there
       and user may want to inspect them later  
     */
    DIR *dir = opendir(tmpdir);

    if (dir)
    {
      dirent *entry;

      while ((entry = readdir(dir)) != 0)
      {
        if (strcmp(entry->d_name, ".") == 0
            || strcmp(entry->d_name, "..") == 0) continue;
        char fName[FILENAME_MAX];

        strcpy(fName, tmpdir);
        strcat(fName, "/");
        strcat(fName, entry->d_name);
        fprintf(stderr, _("warning: temporary file %s deleted\n"), fName);
        unlink(fName);
      }
      closedir(dir);
    }
    rmdir(tmpdir);
  }
}

static void
setup_argv0()
{
  char *tmp;

  string_dup(tmp, global_argv[0]);
  if (!__file_exists(tmp))
  {
    // it was found on PATH
    char *spec = NULL;

    string_cat(spec, "\
$(word 1,$(foreach path,$(subst $(RHIDE_PATH_SEPARATOR), ,$(PATH)),\
$(wildcard $(path)/$(notdir ", tmp, "))))", NULL);
    char *path = expand_rhide_spec(spec);

    string_free(spec);
    if (*path)
    {
      string_free(tmp);
      tmp = path;
    }
    else
      string_free(path);
  }
  FExpand(tmp);
  split_fname(tmp, RHIDE_DIR, RHIDE_NAME, RHIDE_EXT);
}

static char startup_directory[256];

int DebuggerFormatLine(TCEditor * editor,
                       void *DrawBuf,
                       unsigned LinePtr,
                       int Width,
                       unsigned short Colors,
                       unsigned lineLen, uint32 Attr, unsigned LineNo
#if (TCEDITOR_VERSION >= 0x000452)
                       , uint32 *
#endif
                       );

#ifdef __DJGPP__
extern char **environ;
#endif

static void dump_environment() __attribute__ ((noreturn));
static void
dump_environment()
{
  TMouse::suspend();
  TScreen::suspend();
  char **env = environ;

  dump_rhide_environment(stderr);
  while (*env)
  {
    fprintf(stderr, "%s\n", *env);
    env++;
  }
  exit(0);
}

/* Init the history saving for the editor related history ID`s */
static void
__InitHistoryIDs()
{
  int i;

  for (i = hID_Start; i < hID_Start + hID_Cant; i++)
    InitHistoryID(i);
  InitHistoryID(100);           // Read/Write block
}



static TInputLine *
rhideCreateInputLine(const TRect & rect, int aMaxLen)
{
  return new TInputLinePiped(rect, aMaxLen);
}


static void
init_rhide(int _argc, char **_argv)
#define __crt0_argc _argc
#define __crt0_argv _argv
{
#ifdef __DJGPP__
  __system_flags |= __system_allow_multiple_cmds;
  __crt0_load_environment_file("rhide");	// when the exe has an other name
  // than rhide force to load the
  // variables for RHIDE
  __crt0_load_environment_file("info");	// To get the INFO-path for bug-report
#endif

  global_argv = __crt0_argv;
  global_argc = __crt0_argc;
  setup_argv0();

  CreateInputLine = &rhideCreateInputLine;
  parse_commandline(__crt0_argc, __crt0_argv);

  char *spec =
    string_dup("INFOPATH=$(subst $(RHIDE_SPACE),$(RHIDE_PATH_SEPARATOR),\
$(strip $(RHIDE_CONFIG_DIRS) $(INFOPATH) /usr/share/info /usr/info \
/usr/local/share/info /usr/local/info))");
  char *tmp = expand_rhide_spec(spec);

  string_free(spec);
  putenv(tmp);

  push_environment();
  set_tmpdir();
  char *locale_dir = expand_rhide_spec("$(LOCALEDIR)");

#ifndef __DJGPP__
  if (!*locale_dir)
  {
    string_free(locale_dir);
    // get the system default localedir
    char *_locale_dir = BINDTEXTDOMAIN("rhide", NULL);
    if (_locale_dir)
      locale_dir = string_dup(_locale_dir);
    else
      locale_dir = string_dup("");

    char *spec = NULL;

    string_cat(spec, "$(subst /de/LC_MESSAGES/rhide.mo,,",
               "$(strip $(word 1,$(foreach file,$(addsuffix ",
               "/de/LC_MESSAGES/rhide.mo",
               ",",
               locale_dir, " ",
               "/usr/local/share/locale ",
               "/usr/share/locale",
               "),$(wildcard $(file))))))", NULL);
    char *file = expand_rhide_spec(spec);
    string_free(spec);
    if (*file)
    {
      string_free(locale_dir);
      locale_dir = file;
    }
    else
      string_free(file);
  }
#endif
  if (!*locale_dir)
  {
    string_free(locale_dir);
    locale_dir = expand_rhide_spec("$(DJDIR)");
    if (*locale_dir)
    {
      string_cat(locale_dir, "/share/locale");
    }
    else
    {
      string_free(locale_dir);
      locale_dir = string_dup("/usr/local/share/locale");
    }
  }
  setlocale(LC_ALL, "");
  BINDTEXTDOMAIN("rhide", locale_dir);
  string_free(locale_dir);
  TEXTDOMAIN("rhide");
#if 0
  extern int convert_num_pad;

  convert_num_pad = 1;
#endif
  TScreen::suspend();
  fprintf(stderr, _("This is %s. Copyright (c) 1996-2002 by Robert H”hne\n"),
          IDEVersion);
  fprintf(stderr, "             (%s %s)\n", build_date, build_time);
  TScreen::resume();
  PrintSetDefaults();
  InitHistoryIDs = __InitHistoryIDs;
  expand_filenames = ExpandFileNames;
  TFileCollection::sortOptions = fcolAlphabetical | fcolCaseInsensitive;
}

/*
   This string should not translated, because it is used, when there is
   something wrong.
*/

const char msg[] =
  "RHIDE internal error. Please send a description of this situation\r\n"
  "as most as possible detailed to the author together with the version\r\n"
  "you are using."
#ifdef __DJGPP__
  " AND VERY IMPORTANT IS THE NEXT TRACEBACK!!!!"
#endif
  "\r\n\r\n";

static jmp_buf abort_jmp;

#ifdef __DJGPP__
static void free_title_seg();
#endif

extern "C" __attribute__ ((__noreturn__))
     void
     abort()
{
  char *bug;

  // call at least TEventQueue::resume() to clear the mouse hook
  TEventQueue::suspend();
  TScreen::suspend();
  bug = create_bug_report(0);
  write(STDERR_FILENO, bug, strlen(bug));
  write(STDERR_FILENO, msg, sizeof(msg) - 1);
  setjmp(abort_jmp);
#ifdef __DJGPP__
  __djgpp_exception_state_ptr = &abort_jmp;
  free_title_seg();
#endif
  raise(SIGABRT);
  _exit(1);
}

extern int LoadKeysForTCEditor(char *file);

static void
LoadKeys()
{
  LoadKeysForTCEditor(ExpandFileNameToThePointWhereTheProgramWasLoaded
                      (KeyBindFName));
  SLPInterfaceInit(ExpandFileNameToThePointWhereTheProgramWasLoaded
                   ("macros.slp"));

  char *syntax_file;

  TCEditor::SHLGenList = new TNoCaseStringCollection(5, 5);
  syntax_file = ExpandFileNameToThePointWhereTheProgramWasLoaded(SHLFile);
  if (LoadSyntaxHighLightFile(syntax_file, TCEditor::SHLArray,
                              TCEditor::SHLGenList, TCEditor::SHLCant) != 0)
  {
    /*
       should be in the 8.3 DOS scheme 
     */
    syntax_file = "__syntax";
    FILE *f = fopen(syntax_file, "w+t");

    fprintf(f, "\n\
Name=C/C++\n\
Files=C,c,cpp,cxx,cc,h,hpp,i,ii\n\
UseInternal=1\n\
End\n\
Name=Pascal\n\
Files=pas,inc,p,pp\n\
UseInternal=2\n\
End\n\
");
    fclose(f);
    LoadSyntaxHighLightFile(syntax_file, TCEditor::SHLArray,
                            TCEditor::SHLGenList, TCEditor::SHLCant);
    unlink(syntax_file);
  }

}

static void
rhide_sig(int signo)
{
  static int intr_in_progress = 0;

  switch (signo)
  {
    case SIGINT:
      if (intr_in_progress)
        return;
      intr_in_progress = 1;
      if (messageBox(_("Do you really want to interrupt RHIDE?"),
                     mfConfirmation | mfYesButton | mfNoButton) == cmYes)
      {
        destroy(App);
        chdir(startup_directory);
        exit(-1);
      }
      intr_in_progress = 0;
#ifdef __DJGPP__
      if (kbhit())
        getkey();
#endif
      signal(SIGINT, rhide_sig);
      break;
#ifndef __DJGPP__
    case SIGWINCH:
      signal(SIGWINCH, rhide_sig);
      TProgram::application->setScreenMode(TScreen::screenMode);
      Repaint();
      break;
#endif
#ifndef __DJGPP__
    case SIGSEGV:
      signal(SIGSEGV, SIG_DFL);
      /*
         Try to save all opened editors 
       */
      SaveAll();
      /*
         Try to save the project 
       */
      SaveProject();
      // Try the best to restore all to default (keyboard, screen, mouse )
      destroy(App);
      chdir(startup_directory);
      remove_tmpdir();
      raise(signo);
      break;
#endif
#ifndef __DJGPP__
    case SIGTSTP:
      TProgram::application->suspend();
      signal(SIGTSTP, SIG_DFL);
      signal(SIGCONT, rhide_sig);
      raise(signo);
      break;
    case SIGCONT:
      chdir(project_directory);
      TProgram::application->resume();
      check_for_external_modifications();
      Repaint();
      ClearFileHash();
      ClearFindCache();
      already_maked = 0;
      signal(SIGCONT, SIG_DFL);
      signal(SIGTSTP, rhide_sig);
      raise(signo);
      break;
#endif
    default:
      break;
  }
}

static void
init_signals()
{
  if (!no_sigint)
    signal(SIGINT, rhide_sig);
  else
    signal(SIGINT, SIG_IGN);
  signal(SIGSEGV, rhide_sig);
#ifndef __DJGPP__
  signal(SIGTSTP, rhide_sig);
  signal(SIGTTOU, SIG_IGN);
  signal(SIGWINCH, rhide_sig);
#endif
}

#ifdef __DJGPP__
static int _title_seg = -1, _title_selector = 0;

#define _TITLE_SIZE 80
#define _OLD_TITLE 0
#define _NEW_TITLE _OLD_TITLE+_TITLE_SIZE
#define _OLD_SUBTITLE _NEW_TITLE+_TITLE_SIZE
#define _NEW_SUBTITLE _OLD_SUBTITLE+_TITLE_SIZE
#define TITLE_SIZE _NEW_SUBTITLE+_TITLE_SIZE

static void
free_title_seg()
{
  __dpmi_regs r;

  if (_title_seg == -1)
    return;

  /*
     Restore the old virtual machine title 
   */
  r.x.ax = 0x168E;
  r.x.dx = 1;
  r.x.es = _title_seg;
  r.x.di = _OLD_TITLE;
  r.x.cx = _TITLE_SIZE;
  __dpmi_int(0x2f, &r);

  /*
     Save the old application title 
   */
  r.x.ax = 0x168E;
  r.x.dx = 0;
  r.x.es = _title_seg;
  r.x.di = _OLD_SUBTITLE;
  r.x.cx = _TITLE_SIZE;
  __dpmi_int(0x2f, &r);

  __dpmi_free_dos_memory(_title_selector);
  _title_seg = -1;
}

static void
_init_title()
{
  __dpmi_regs r;

  if (no_title)
    return;
  _title_seg = __dpmi_allocate_dos_memory(TITLE_SIZE / 16, &_title_selector);
  if (_title_seg == -1)
    return;

  /*
     Save the old virtual machine title 
   */
  r.x.ax = 0x168E;
  r.x.dx = 3;
  r.x.es = _title_seg;
  r.x.di = _OLD_TITLE;
  r.x.cx = _TITLE_SIZE;
  __dpmi_int(0x2f, &r);

  /*
     Save the old application title 
   */
  r.x.ax = 0x168E;
  r.x.dx = 2;
  r.x.es = _title_seg;
  r.x.di = _OLD_SUBTITLE;
  r.x.cx = _TITLE_SIZE;
  __dpmi_int(0x2f, &r);

  atexit(free_title_seg);
}

static void
_setup_title(const char *title, int offset, int subfunc)
{
  __dpmi_regs r;

  if (_title_seg == -1)
    return;
  movedata(_my_ds(), (unsigned) title, _title_selector, offset, _TITLE_SIZE);
  _farpokeb(_title_selector, offset + _TITLE_SIZE, 0);
  r.x.ax = 0x168E;
  r.x.dx = subfunc;
  r.x.es = _title_seg;
  r.x.di = offset;
  __dpmi_int(0x2f, &r);
}

#endif

void
setup_main_title()
{
#ifdef __DJGPP__
  _setup_title(IDEVersion, _NEW_TITLE, 1);
#endif
}

void
setup_title(const char *title)
{
#ifdef __DJGPP__
  _setup_title(title, _NEW_SUBTITLE, 0);
#else
  if (title)
    return;
#endif
}

int
main(int argc, char **argv)
{
  init_rhide(argc, argv);
  if (call_usage)
    usage();
  if (dump_env)
    dump_environment();
  LoadKeys();
  getcwd(startup_directory, 255);
#ifdef __DJGPP__
  extern int w95_detected;

  if (w95_detected)
  {
    _init_title();
    setup_main_title();
  }
#endif
#ifdef INTERNAL_DEBUGGER
  InitDebuggerInterface();
  TIDEFileEditor::externalFormatLine = DebuggerFormatLine;
#endif
  App = new IDE();
  init_signals();
  if (!PRJNAME)
    find_project();
  if (CheckIDE() != 1)
  {
#ifdef __DJGPP__
    if (getenv("DJDIR") == NULL)
    {
      BigmessageBox(mfError | mfOKButton,
                    _
                    ("RHIDE has detected, that the environment variable 'DJDIR' "
                     "has not been set. This means, you haven't installed DJGPP correct. "
                     "Please read the file README.1ST from the DJGPP distribution how to "
                     "install DJGPP. If you continue now, you will get probably many "
                     "errors."));
    }
#endif
    {
      pop_environment();
      OpenProject(PRJNAME);
      if (!PRJNAME)
      {
        if (!EDITNAME)
          About();
        if (EDITNAME)
          OpenEditor(EDITNAME, true);
      }
      should_update = 1;
      App->update();
      App->run();
    }
#ifndef __DJGPP__
    signal(SIGSTOP, SIG_DFL);
    signal(SIGINT, SIG_DFL);
    signal(SIGWINCH, SIG_DFL);
#endif
  }
  destroy(App);
  chdir(startup_directory);
  return 0;
}

void
IDE::fileOpen()
{
  ushort result;
  TFileDialog *dialog;

  InitHistoryID(RHIDE_History_source_file);
  dialog = FileOpenDialog(Project.defaultprojectmask,
                          _("Open a file"), _("~N~ame"),
                          fdOpenButton | fdHelpButton,
                          RHIDE_History_source_file, default_directory);
  dialog->helpCtx = hcOpenEditor;
  TProgram::deskTop->insert(dialog);
  dialog->setState(sfModal, True);
  result = dialog->execute();
  if (result != cmCancel)
  {
    char fileName[PATH_MAX];

    string_free(Project.defaultprojectmask);
    string_free(default_directory);
    string_dup(default_directory, dialog->directory);
    string_dup(Project.defaultprojectmask, dialog->wildCard);
    dialog->getData(fileName);
    TCEditWindow *win = is_on_desktop(fileName);

    if (win != NULL)
      win->select();
    else
      openEditor(fileName, True);
  }
  TProgram::deskTop->remove(dialog);
  destroy(dialog);
}



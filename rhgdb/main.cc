/* Copyright (C) 1996-2000 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <locale.h>
#include <unistd.h>
#include <signal.h>
#include <setjmp.h>
#ifdef __DJGPP__
#include <dir.h>
#include <sys/exceptn.h>
#include <dpmi.h>
#include <crt0.h>
#endif
#ifdef __linux__
#undef ERR
#include <curses.h>
#endif
#define Uses_TEventQueue
#define Uses_TApplication
#define Uses_TScreen
#define Uses_TDeskTop
#define Uses_TStatusLine
#define Uses_TStatusDef
#define Uses_TStatusItem
#define Uses_TKeys
#define Uses_TPalette
#define Uses_MsgBox
#define Uses_TDialog
#define Uses_TSCollection
#define Uses_TMenuBar
#define Uses_TSubMenu
#define Uses_TMenuItem
#define Uses_TFileDialog
#define Uses_TFileInputLine
#define Uses_TStaticText
#define Uses_TButton
#define Uses_TFileViewer
#define Uses_TGKey

#define Uses_TDirList
#define Uses_TParamList
#define Uses_tvutilFunctions
#define Uses_tvutilCommands
#define Uses_TWindowList
#define Uses_TInputLinePiped
#define Uses_TCEditor
#define Uses_TCEditor_Internal
#define Uses_TWatchListBox
#define Uses_tvgdbCommands
#define Uses_tvgdbFunctions
#define Uses_TDisassemblerWindow
#define Uses_TDataWindow
#define Uses_TInspector
#define Uses_TInspectDialog
#include <ceditor.h>
#include <libtvuti.h>
#include <rhutils.h>
#include <libtvgdb.h>
#include <librhgdb.h>
#include "rhgdb.h"


extern int RunProgram(const char *cmd,
                      Boolean redir_stderr, Boolean redir_stdout,

                      Boolean SwitchToUser);

char *cpp_outname, *cpp_errname;

int
RunProgram(const char *cmd,
           Boolean redir_stderr, Boolean redir_stdout, Boolean SwitchToUser)
{
  int retval;

  if (redir_stdout == True)
    cpp_outname = open_stdout();
  if (redir_stderr == True)
    cpp_errname = open_stderr();
  if (SwitchToUser == True)
  {
    TProgram::deskTop->setState(sfVisible, False);
    TProgram::application->suspend();
  }
#ifdef __DJGPP__
  __djgpp_exception_toggle();
#endif
  retval = system(cmd);
#ifdef __DJGPP__
  __djgpp_exception_toggle();
#endif
  if (redir_stdout == True)
    close_stdout();
  if (redir_stderr == True)
    close_stderr();
  if (SwitchToUser == True)
  {
    TProgram::application->resume();
    TProgram::deskTop->setState(sfVisible, True);
  }
  return retval;
}


void SaveScreen();
void RestoreScreen();

extern char build_date[];
extern char build_time[];
extern char RHGDBVersion[];

char *progname = NULL;
char *init_name = NULL;
TDirList *src_dirs = NULL;
int tabsize = 8;
int use_dual_display = 0;

TDisassemblerWindow *dis_win = NULL;

static void ReadOptions(char *);
static void SaveOptions(char *);

class RHGDBApp:public TApplication
{
public:
  RHGDBApp();
  virtual void handleEvent(TEvent &);
  virtual TPalette & getPalette() const;
  static TMenuBar *initMenuBar(TRect);
  static TStatusLine *initStatusLine(TRect);
};

RHGDBApp *app;


#include <pal.h>
#define cpRHGDBColor cpIDEColor
#define cpRHGDBBlackWhite cpIDEBlackWhite
#define cpRHGDBMono cpIDEMono

const char *_cpRHGDBColor = cpRHGDBColor;
const char *_cpRHGDBBlackWhite = cpRHGDBBlackWhite;
const char *_cpRHGDBMono = cpRHGDBMono;
int cpColorSize = sizeof(cpRHGDBColor) - 1;
int cpBlackWhiteSize = sizeof(cpRHGDBBlackWhite) - 1;
int cpMonoSize = sizeof(cpRHGDBMono) - 1;

static int last_palette = -1;

TPalette & RHGDBApp::getPalette() const
{
  static TPalette
  color(_cpRHGDBColor, cpColorSize);
  static TPalette
  bw(_cpRHGDBBlackWhite, cpBlackWhiteSize);
  static TPalette
  mono(_cpRHGDBMono, cpMonoSize);

  if (last_palette != appPalette)
  {
    last_palette = appPalette;
    TCEditor::colorsCached = 0;
  }
  return appPalette == apBlackWhite ? bw :
    appPalette == apMonochrome ? mono : color;
}

RHGDBApp::RHGDBApp():
TApplication(),
TProgInit(RHGDBApp::initStatusLine,
          RHGDBApp::initMenuBar, RHGDBApp::initDeskTop)
{
}

TMenuBar *
RHGDBApp::initMenuBar(TRect r)
{
  r = TProgram::application->getExtent();
  r.b.y = r.a.y + 1;
  return new TMenuBar(r,
                      *new TSubMenu(_("~ð~"), kbAltSpace) +
                      *new TMenuItem(_("about RH~G~DB"), cmAbout, kbNoKey,
                                     hcNoContext) + *new TSubMenu(_("~F~ile"),
                                                                  kbAltF) +
                      *new TMenuItem(_("~O~pen"), cmOpen, kbF3, hcNoContext,
                                     "F3") + *new TMenuItem(_("~Q~uit"),
                                                            cmQUIT, kbAltX,
                                                            hcNoContext,
                                                            "Alt+X") +
                      newLine() + *new TMenuItem(_("~G~oto Line..."),
                                                 cmGotoLine, kbNoKey,
                                                 hcNoContext,
                                                 "Ctrl+J") +
                      *new TMenuItem(_("~J~ump to function"), cmJumpFunction,
                                     kbAltF2, hcNoContext,
                                     "Alt+F2") + *new TSubMenu(_("~R~un"),
                                                               kbAltR) +
                      *new TMenuItem(_("~R~un"), cmRun, kbCtrlF9, hcNoContext,
                                     "Ctrl+F9") +
                      *new TMenuItem(_("~S~tep over"), cmStep, kbF8,
                                     hcNoContext,
                                     "F8") +
                      *new TMenuItem(_("Step over (No display swap)"),
                                     cmStepNo, kbShiftF8, hcNoContext,
                                     "Shift+F8") +
                      *new TMenuItem(_("~T~race into"), cmTrace, kbF7,
                                     hcNoContext,
                                     "F7") +
                      *new TMenuItem(_("Trace into (No display swap)"),
                                     cmTraceNo, kbShiftF7, hcNoContext,
                                     "Shift+F7") +
                      *new TMenuItem(_("~G~o to cursor"), cmGoto, kbF4,
                                     hcNoContext,
                                     "F4") +
                      *new TMenuItem(_("Go to cursor (No display swap)"),
                                     cmGotoNo, kbShiftF4, hcNoContext,
                                     "Shift+F4") +
                      *new TMenuItem(_("~P~rogram reset"), cmReset, kbCtrlF2,
                                     hcNoContext,
                                     "Ctrl+F2") + newLine() +
                      *new TMenuItem(_("~M~ain function"), cmMainFunction,
                                     kbNoKey,
                                     hcNoContext) +
                      *new TMenuItem(_("~A~rguments ..."), cmProgArgs,
                                     kbNoKey,
                                     hcNoContext) +
                      *new TSubMenu(_("~D~ebug"),
                                    kbAltD) +
                      *new TMenuItem(_("~S~et/Reset Breakpoint"),
                                     cmToggleBreak, kbCtrlF8, hcNoContext,
                                     "Ctrl+F8") +
                      *new TMenuItem(_("~E~valuate/Modify"), cmEvaluate,
                                     kbCtrlF4, hcNoContext,
                                     "Ctrl+F4") +
                      *new TMenuItem(_("~W~atch an expression"),
                                     cmAddWatchEntry, kbCtrlF7, hcNoContext,
                                     "Ctrl+F7") +
                      *new TMenuItem(_("~B~reakpoints"), cmBreakPoints,
                                     kbNoKey, hcNoContext,
                                     "") + newLine() +
                      *new TMenuItem(_("~D~isassembler window"), cmDisWindow,
                                     kbNoKey, hcNoContext,
                                     "") + *new TMenuItem(_("D~a~ta window"),
                                                          cmDataWindow,
                                                          kbNoKey,
                                                          hcNoContext,
                                                          "") +
                      *new TMenuItem(_("S~t~ack window"), cmStackWindow,
                                     kbNoKey, hcNoContext,
                                     "") + *new TMenuItem(_("~C~all stack"),
                                                          cmCallStack,
                                                          kbCtrlF3,
                                                          hcNoContext,
                                                          "Ctrl+F3") +
                      *new TMenuItem(_("List of ~F~unctions"), cmFunctionList,
                                     kbNoKey, hcNoContext,
                                     "") + newLine() +
                      *new TMenuItem(_("~I~nspect data"), cmInspectData,
                                     kbCtrlF6, hcNoContext,
                                     "Ctrl+F6") +
                      *new TSubMenu(_("~O~ptions"),
                                    kbAltO) +
                      *new TMenuItem(_("~D~irectories for sources"),
                                     cmSrcDirs, kbNoKey, hcNoContext,
                                     "") + *new TMenuItem(_("~P~references"),
                                                          cmPreferences,
                                                          kbNoKey,
                                                          hcNoContext,
                                                          "") +
                      *new TMenuItem(_("~S~ave options"), cmSaveOptions,
                                     kbNoKey, hcNoContext,
                                     "") + *new TMenuItem(_("~L~oad options"),
                                                          cmLoadOptions,
                                                          kbNoKey,
                                                          hcNoContext,
                                                          "") +
                      *new TSubMenu(_("~W~indows"),
                                    kbAltW) + *new TMenuItem(_("~S~ize/move"),
                                                             cmResize,
                                                             kbCtrlF5,
                                                             hcNoContext,
                                                             "Ctrl+F5") +
                      *new TMenuItem(_("~Z~oom"), cmZoom, kbF5, hcNoContext,
                                     "F5") + *new TMenuItem(_("~N~ext"),
                                                            cmNext, kbF6,
                                                            hcNoContext,
                                                            "F6") +
                      *new TMenuItem(_("~P~revious"), cmPrev, kbShiftF6,
                                     hcNoContext,
                                     "Shift+F6") +
                      *new TMenuItem(_("~C~lose"), cmClose, kbAltF3,
                                     hcNoContext,
                                     "Alt+F3") + newLine() +
                      *new TMenuItem(_("~W~atches"), cmShowWatchWindow,
                                     kbNoKey,
                                     hcNoContext) +
                      *new TMenuItem(_("~U~ser screen"), cmUserScreen,
                                     kbAltF5, hcNoContext,
                                     "Alt+F5") +
                      *new TMenuItem(_("~W~indow list"), cmWindowList, kbAlt0,
                                     hcNoContext, "Alt+0"));
}

TStatusLine *
RHGDBApp::initStatusLine(TRect r)
{
  r = TProgram::application->getExtent();
  r.a.y = r.b.y - 1;
  return new TStatusLine(r,
                         *new TStatusDef(hcFunctionInputLine,
                                         hcFunctionInputLine) +
                         *new TStatusItem(_("~Ctrl+F1~ Select from a list"),
                                          kbCtrlF1,
                                          cmSelectFunction) +
                         *new TStatusDef(hcWatchListBox,
                                         hcWatchListBox) +
                         *new TStatusItem(_("~Ins~ Add"), kbIns,
                                          cmAddWatchEntry) +
                         *new TStatusItem(_("~Del~ Delete"), kbDel,
                                          cmRemoveWatch) +
                         *new TStatusItem(_("~Enter~ Edit"), kbEnter,
                                          cmEditWatch) +
                         *new TStatusItem(_("~F5~ Zoom"), kbF5,
                                          cmZoom) +
                         *new TStatusItem(_("~Alt+X~ Quit"), kbAltX,
                                          cmQuit) +
                         *new TStatusItem(_("~F10~ Menu"), kbF10,
                                          cmMenu) + *new TStatusDef(0,
                                                                    0xFFFF) +
                         *new TStatusItem(NULL, kbAltX,
                                          cmQUIT) +
                         *new TStatusItem(_("~F7~ Trace"), kbF7,
                                          cmTrace) +
                         *new TStatusItem(_("~F8~ Step"), kbF8,
                                          cmStep) +
                         *new TStatusItem(_("~F4~ Goto"), kbF4,
                                          cmGoto) +
                         *new TStatusItem(_("~^F2~ Reset"), kbCtrlF2,
                                          cmReset) +
                         *new TStatusItem(_("~^F8~ Break"), kbCtrlF8,
                                          cmToggleBreak) +
                         *new TStatusItem(_("~^F4~ Eval"), kbCtrlF4,
                                          cmEvaluate) +
                         *new TStatusItem(_("~^F9~ Run"), kbCtrlF9,
                                          cmRun) + *new TStatusItem(NULL,
                                                                    kbCtrlF7,
                                                                    cmAddWatchEntry)
                         + *new TStatusItem(NULL, kbAltF5,
                                            cmUserScreen) +
                         *new TStatusItem(NULL, kbAlt0,
                                          cmWindowList) +
                         *new TStatusItem(NULL, kbF10,
                                          cmMenu) + *new TStatusItem(NULL,
                                                                     kbF2,
                                                                     cmSaveFile));
}

TParamList *ProgArgs = NULL;

static void
About()
{
  TDialog *dialog;
  TStaticText *text;
  char buffer[1000];
  extern char *version;         // gdb version

  dialog = new TDialog(TRect(0, 0, 60, 17), _("About RHGDB"));
  dialog->options |= ofCentered;
  sprintf(buffer, "\003%s\n"
          "\003(%s)\n"
          "\003\n"
          "\003%s\n"
          "\003%s\n"
          "\003(%s%s)\n"
          "\003\n"
          "\003%s, %d-%d\n",
          RHGDBVersion,
          build_date,
          _("RHGDB is a source level debugger"),
          _("with Turbo Vision interface"),
          _("based on GDB "), version,
          _("Copyright (C) by Robert H”hne"),
          1996,2001);
  text = new TStaticText(TRect(0, 0, 50, 9), buffer);
  text->options |= ofCentered;
  dialog->insert(text);
  dialog->
    insert(new TButton(TRect(25, 13, 35, 15), _("~O~K"), cmOK, bfDefault));
  TProgram::deskTop->execView(dialog);
  destroy(dialog);
}

static char *main_function = NULL;

static void
MainFunction()
{
  char buffer[256];

  if (main_function)
    strcpy(buffer, main_function);
  else
    buffer[0] = 0;
  if (inputBox(_("Name of the main function"), _("~N~ame"),
               buffer, 255) == cmOK)
  {
    string_free(main_function);
    string_dup(main_function, buffer);
  }
}

extern unsigned long stop_pc;

static void
OpenDisWin()
{
  if (!dis_win)
  {
    TRect r(1, 1, 79, 20);

    dis_win = new TDisassemblerWindow(r, _("Disassembler window"));
    AddWindow(dis_win, (TWindow **) & dis_win);
  }
}

static void
AddInspect(const char *expr)
{
  char buf[1000];

  strcpy(buf, expr ? expr : "");
  TInspectDialog *insp =

    new TInspectDialog(TRect(0, 0, 40, 8), "Inspect", buf);
  insp->options |= ofCentered;
  if (TProgram::deskTop->execView(insp) == cmOK)
  {
    insp->getData(buf);
    TInspector *w = new TInspector(TProgram::deskTop->getExtent(), buf);

    w->helpCtx = hcInspector;
    w->options |= ofCentered;
    w->update(buf);
    AddWindow(w, (TWindow **) & w);
  }
  TObject::destroy(insp);
}


static TDataWindow *stack_win = NULL;

void
RHGDBApp::handleEvent(TEvent & event)
{
  static char *callstack_name = NULL;
  static Boolean Focus = False;

  if (event.what == evCommand && event.message.command == cmQuit)
    event.message.command = cmQUIT;
  TApplication::handleEvent(event);
  switch (event.what)
  {
    case evBroadcast:
      switch (event.message.command)
      {
        case cmFocusWindow:
          Focus = True;
        case cmOpenWindow:
          string_dup(callstack_name, (char *) event.message.infoPtr);
          clearEvent(event);
          break;
        case cmGotoWindowLine:
          if (!callstack_name)
            break;
          OpenViewer(callstack_name, event.message.infoInt, False, Focus);
          string_free(callstack_name);
          Focus = False;
          clearEvent(event);
          break;
        default:
          break;
      }
      break;
    case evCommand:
      switch (event.message.command)
      {
        case cmShowWatchWindow:
          OpenWatchWindow();
          clearEvent(event);
          break;
        case cmDisWindow:
          OpenDisWin();
          if (debugger_started())
            dis_win->update(stop_pc);
          clearEvent(event);
          break;
        case cmDataWindow:
          TDataWindow * data_win;
          if ((data_win = TDataWindow::createNew()))
            AddWindow(data_win);
          clearEvent(event);
          break;
        case cmStackWindow:
          if (!stack_win)
          {
            if ((stack_win = TDataWindow::stackWindow()))
              AddWindow(stack_win, (TWindow **) & stack_win);
          }
          else
            stack_win->select();
          clearEvent(event);
          break;
        case cmInspectData:
          AddInspect(RHGDBWordUnderCursor());
          clearEvent(event);
          break;
        case cmMainFunction:
          MainFunction();
          clearEvent(event);
          break;
        case cmAbout:
          About();
          clearEvent(event);
          break;
        case cmCallStack:
          ShowCallStackWindow();
          clearEvent(event);
          break;
        case cmProgArgs:
          editParamList(ProgArgs, _("Program arguments"), 2);
          clearEvent(event);
          break;
        case cmFunctionList:
        {
          char regex[256];

          regex[0] = 0;
          function_entry *func;

          if (inputBox(_("Regular expression to list functions"),
                       _("~E~xpression"), regex, 255) == cmOK)
          {
            if (SelectFunction
                (_("List of functions"), regex, NULL, NULL, &func) == cmOK)
            {
              OpenViewer(func->file_name, func->line_number, False);
              CenterCursor();
            }
          }
          clearEvent(event);
          break;
        }
        case cmPreferences:
          Preferences();
          clearEvent(event);
          break;
        case cmSrcDirs:
          SourceDirectories();
          clearEvent(event);
          break;
        case cmBreakPoints:
          BreakDialog();
          repaint();
          clearEvent(event);
          break;
        case cmOpen:
          FileOpen();
          clearEvent(event);
          break;
        case cmSaveOptions:
          SaveOptions();
          clearEvent(event);
          break;
        case cmLoadOptions:
          LoadOptions();
          clearEvent(event);
          break;
        case cmAddWatchEntry:
          AddWatchEntry(RHGDBWordUnderCursor());
          clearEvent(event);
          break;
        case cmEvaluate:
          Evaluate(RHGDBWordUnderCursor());
          clearEvent(event);
          break;
        case cmUserScreen:
        {
          TMouse::suspend();
#if 0
          TScreen::suspend();
#else
          RestoreScreen();
#endif
          do
          {
            clearEvent(event);
#ifdef __linux__
            timeout(1);
#endif
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
#if 0
          TScreen::resume();
#endif
          TMouse::resume();
          repaint();
          clearEvent(event);
          break;
        }
        case cmStep:
          STEP();
          clearEvent(event);
          break;
        case cmStepNo:
          STEP(0);
          clearEvent(event);
          break;
        case cmTrace:
          TRACE();
          clearEvent(event);
          break;
        case cmTraceNo:
          TRACE(0);
          clearEvent(event);
          break;
        case cmRun:
          RUN();
          clearEvent(event);
          break;
        case cmGoto:
          GOTO();
          clearEvent(event);
          break;
        case cmGotoNo:
          GOTO(0);
          clearEvent(event);
          break;
        case cmReset:
          RESET();
          clearEvent(event);
          break;
        case cmQUIT:
          RESET();
          endModal(cmQuit);
          clearEvent(event);
          break;
        case cmWindowList:
          ShowWindowList(_("All windows on the desktop"));
          clearEvent(event);
          break;
        case cmToggleBreak:
          ToggleBreak();
          repaint();
          clearEvent(event);
          break;
      }
      break;
  }
}

static void
UPDATE_WATCH()
{
  if (watchwindow)
    watches->update();
  UpdateCallStackWindow();
  TDataWindow::updateAll();
  if (dis_win)
    dis_win->update(stop_pc);
}

static void InitDebuggerInterface();

static int show_usage = 0;
static char *prog_name = NULL;


static __attribute__ ((noreturn))
     void
     usage()
{
//  TEventQueue::suspend();
//  TScreen::suspend();

  TEventQueue::suspend();
  TScreen::suspend();

  fprintf(stderr, _("usage: %s [options] exe-file [[arg1 [arg2 ...]]]\n"),
          prog_name);
  fprintf(stderr, _("options:\n"));
  fprintf(stderr,
          _("            -c : show filename exactly (no case conversion)\n"));
  fprintf(stderr,
          _("            -y : force to use long filenames (Windows 95)\n"));
  fprintf(stderr,
          _("            -n : do not use long filenames (Windows 95)\n"));
  fprintf(stderr, _("            -L LANGUAGE : use language LANGUAGE\n"));
  fprintf(stderr, _("            -h : show this help\n"));
  fprintf(stderr,
          _("            -b : use BIOS calls for toggle the blinkstate\n"));
  fprintf(stderr, _("            -p : do not convert the numpad keys\n"));
  fprintf(stderr,
          _("            -G num : Use methode <num> for screen swapping\n"));
  fprintf(stderr, _("            -K : Use the BIOS for keyboard input\n"));
  fprintf(stderr, _("            -D : Use dual-display debugging\n"));
  fprintf(stderr,
          _("            -i INI_FILE : Use INI_FILE as config file\n"));
  fprintf(stderr, _("            -M : Do not install the mouse handler\n"));
  fflush(stderr);
  exit(-1);
}

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

#define Usage() do { show_usage = 1; return; } while (0)

static void
parse_commandline(int argc, char *argv[])
{
  /*
     At first look for an environment variable $(RHGDBOPT) and
     use the options from that variable as they were put at the
     beginning of the commandline, so they can be overwritten 
   */
  char *rhgdb_opt = getenv("RHGDBOPT");
  char *rhgdb_opt_end = NULL;
  int i = 1;
  char *arg;

  ProgArgs = new TParamList();
  if (rhgdb_opt && *rhgdb_opt)
  {
    // strip leading whitespaces
    while (*rhgdb_opt == ' ')
      rhgdb_opt++;
    if (*rhgdb_opt)
    {
      // skip trailing whitespaces
      rhgdb_opt_end = rhgdb_opt + strlen(rhgdb_opt) - 1;
      while (*rhgdb_opt_end == ' ')
        rhgdb_opt_end--;
    }
  }
  while ((arg = next_option(rhgdb_opt, rhgdb_opt_end, i, argc, argv)) != NULL)
  {
    if (progname)               // everything after the exe-file are the arguments for
      // the prog
    {
      ProgArgs->insert(strdup(arg));
      continue;
    }
    if (arg[0] == '-')
    {
      switch (arg[1])
      {
        case 'D':
          use_dual_display = 1;
          break;
        case 'M':
          extern int use_mouse_handler;

          use_mouse_handler = 0;
          break;
        case 'K':
          TGKey::useBIOS = 1;
          break;
        case 'G':
//          extern int screen_saving;
          arg = next_option(rhgdb_opt, rhgdb_opt_end, i, argc, argv);
          if (!arg)
            Usage();
//          screen_saving = atoi(arg);
          break;
        case 'p':
#if 0
          extern int convert_num_pad;

          convert_num_pad = 0;
#endif
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
          static char language[256];

          arg = next_option(rhgdb_opt, rhgdb_opt_end, i, argc, argv);
          if (!arg)
            Usage();
          strcpy(language, "LANGUAGE=");
          strcat(language, arg);
          putenv(language);
          break;
        }
        case 'i':
          arg = next_option(rhgdb_opt, rhgdb_opt_end, i, argc, argv);
          if (!arg)
            Usage();
          init_name = string_dup(arg);
          break;
        case 'h':
          Usage();
        default:
          Usage();
      }
    }
    else
    {
      progname = string_dup(arg);
    }
  }
  if (!init_name && !progname)
    Usage();
}



static TInputLine *
rhgdbCreateInputLine(const TRect & rect, int aMaxLen)
{
  return new TInputLinePiped(rect, aMaxLen);
}


static void
init_rhgdb(int __crt0_argc, char **__crt0_argv)
{
#ifdef __DJGPP__
  __crt0_load_environment_file("rhgdb");	// when the exe has an other name
  // than rhide force to load the
  // variables for RHIDE
#endif
  char *locale_dir = getenv("LOCALEDIR");

#ifndef __DJGPP__
  if (!locale_dir)
  {
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
    char *file = expand_spec(spec, NULL);
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
  if (!locale_dir)
  {
    locale_dir = getenv("DJDIR");
    if (locale_dir)
    {
      static char tmp[512];

      strcpy(tmp, locale_dir);
      strcat(tmp, "/share/locale");
      locale_dir = tmp;
    }
    else
    {
      locale_dir = "/share/locale";
    }
  }
  setlocale(LC_ALL, "");
  BINDTEXTDOMAIN("rhide", locale_dir);
  TEXTDOMAIN("rhide");
#if 0
  extern int convert_num_pad;

  convert_num_pad = 1;
#endif
#ifdef __DJGPP__
  extern int __crt0_argc;
  extern char **__crt0_argv;
#endif

  CreateInputLine = &rhgdbCreateInputLine;

  prog_name = __crt0_argv[0];
  parse_commandline(__crt0_argc, __crt0_argv);

  TScreen::suspend();
  fprintf(stderr, _("This is %s. Copyright (c) 1996-2001 by Robert H”hne\n"),
          RHGDBVersion);
  TScreen::resume();
  init_syntax();
}

static void
rhgdb_sig(int signo)
{
  switch (signo)
  {
    case SIGINT:
      signal(SIGINT, SIG_DFL);
      if (messageBox(_("Do you really want to interrupt RHGDB?"),
                     mfConfirmation | mfYesButton | mfNoButton) == cmYes)
      {
        destroy(app);
        exit(-1);
      }
      signal(SIGINT, rhgdb_sig);
      break;
#ifndef __DJGPP__
    case SIGTSTP:
      TProgram::application->suspend();
      signal(SIGTSTP, SIG_DFL);
      signal(SIGCONT, rhgdb_sig);
      break;
    case SIGCONT:
      TProgram::application->resume();
      repaint();
      signal(SIGCONT, SIG_DFL);
      signal(SIGTSTP, rhgdb_sig);
      break;
    default:
      break;
#endif
  }
}

int
main(int argc, char **argv)
{
  char *main_source = NULL;
  int main_line;
  char initname[256];

  init_rhgdb(argc, argv);
  if (show_usage)
    usage();
  string_dup(main_function, "main");
  src_dirs = new TDirList();
  app = new RHGDBApp();
  signal(SIGINT, rhgdb_sig);
#ifndef __DJGPP__
  signal(SIGTSTP, rhgdb_sig);
  signal(SIGTTOU, SIG_IGN);
#endif
  if (!init_name)
  {
    if (!progname)
      usage();
    char *dot;

    strcpy(initname, progname);
    dot = strrchr(initname, '.');
    if (dot)
      *dot = 0;
    strcat(initname, ".rgd");
    init_name = initname;
  }
  ReadOptions(init_name);
  if (!progname)              // probably an old ini file
  {
    char *spec = string_dup("$(subst .rgd,");

#ifndef __linux__
    string_cat(spec, ".exe");
#endif
    string_cat(spec, ",");
    string_cat(spec, init_name);
    string_cat(spec, ")");
    progname = expand_spec(spec, NULL);
    string_free(spec);
  }
  InitDebuggerInterface();
  if (InitRHGDB())
  {
    char *dirname;
    main_source = SourceForMain(&main_line, &dirname);
    if (main_source)
    {
      bool found = false;
      if (dirname)
      {
        char *full_name = string_dup(dirname);
        string_cat(full_name, "/", main_source, NULL);
        found = OpenViewer(full_name, main_line, true);
        string_free(full_name);
      }
      if (!found)
        OpenViewer(main_source, main_line, False);
      CenterCursor();
    }
    else if (main_line != 0)
    {
      OpenDisWin();
      dis_win->update(main_line);
    }
  }
  app->run();
  SaveOptions(init_name);
  destroy(app);
  return 0;
}

void
STEP(int _switch_to_user)
{
  switch_to_user = _switch_to_user;
  if (dis_win && TProgram::deskTop->current == dis_win)
  {
    Step_i();
  }
  else
    Step();
}

void
TRACE(int _switch_to_user)
{
  switch_to_user = _switch_to_user;
  if (dis_win && TProgram::deskTop->current == dis_win)
  {
    Trace_i();
  }
  else
    Trace();
}

void
RESET()
{
  if (!debugger_started())
    return;
  ResetDebugger();
  repaint();
}

void
RUN()
{
  switch_to_user = 1;
  Continue();
}

void
GOTO(int _switch_to_user)
{
  char *fname, *bname;
  int line, column;

  if ((fname = WhereIsCursor(line, column, bname)) == NULL)
    return;
  switch_to_user = _switch_to_user;
  GoToLine(bname, line);
}

void
repaint()
{
  TProgram::deskTop->redraw();
  TProgram::application->Redraw();
}

void
ToggleBreak()
{
  char *fname, *bname;
  int break_number, line, column;

  if ((fname = WhereIsCursor(line, column, bname)) == NULL)
    return;
  break_number = IsBreakPointLine(bname, line);
  if (break_number >= 0)
    RemoveBreakPoint(break_number);
  else
    AddBreakPointLine(bname, line);
}

static int
DoMake()
{
  return 1;
}

static char *
GetProgName()
{
  return progname;
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

  *count = ProgArgs->getCount();
  if (*count == 0)
    return NULL;
  args = (char **) realloc(args, *count * sizeof(char *));

  for (i = 0; i < *count; i++)
  {
    args[i] = (char *) ProgArgs->at(i);
  }
  return args;
}

static void
UserScreen()
{
  TProgram::application->suspend();
}

static void
DebuggerScreen()
{
  TProgram::application->resume();
  repaint();
}

static void
select_source_line(char *fname, int line, char *dirname, char *fullname)
{
  Boolean isSource = False;
  int select_dis_win = dis_win && TProgram::deskTop->current == dis_win;
  fprintf(stderr, "%s   %s   %s\n", fname, dirname, fullname);
  ClearCPULine();
  
  if (fullname)
    isSource = OpenViewer(fullname, line, True);
    
  if (!isSource && dirname)
  {
    if (fname)
    {
      char *full_name = string_dup(dirname);
      string_cat(full_name, "/", fname, NULL);
      isSource = OpenViewer(full_name, line, true);
      string_free(full_name);
    }
  }
    
  if (!isSource && fname)
    isSource = OpenViewer(fname, line, True);

  if (!fname || !isSource)
  {
    OpenDisWin();
    select_dis_win = 1;
  }
  if (dis_win)
  {
    extern unsigned long stop_pc;

    dis_win->update(stop_pc);
  }
  if (select_dis_win)
    dis_win->select();
}

int dual_display_supported();
static int old_mode;

static void
StartSession()
{
  old_mode = TScreen::getCrtMode();
  if (!dual_display && use_dual_display && dual_display_supported())
  {
    TMouse::suspend();
    TScreen::suspend();
    dual_display = 1;
    TScreen::resume();
    TMouse::resume();
    TProgram::application->setScreenMode(old_mode);
  }
  if (watchwindow)
    watchwindow->select();
}

static void
EndSession(int exit_code)
{
  if (dual_display && use_dual_display)
  {
    TMouse::suspend();
    TScreen::suspend();
    dual_display = 0;
    TScreen::resume();
    TMouse::resume();
    TProgram::application->setScreenMode(old_mode);
  }

  messageBox(mfInformation | mfOKButton,
             _("Program exit code: %d (0x%04x)"), exit_code, exit_code);

  repaint();
}

static void
BreakSession()
{
  if (dual_display && use_dual_display)
  {
    TMouse::suspend();
    TScreen::suspend();
    dual_display = 0;
    TScreen::resume();
    TMouse::resume();
    TProgram::application->setScreenMode(old_mode);
  }
  repaint();
}

static char *
GetMainFunction()
{
  return main_function;
}

static void
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

long
TimeOfFile(const char *, Boolean, Boolean)
{
  return 0;
}

static char read_buffer[1024];
static FILE *opt_file;

static void
WriteIDEOptions()
{
  int i, count;

  fprintf(opt_file, "[IDE]\n");
  fprintf(opt_file, "SCREENMODE: %d\n", TScreen::screenMode);
  count = src_dirs->getCount();
  for (i = 0; i < count; i++)
    fprintf(opt_file, "SOURCEDIR: %s\n", (char *) src_dirs->at(i));
  fprintf(opt_file, "TABSIZE: %d\n", tabsize);
  fprintf(opt_file, "VERBOSEGDB: %d\n", verbose_gdb_commands);
  fprintf(opt_file, "MAIN: %s\n", main_function);
  fprintf(opt_file, "USE_DUAL: %d\n", use_dual_display);
  fprintf(opt_file, "PROGRAM: %s\n", progname);
  count = ProgArgs->getCount();
  for (i = 0; i < count; i++)
  {
    fprintf(opt_file, "ARG: %s\n", (char *) ProgArgs->at(i));
  }
}

static int
read_in_buffer()
{
  if (!fgets(read_buffer, 1023, opt_file))
    return 0;
  if (read_buffer[0])
    read_buffer[strlen(read_buffer) - 1] = 0;
  return 1;
}

static int
key_index(char *key)
{
  int len = strlen(key);

  if (strncmp(read_buffer, key, len) != 0)
    return 0;
  len++;
  while (read_buffer[len] == ' ')
    len++;
  return len;
}

static int
ReadIDEOptions()
{
  int index, retval;
  int read_args = ProgArgs->getCount();

  while ((retval = read_in_buffer()) && read_buffer[0] != '[')
  {
    if ((index = key_index("SCREENMODE")) != 0)
    {
      int mode;

      sscanf(&read_buffer[index], "%d", &mode);
      TProgram::application->setScreenMode(mode);
      TProgram::application->Redraw();
    }
    else if ((index = key_index("ARG")) != 0)
    {
      if (!read_args)           // read only if not given on command_line
        ProgArgs->insert(strdup(&read_buffer[index]));
    }
    else if ((index = key_index("SOURCEDIR")) != 0)
    {
      src_dirs->insert(strdup(&read_buffer[index]));
    }
    else if ((index = key_index("TABSIZE")) != 0)
    {
      sscanf(&read_buffer[index], "%d", &tabsize);
    }
    else if ((index = key_index("USE_DUAL")) != 0)
    {
      sscanf(&read_buffer[index], "%d", &use_dual_display);
    }
    else if ((index = key_index("VERBOSEGDB")) != 0)
    {
      sscanf(&read_buffer[index], "%d", &verbose_gdb_commands);
    }
    else if ((index = key_index("MAIN")) != 0)
    {
      string_free(main_function);
      string_dup(main_function, &read_buffer[index]);
    }
    else if ((index = key_index("PROGRAM")) != 0)
    {
      string_free(progname);
      progname = string_dup(&read_buffer[index]);
    }
  }
  return retval;
}

static void
WriteBreakPoints()
{
  int count, i;
  BreakPoint *b;

  fprintf(opt_file, "[BREAKPOINTS]\n");
  count = BreakPointCount();
  for (i = 0; i < count; i++)
  {
    b = GetBreakPoint(i);
    fprintf(opt_file, "TYPE: %d\n", b->type);
    if (BREAK_LINE(b))
    {
      fprintf(opt_file, "FILE: %s\nLINE: %d\n", b->filename, b->line_number);
    }
    else
    {
      fprintf(opt_file, "FUNCTION: %s\n", b->function);
    }
    if (BREAK_COUNT(b))
    {
      fprintf(opt_file, "COUNT: %d\n", b->count);
    }
    if (BREAK_CONDITION(b))
    {
      fprintf(opt_file, "CONDITION: %s\n", b->condition);
    }
  }
}

#define FREE(x) do {if (x) { free(x); x = NULL; } } while (0)

static int
ReadBreakPoints()
{
  int retval, index;
  BreakPoint _bp;
  BreakPoint *bp = &_bp;

  memset(bp, 0, sizeof(BreakPoint));
  while ((retval = read_in_buffer()) && read_buffer[0] != '[')
  {
    if ((index = key_index("TYPE")) != 0)
    {
      if (bp->type != 0)
        EditBreakPoint(bp, -1);
      FREE(bp->function);
      FREE(bp->condition);
      FREE(bp->filename);
      bp->count = 0;
      bp->line_number = 0;
      bp->type = 0;
      int _type;

      sscanf(&read_buffer[index], "%d", &_type);
      bp->type = _type;
    }
    else if ((index = key_index("COUNT")) != 0)
    {
      sscanf(&read_buffer[index], "%d", &bp->count);
    }
    else if ((index = key_index("LINE")) != 0)
    {
      sscanf(&read_buffer[index], "%d", &bp->line_number);
    }
    else if ((index = key_index("FILE")) != 0)
    {
      bp->filename = strdup(&read_buffer[index]);
    }
    else if ((index = key_index("FUNCTION")) != 0)
    {
      bp->function = strdup(&read_buffer[index]);
    }
    else if ((index = key_index("CONDITION")) != 0)
    {
      bp->condition = strdup(&read_buffer[index]);
    }
  }
  if (bp->type != 0)
    EditBreakPoint(bp, -1);
  FREE(bp->function);
  FREE(bp->condition);
  FREE(bp->filename);
  return retval;
}

static void
WriteWatches()
{
  int i, count;

  fprintf(opt_file, "[WATCHES]\n");
  count = Watches.getCount();
  for (i = 0; i < count; i++)
  {
    fprintf(opt_file, "%s\n", (char *) Watches.at(i));
  }
}

static int
ReadWatches()
{
  int retval;

  while ((retval = read_in_buffer()) && read_buffer[0] != '[')
  {
    if (read_buffer[0] != ' ')
      AddWatch(read_buffer, False);
  }
  return retval;
}

static void
SaveOptions(char *fname)
{
  opt_file = fopen(fname, "w+");
  if (opt_file)
  {
    WriteBreakPoints();
    WriteWatches();
    WriteIDEOptions();
    fclose(opt_file);
  }
}

static void
ReadOptions(char *fname)
{
  int not_end;

  opt_file = fopen(fname, "r");
  if (!opt_file)
    return;
  while ((not_end = read_in_buffer()) && read_buffer[0] != '[');
  while (not_end)
  {
    if (strcmp(read_buffer, "[IDE]") == 0)
      not_end = ReadIDEOptions();
    else if (strcmp(read_buffer, "[WATCHES]") == 0)
      not_end = ReadWatches();
    else if (strcmp(read_buffer, "[BREAKPOINTS]") == 0)
      not_end = ReadBreakPoints();
    else
      while ((not_end = read_in_buffer()) && read_buffer[0] != '[');
  }
  fclose(opt_file);
}

void
SaveOptions()
{
  char fname[256];
  TFileDialog *dialog;

  dialog = new TFileDialog("*.rgd", _("Save options to file"),
                           _("~N~ame"), fdOKButton, 0);
  if (TProgram::deskTop->execView(dialog) != cmCancel)
  {
    strcpy(fname, dialog->directory);
    strcat(fname, dialog->fileName->data);
    SaveOptions(fname);
  }
  destroy(dialog);
}


void
LoadOptions()
{
  char fname[256];
  TFileDialog *dialog;

  dialog = new TFileDialog("*.rgd", _("Read options from file"),
                           _("~N~ame"), fdOpenButton, 0);
  if (TProgram::deskTop->execView(dialog) != cmCancel)
  {
    strcpy(fname, dialog->directory);
    strcat(fname, dialog->fileName->data);
    ReadOptions(fname);
  }
  destroy(dialog);
}

void
FileOpen()
{
  char fname[256];
  TFileDialog *dialog;

  dialog = new TFileDialog("*.*", _("Open a file"),
                           _("~N~ame"), fdOpenButton, 0);
  if (TProgram::deskTop->execView(dialog) != cmCancel)
  {
    strcpy(fname, dialog->directory);
    strcat(fname, dialog->fileName->data);
    OpenViewer(fname, 1, False);
  }
  destroy(dialog);
}

void
SourceDirectories()
{
  editDirList(src_dirs, _("Pathlist for source files"), 1);
}

const char msg[] =
  "RHGDB internal error. Please send a description of this situation\r\n"
  "as most as possible detailed to the author together with the version\r\n"

  "you are using. AND VERY IMPORTANT IS THE NEXT TRACEBACK!!!!\r\n\r\n";

static jmp_buf abort_jmp;

extern "C" __attribute__ ((__noreturn__))
     void abort()
{
  // call at least TEventQueue::resume() to clear the mouse hook
  TEventQueue::suspend();
  TScreen::suspend();
  write(STDERR_FILENO, msg, sizeof(msg) - 1);
  setjmp(abort_jmp);
#ifdef __DJGPP__
  __djgpp_exception_state_ptr = &abort_jmp;
#endif
  raise(SIGABRT);
  _exit(1);
}

/* The following functions are provided here only to make the
   linker happy, since they are refernced by libset */
void
show_message(const char *, const char *, int, int, int)
{
}

void
#if (TCEDITOR_VERSION >= 0x000447UL)
InsertEnviromentVar(const char *variable, const char *contents)
#else
InsertEnviromentVar(char *variable, char *contents)
#endif
{
  insert_variable(variable, contents);
}

void
RunExternalProgram(const char *, int, int)
{
}

void
RunExternalProgram(char *, unsigned int, char *)
{
}


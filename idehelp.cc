/* Copyright (C) 1996-2000 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#define Uses_TProgram
#define Uses_TDeskTop
#define Uses_TDialog
#define Uses_TPalette
#define Uses_TSortedListBox

#define Uses_TStreamableClass

#define Uses_IDEConst
#define Uses_TKeys
#define Uses_TMenuBar
#define Uses_ideCommands
#define Uses_TIDEEditWindow
#define Uses_TIDEFileEditor
#define Uses_TStringCollection
#define Uses_TNoCaseStringCollection
#define Uses_TDialog
#define Uses_TScroller
#include <libide.h>

#define Uses_tvutilCommands
#define Uses_tvutilFunctions
#include <libtvuti.h>

#include <rhutils.h>

#include "rhide.h"

#include <inf.h>
#include <ctype.h>

#define cIDEInfWindowColor "\xA0\xA1\xA2\xA3\xA4\xA5\xA6\xA7\xA8"

#define IW ((TIDEInfWindow *)InfWindow)

__link(RNoCaseStringCollection)
     extern TCEditWindow *clipWindow;
     static char *last_title = NULL;

     static ushort help_dsk_version = 1;

     void SaveHelpDesktop(opstream & os)
{
  os << help_dsk_version;
  os << TInfViewer::BookMark;
  os.writeString(last_title);
}

void
LoadHelpDesktop(ipstream & is)
{
  ushort version;

  ClearHelpDesktop();
  is >> version;
  is >> TInfViewer::BookMark;
  string_free(last_title);

  if (version > 0)
    last_title = is.readString();
}

void
ClearHelpDesktop()
{
  destroy(TInfViewer::BookMark);
  TInfViewer::BookMark = NULL;
}

TRect InfRec(1, 1, 79, 22);

class TIDEInfWindow:public TWindow
{
public:
  TIDEInfWindow(TInfFile *, char *, char *match = NULL);
  virtual void handleEvent(TEvent &);
  virtual TPalette & getPalette() const;
  virtual void changeBounds(const TRect &);
  virtual void close();
  TInfViewer *viewer;
private:
    virtual const char *streamableName() const
  {
    return name;
  }
protected:
    TIDEInfWindow(StreamableInit);
  virtual void write(opstream &);
  virtual void *read(ipstream &);
public:
  static const char *const name;
  static TStreamable *build();
};

TIDEInfWindow::TIDEInfWindow(StreamableInit):
TWindow(streamableInit), TWindowInit(NULL)
{
}

const char *const
  TIDEInfWindow::name = "\x11";

TStreamable *
TIDEInfWindow::build()
{
  return new TIDEInfWindow(streamableInit);
}


__link(RInfViewer) __link(RFrame)
     TStreamableClass
     RIDEInfWindow(TIDEInfWindow::name,

                   TIDEInfWindow::build, __DELTA(TIDEInfWindow));

     void
     TIDEInfWindow::write(opstream & os)
{
  TWindow::write(os);
  os << viewer;
}

static void
Copy2Clip(char *b, long l)
{
  if (clipWindow)
    clipWindow->editor->insertBuffer(b, 0, (unsigned) l, False, True);
}

void *
TIDEInfWindow::read(ipstream & is)
{
  TWindow::read(is);
  is >> viewer;
  viewer->InsertRoutine = Copy2Clip;
  return this;
}

void
TIDEInfWindow::changeBounds(const TRect & r)
{
  TWindow::changeBounds(r);
  InfRec = r;
}

TIDEInfWindow::TIDEInfWindow(TInfFile * hFile, char *context, char *match):
TWindow(InfRec, _("Info Viewer"), wnNoNumber),
TWindowInit(&TIDEInfWindow::initFrame)
{
  TRect r = getExtent();

  r.grow(-1, -1);

  insert((viewer = new TInfViewer(r,
                                  standardScrollBar(sbHorizontal |
                                                    sbHandleKeyboard),
                                  standardScrollBar(sbVertical |
                                                    sbHandleKeyboard), hFile,
                                  context, match)));
  helpCtx = hcInfView;
  viewer->InsertRoutine = Copy2Clip;
}

void
TIDEInfWindow::close()
{
  string_free(last_title);
  last_title = string_dup(title);
  TWindow::close();
}

void
TIDEInfWindow::handleEvent(TEvent & event)
{
  TWindow::handleEvent(event);
  switch (event.what)
  {
    case evCommand:
      switch (event.message.command)
      {
        case cmCancel:
          endModal(event.message.command);
          clearEvent(event);
          break;
        default:
          break;
      }
      break;
    case evBroadcast:
      switch (event.message.command)
      {
        default:
          break;
        case cmInfoAnswer:
          clearEvent(event);
          break;
      }
      break;
    default:
      break;
  }
}

TPalette & TIDEInfWindow::getPalette() const
{
  static TPalette
  color(cIDEInfWindowColor, sizeof(cIDEInfWindowColor) - 1);

  return color;
}

static TWindow *
  InfWindow = NULL;

void
Help()
{
  ushort
    ctx = TProgram::application->getHelpCtx();

  Help(ctx);
}

typedef struct
{
  char *
    help_file;
  char *
    help_string;
}
InfoHelpRec;

typedef struct
{
  ushort
    ctx;
  InfoHelpRec
    help;
}
CtxInfoRec;

#define H(x) {hc##x,{"rhide",#x}}
#define HH(x,y) {hc##x,{"rhide",y}}

static CtxInfoRec
  known_contexts[] = {
  {hcOptFlagList, {"gcc", "Optimize Options"}},
  {hcDebFlagList, {"gcc", "Debugging Options"}},
  {hcWarnFlagList, {"gcc", "Warning Options"}},
  {hcCFlagList, {"gcc", "C Dialect Options"}},
  {hcCXXFlagList, {"gcc", "C++ Dialect Options"}},
  {hcPascalFlagList, {"gpc", "Pascal Dialect Options"}},
  {hcPascalFlagList, {"fpc", "Free Pascal Compiler Options"}},
  {hcFortranFlagList, {"g77", "Fortran Dialect Options"}},
  H(About),
  HH(BugReport, "Bug report"),
  H(FSDB),
  H(GREP),
  H(GDB),
  HH(Repaint, "Refresh Desktop"),
  HH(ShowCalculator, "Calculator"),
  H(Puzzle),
  H(Calendar),
  HH(Ascii, "ASCII table"),
  H(File),
  HH(OpenEditor, "Open"),
  HH(NewEditor, "New"),
  HH(SaveEditor, "Save"),
  HH(SaveAs, "Save as"),
  HH(SaveAll, "Save all"),
  HH(DosShell, "DOS Shell"),
  HH(QUIT, "Exit"),
  H(Edit),
  H(Undo),
  H(Redo),
  H(Cut),
  H(Copy),
  H(Paste),
  HH(ShowClip, "Show Clipboard"),
  H(Clear),
  HH(CopyClipWin, "Copy to Windows clipboard"),
  HH(PasteClipWin, "Paste from Windows clipboard"),
  HH(ExpandAllTabs, "Expand all tabs"),
  HH(CompactBuffer, "Compact text"),
  H(Macro),
  H(Record),
  H(Stop),
  H(Play),
  H(Search),
  H(Find),
  H(Replace),
  HH(SearchAgain, "Search again"),
  HH(GotoLine, "Goto line"),
  HH(JumpToFunction, "Jump to function"),
  HH(NextMessage, "Next message"),
  HH(PrevMessage, "Previous message"),
  HH(RunMenu, "RUN"),
  H(Run),
  HH(Step, "Step over"),
  HH(StepNo, "Step over"),
  HH(Trace, "Trace into"),
  HH(TraceNo, "Trace into"),
  HH(Goto, "Go to cursor"),
  HH(GotoNo, "Go to cursor"),
  HH(Finish, "Until return"),
  HH(Reset, "Program reset"),
  HH(MainFunction, "Main function"),
  HH(ProgArgs, "Arguments"),
  HH(CompileMenu, "Compile menu"),
  H(Compile),
  H(Make),
  H(Link),
  HH(Build, "Build all"),
  H(Debug),
  HH(ToggleBreak, "Set/Reset Breakpoint"),
  H(Evaluate),
  HH(AddWatchEntry, "Watch an expression"),
  HH(BreakPoints, "Breakpoints"),
  HH(DisWindow, "Disassembler window"),
  HH(FunctionList, "List of Functions"),
  HH(CallStack, "Call stack"),
  HH(ProjectMenu, "PROJECT"),
  HH(OpenProject, "Open project"),
  HH(CloseProject, "Close project"),
  HH(AddProjectItem, "Add item"),
  HH(DelProjectItem, "Delete item"),
  HH(LocalOptions, "Local options"),
  HH(ShowIncludes, "Includes"),
  HH(TargetName, "Main targetname"),
  HH(PrimaryFile, "Primary file"),
  HH(ClearDependencies, "Clear dependencies"),
  HH(MakeClear, "Delete rebuildable files"),
  HH(CreateDependencies, "Create dependencies"),
  HH(WriteMake, "Write Makefile"),
  HH(OptionsMenu, "Options"),
  H(Directories),
  HH(IncludeDir, "Include directories"),
  HH(LibDir, "Library directories"),
  HH(ObjDir, "Object directories"),
  HH(SrcDir, "Sources directories"),
  HH(StandardIncludeDir, "Standard headers"),
  HH(C_CXXCompiler, "C/C++-Compiler"),
  HH(WarningFlags, "Warnings"),
  HH(OptimizationFlags, "Optimizations"),
  HH(DebugFlags, "Debugging"),
  HH(CFlags, "C options"),
  HH(CXXFlags, "CXX options"),
  HH(PascalFlags, "Pascal options"),
  HH(FortranFlags, "Fortran options"),
  HH(AdaFlags, "ADA options"),
  H(Libraries),
  HH(LinkerOptions, "Linker options"),
  HH(CompilerOptions, "Compiler options"),
  H(Environment),
  H(Colors),
  HH(SetGlobalOptions, "Editor options"),
  H(Preferences),
  HH(EditReserved, "Reserved words"),
  HH(EditGPCReserved, "GNU Pascal reserved words"),
  HH(EditCFlags, "C-Flags"),
  HH(EditCXXFlags, "CXX-Flags"),
  HH(EditPascalFlags, "Pascal-Flags"),
  HH(EditFortranFlags, "Fortran-Flags"),
  HH(EditAdaFlags, "Ada-Flags"),
  HH(EditWarnFlags, "Warning-Flags"),
  HH(EditDebugFlags, "Debugging-Flags"),
  HH(EditOptFlags, "Optimization-Flags"),
  HH(EditUserWords, "User words"),
  HH(MouseDlg, "Mouse options"),
  H(Windows),
  HH(Resize, "Size/move"),
  H(Zoom),
  H(Tile),
  H(Cascade),
  H(Next),
  HH(Prev, "Previous"),
  H(Close),
  HH(ShowWindowList, "List"),
  HH(ShowProject, "Project"),
  H(UserScreen),
  HH(ShowWatchWindow, "Watches"),
  HH(HelpMenu, "Help submenu"),
  H(Help),
  HH(HelpIndex, "RHIDE Help index"),
  HH(Syntax, "Syntax help submenu"),
  HH(SyntaxHelp, "Syntax help"),
  HH(SyntaxFiles, "Files to search"),
  HH(SyntaxOptions, "Options for searching"),
  HH(SyntaxIndex, "Index for syntax help"),
  HH(LibcHelp, "libc reference"),
  HH(HelpHelp, "Help about help"),
  HH(LastHelp, "Last help"),
  HH(IDEFileEditor, "How the editor works"),
  HH(WindowList, "List"),
  HH(OptFlagList, "Optimizations"),
  HH(DebFlagList, "Debugging"),
  HH(WarnFlagList, "Warnings"),
  HH(CFlagList, "C options"),
  HH(CXXFlagList, "CXX Options"),
  HH(MsgListBox, "Message window"),
  HH(ProjectListBox, "Project window"),
#if 0
  HH(PreferenceCheckbox + 4, "autoindent"),
  HH(PreferenceCheckbox + 5, "overwrite mode"),
  HH(PreferenceCheckbox + 11, "C intelligent indenting"),
  HH(PreferenceCheckbox + 12, "Use real tabs"),
  HH(PreferenceCheckbox + 13, "No persistent blocks"),
  HH(PreferenceCheckbox + 14, "Column cursor"),
  HH(PreferenceCheckbox + 15, "Row cursor"),
#endif
  HH(PreferenceCheckbox + 0, "all dependencies in makefile"),
  HH(PreferenceCheckbox + 1, "create backupfiles"),
  HH(PreferenceCheckbox + 2, "syntax highlighting"),
  HH(PreferenceCheckbox + 3, "Use dual display"),
  HH(PreferenceCheckbox + 4, "redirect stderr"),
  HH(PreferenceCheckbox + 5, "redirect stdout"),
  HH(PreferenceCheckbox + 6, "show process information"),
  HH(PreferenceCheckbox + 7, "show free memory"),
  HH(PreferenceCheckbox + 8, "No file caching"),
  HH(PreferenceCheckbox + 9, "16 background colors"),
  HH(PreferenceCheckbox + 10, "Show GDB commands"),
  HH(PreferenceCheckbox + 11, "Use no shadows"),
  HH(PreferenceCheckbox + 12, "Save text palette"),
  HH(PreferenceCheckbox + 13, "Save project only when closing"),
  HH(PreferenceCheckbox + 14, "Show user screen after exit"),
  HH(PreferenceCheckbox + 15, "Only #include \"...\" in dependencies"),
  HH(PreferenceCheckbox + 16, "Directories in project items"),
  HH(PreferenceCheckbox + 17, "Show disassembler window when needed"),
  HH(PreferenceCheckbox + 18, "Use RCS"),
  HH(PreferenceCheckbox + 18, "Use FPC pascal compiler for .pas files"),
#if 0
  HH(PreferenceTabsize, "Tabsize"),
#endif
  HH(PreferenceVideomode + 0, "Screen mode"),
  HH(PreferenceVideomode + 1, "Screen mode"),
  HH(PreferenceVideomode + 2, "Screen mode"),
  HH(PreferenceVideomode + 3, "Screen mode"),
  HH(PreferenceVideomode + 4, "Screen mode"),
  HH(PreferenceVideomode + 5, "Screen mode"),
  HH(PreferenceVideomode + 6, "Screen mode"),
  HH(PreferenceVideomode + 7, "Screen mode"),
  HH(PreferenceVideomode + 8, "Screen mode"),
  HH(PreferenceVideomode + 9, "Screen mode"),
  HH(PreferenceVideomode + 10, "Screen mode"),
  HH(PreferenceVideomode + 11, "Screen mode"),
  HH(PreferenceVideomode + 12, "Screen mode"),

  HH(LocalCommandline, "Local commandline options"),
  HH(LocalOutputname, "Name of the output file"),
  HH(LocalCompilertype, "Compiler type"),
  HH(LocalCompilertype + 1, "Compiler type"),
  HH(LocalCompilertype + 2, "Compiler type"),
  HH(LocalCompilertype + 3, "Compiler type"),
  HH(LocalCompilertype + 4, "Compiler type"),
  HH(LocalCompilertype + 5, "Compiler type"),
  HH(LocalCompilertype + 6, "Compiler type"),
  HH(LocalCompilertype + 7, "Compiler type"),
  HH(LocalCompilertype + 8, "Compiler type"),
  HH(LocalCompilertype + 9, "Compiler type"),
  HH(LocalCompilertype + 10, "Compiler type"),
  HH(LocalCompilertype + 11, "Compiler type"),
  HH(LocalCompilertype + 12, "Compiler type"),
  HH(LocalCompilertype + 13, "Compiler type"),
  HH(LocalCompilertype + 14, "Compiler type"),
  HH(LocalCompilertype + 15, "Compiler type"),
  HH(LocalCompiler, "User defined compiler"),
  HH(LocalErrortype, "Error checking"),
  HH(LocalErrortype + 1, "Error checking"),
  HH(LocalErrortype + 2, "Error checking"),
  HH(LocalErrortype + 3, "Error checking"),
  HH(LocalErrortype + 4, "Error checking"),
  HH(LocalErrortype + 5, "Error checking"),
  HH(LocalErrortype + 6, "Error checking"),
  HH(LocalErrortype + 7, "Error checking"),
  HH(LocalErrortype + 8, "Error checking"),
  HH(LocalErrortype + 9, "Error checking"),
  HH(LocalErrortype + 10, "Error checking"),
  HH(LocalErrortype + 11, "Error checking"),
  HH(LocalErrortype + 12, "Error checking"),
  HH(LocalErrortype + 13, "Error checking"),
  HH(LocalErrortype + 14, "Error checking"),
  HH(LocalErrortype + 15, "Error checking"),
  HH(LocalErrorprogram, "Error program"),
  HH(LocalOtheroptions + 0, "exclude from link"),
  HH(LocalOtheroptions + 1, "rebuild seldom"),
  HH(LocalOtheroptions + 2, "Local options"),
  HH(LocalOtheroptions + 3, "Local options"),
  HH(LocalOtheroptions + 4, "Local options"),
  HH(LocalOtheroptions + 5, "Local options"),
  HH(LocalOtheroptions + 6, "Local options"),
  HH(LocalOtheroptions + 7, "Local options"),
  HH(LocalOtheroptions + 8, "Local options"),
  HH(LocalOtheroptions + 9, "Local options"),
  HH(LocalOtheroptions + 10, "Local options"),
  HH(LocalOtheroptions + 11, "Local options"),
  HH(LocalOtheroptions + 12, "Local options"),
  HH(LocalOtheroptions + 13, "Local options"),
  HH(LocalOtheroptions + 14, "Local options"),
  HH(LocalOtheroptions + 15, "Local options"),

  {hcCalculatorDialog, {"setedit", "Calculator"}},

  HH(ClosedWindows, "Closed windows"),
  {0, {"rhide", ""}}
};

#undef H

static const InfoHelpRec *
HelpstringFromCtx(ushort ctx)
{
  int
    i = 0;

  while (known_contexts[i].ctx)
  {
    if (known_contexts[i].ctx == ctx)
      return &(known_contexts[i].help);
    i++;
  }
  return &(known_contexts[i].help);
}

void
Help(ushort ctx)
{
  const InfoHelpRec *
    help_rec;

  help_rec = HelpstringFromCtx(ctx);
  SyntaxHelp(help_rec->help_string, help_rec->help_file);
}


void
HelpIndex()
{
  SyntaxHelp("Index", "rhide");
}

static void
CheckHelpWindow(int open_closed)
{
  int
    closed;

  InfWindow =
    (TWindow *) message(TProgram::application, evBroadcast, cmInfoAnswer,
                        NULL);
  if (InfWindow)
    return;
  if (!open_closed)
    return;
  if (last_title && (closed = IsClosedWindow(last_title)) != -1)
    InfWindow = OpenClosedWindow(closed);
  return;
}

void
LastHelp()
{
  CheckHelpWindow(0);
  if (InfWindow)
  {
    TEvent
      event;

    event.what = evCommand;
    event.message.command = cmInfBack;
    InfWindow->select();
    InfWindow->handleEvent(event);
    return;
  }
  CheckHelpWindow(1);
  if (InfWindow)
    return;
  SyntaxHelp("Top", "rhide");
}

void
SyntaxHelp(const char *word, const char *fname)
{
  if (!fname)
  {
    syntaxhelp(word);
    return;
  }
  int
    fromModal = 0;
  TView *
    v = TProgram::deskTop->current;

  if (v)
    fromModal = v->state & sfModal;
  if (fromModal)
  {
    TInfFile *
      i = new TInfFile((char *) fname);
    TWindow *
      w = NULL;

    // Check for fails
    if (i)
    {
      if (i->Status == False)
        w = new TIDEInfWindow(i, (char *) word);
      else
        delete(i);
    }
    if (!w)
      return;
    TProgram::deskTop->execView(w);
    destroy(w);
    return;
  }
  CheckHelpWindow(1);
  if (InfWindow)
  {
    char *
      tmp;

    string_dup(tmp, "(");
    string_cat(tmp, fname);
    string_cat(tmp, ")");
    string_cat(tmp, word);
    IW->viewer->switchToTopic(tmp);
    InfWindow->select();
    string_free(tmp);
    return;
  }
  {
    TInfFile *
      i = new TInfFile((char *) fname);

    // Check for fails
    if (i)
    {
      if (i->Status == False)
        InfWindow = new TIDEInfWindow(i, (char *) word);
      else
        delete(i);
    }
    if (!InfWindow)
      return;
    AddWindow(InfWindow, NULL, False, True, False);
  }
}


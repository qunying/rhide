/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#define Uses_IDEConst
#include "rhide.h"

#define Uses_ideCommands
#include <libide.h>

#define h(ctx,hint) case hc##ctx: return hint;

const char *
IDEHint(ushort ctx)
{
  switch (ctx)
  {
// Sytem
      h(About,
        _("show information about RHIDE"))
        h(BugReportSmall,
          _("Creates a small bug report"))
        h(BugReport,
          _("Creates a sample bug report"))
        h(FSDB,
          _("Start the FSDB debugger"))
        h(GREP,
          _("Running GREP (arguments are prompted)"))
        h(GDB,
          _("Start the GDB debugger"))
        h(Repaint,
          _("repaints the desktop"))
        h(ShowCalculator,
          _("Powerful builtin calculator"))
        h(Puzzle,
          _("A nice puzzle game"))
        h(Calendar,
          _("A simple calender"))
        h(Ascii, _("A table with all the ASCII characters"))
// File
        h(File,
          _("Menu entries for handling with files"))
        h(OpenEditor,
          _("opens a file in the editor"))
        h(NewEditor,
          _("create a new editor window"))
        h(SaveEditor,
          _("save the current editor to disk (if modified)"))
        h(SaveAs,
          _("save the current editor under a different name to disk"))
        h(SaveAll,
          _("save all opened editors to disk (if modified)"))
        h(PrintSetup,
          _("konfigure your print settings"))
        h(Print, _("print the current editor file"))
#ifdef __DJGPP__
        h(DosShell, _("switch to DOS (call of %COMSPEC%, return with EXIT)"))
#else
        h(DosShell,
          _
          ("switch to a shell (call of $SHELL or /bin/sh, return with exit)"))
#endif
        h(QUIT, _("quits the IDE"))
// Edit
        h(Edit,
          _("Submenu for things dealing with the editor"))
        h(Undo,
          _("undoes the last edit command"))
        h(Redo,
          _("undoes the last undo command"))
        h(Cut,
          _("move the selected text to the clipboard"))
        h(Copy,
          _("copy the selected text to the clipboard"))
        h(Paste,
          _("insert the selected text from the clipboard"))
        h(ShowClip,
          _("shows the clipboard"))
        h(Clear,
          _("delete the selected text "))
        h(Rectangle,
          _("Submenu for handling with rectangular blocks"))
        h(SelRectStart,
          _("Start selecting rectangular block"))
        h(SelRectEnd,
          _("End selecting rectangular block"))
        h(SelRectHide,
          _("Hide selected rectangular block"))
        h(SelRectCopy,
          _("Copy selected rectangular block"))
        h(SelRectPaste,
          _("Paste selected rectangular block"))
        h(SelRectCut,
          _("Cut selected rectangular block"))
        h(SelRectDel,
          _("Clear selected rectangular block"))
        h(SelRectMove,
          _("Move selected rectangular block"))
        h(CopyClipWin,
          _("Copy the selected text to the Windows clipboard"))
        h(PasteClipWin,
          _("Paste text from the Windows clipboard"))
        h(ExpandAllTabs,
          _("Expand all tabs to spaces"))
        h(CompactBuffer, _("Replace as much as possible spaces with tabs"))
// Edit/Macro
        h(Macro,
          _("Submenu for handling with macros"))
        h(Record,
          _("Start the recording for a macro"))
        h(Stop,
          _("Stop the recording for a macro"))
        h(Play,
          _("Run the recorded macro"))
        h(ChooseMacro,
          _("choose a named macro"))
        h(RepeatMacro,
          _("repeat the last macro"))
        h(GenCodeForMacro,
          _("write code for recorded macro"))
        h(ProfileEditor, _("Check the speed of the editor"))
// Search
        h(Search,
          _("Submenu for searching/replacing text"))
        h(Find,
          _("find a string in the current editor"))
        h(Replace,
          _("find and replace a string in the current editor"))
        h(SearchAgain,
          _("repeat the last find- or replace-operation"))
        h(GotoLine,
          _("go to line number (after prompting)"))
        h(JumpToFunction,
          _("Select a function from the current editor"))
        h(NextMessage,
          _("select next message"))
        h(PrevMessage, _("select previous message"))
// Run
        h(RunMenu,
          _("Submenu for running your program"))
        h(Run,
          _("run the project-target (if executable after doing a make)"))
        h(Step,
          _("Step over function calls"))
        h(StepNo,
          _("Step over function calls (No display swap)"))
        h(Trace,
          _("Execute one instruction"))
        h(TraceNo,
          _("Execute one instruction (No display swap)"))
        h(Goto,
          _("Stops execution at the cursor"))
        h(GotoNo,
          _("Stops execution at the cursor (No display swap)"))
        h(Reset,
          _("Reset the debugged program"))
        h(Finish,
          _("Execute until end of function"))
        h(MainFunction,
          _("Enter the name of the 'main'-function"))
        h(ProgArgs, _("enter commandline arguments for your program"))
// Compile
        h(CompileMenu,
          _("Submenu for compiling and/or creating your program/library"))
        h(Compile,
          _("compile the current editor or the selected project-item"))
        h(Make,
          _("make the project (with checking auto-dependencies)"))
        h(Link,
          _("link the project or create the library"))
        h(Build, _("build the project completely new"))
// Debug
        h(Debug,
          _("Submenu for debugging your program"))
        h(ToggleBreak,
          _("Set/Clear a breakpoint at the current line"))
        h(Evaluate,
          _("Evaluate or modify an expression"))
        h(AddWatchEntry,
          _("Add an expression to the watching list"))
        h(Inspect,
          _("Open an inspector window for an expression"))
        h(BreakPoints,
          _("Show/Modify the breakpoints"))
        h(DisWindow,
          _("Show a disassembler window at the current execution point"))
        h(CallStack,
          _("Shows a window with the function call stack"))
        h(FunctionList,
          _("Show a list with all functions"))
        h(AddDataWindow,
          _("Open a window with a memory dump"))
        h(ShowStackWindow, _("Open a window showing the dump of the stack"))
// Project
        h(ProjectMenu,
          _("Submenu for handling your projects"))
        h(OpenProject,
          _("open a project"))
        h(CloseProject,
          _("close the current project"))
        h(AddProjectItem,
          _("add an item to the project"))
        h(DelProjectItem,
          _("remove the selected item from the project"))
        h(LocalOptions,
          _("enter local commandline options for compiling this item"))
        h(ShowIncludes,
          _("show the dependencies for the selected item"))
        h(TargetName,
          _("change the name of the project-target"))
        h(PrimaryFile,
          _("Set the primary file (only for GPC with '--automake'!!!)"))
        h(ClearDependencies,
          _("remove the list of automaticaly generated dependencies"))
        h(MakeClear,
          _("remove all files, which can be rebuild with this project"))
        h(CreateDependencies,
          _("Create for all project items the dependencies"))
        h(WriteMake, _("write a makefile for this project"))
// Options
        h(OptionsMenu, _("Submenu for most of the options of RHIDE"))
// Options/Directories
        h(Directories,
          _("set various search paths for finding the files"))
        h(IncludeDir,
          _("set where to search for include files"))
        h(LibDir,
          _("set where to search for libraries"))
        h(ObjDir,
          _("set where to search for object files"))
        h(SrcDir,
          _("set where to search for source files"))
        h(StandardIncludeDir, _("Define, where your standard headers are"))
// Options/C/C++-compiler
        h(C_CXXCompiler,
          _("customize various options for the C/C++-compiler"))
        h(WarningFlags,
          _("customize the generating of warnings"))
        h(OptimizationFlags,
          _("customize, how to optimize the code"))
        h(DebugFlags,
          _("customize the generating of debugging information"))
        h(CFlags,
          _("customize C-compiler specific flags"))
        h(CXXFlags,
          _("customize C++-compiler specific flags"))
        h(PascalFlags,
          _("Flags only when compiling Pascal files"))
        h(FortranFlags,
          _("Flags only when compiling Fortran files"))
        h(AdaFlags,
          _("Flags only when compiling ADA files"))
        h(Libraries,
          _("set which libraries to be linked with the program"))
        h(LinkerOptions,
          _("set additional flags to the linker"))
        h(CompilerOptions,
          _("set additional flags to the compiler (defines, ...)"))
// Options/Environment
        h(Environment,
          _("customize the IDE"))
        h(Colors, _("customize the colors of the IDE"))
// Options/Environment/Editor
        h(Editor,
          _("various options relating to the editor"))
        h(SetGlobalOptions,
          _("Set the editor options"))
        h(EditKeyBind,
          _("Customize your key bindings"))
        h(SetUpAltKeys,
          _("Define the meaning of the Alt-keys"))
        h(KbBackDefault,
          _("Restore default key bindings"))
        h(Preferences,
          _("set basic options for the IDE"))
        h(MouseDlg,
          _("Set some options for the mouse"))
        h(EditReserved,
          _("edit the list of reserved words"))
        h(EditGPCReserved,
          _("Edit the list of reserved words for Pascal files"))
        h(EditCFlags,
          _("edit the list of flags for the C-compiler"))
        h(EditCXXFlags,
          _("edit the list of flags for the C++-compiler"))
        h(EditPascalFlags,
          _("Edit the list of pascal options"))
        h(EditFortranFlags,
          _("Edit the list of fortran options"))
        h(EditAdaFlags,
          _("Edit the list of ADA options"))
        h(EditWarnFlags,
          _("edit the list of flags for warnings"))
        h(EditDebugFlags,
          _("edit the list of flags for debugging"))
        h(EditOptFlags,
          _("edit the list of optimization flags"))
        h(EditUserWords,
          _("edit the list of user defined words (for syntaxhighlight)"))
        h(SaveOptions,
          _("Save all the options to a project file"))
        h(LoadOptions, _("Load the options from a project file"))
// Windows
        h(Windows,
          _("Submenu for dealing with windows"))
        h(Resize,
          _("resize/move the current window"))
        h(Zoom,
          _
          ("toggles between full size and previous size of the current window"))
        h(Tile, _("show all editor windows simultaniously")) h(Cascade,
                                                               _
                                                               ("show from each editor the title"))
        h(Next, _("switch to the next window on the desktop")) h(Prev,
                                                                 _
                                                                 ("switch to the previous window on the desktop"))
        h(Close, _("close the current window")) h(ShowWindowList,
                                                  _
                                                  ("show a list of all windows on the desktop"))
        h(ShowProject, _("show the project window")) h(ShowWatchWindow,
                                                       _
                                                       ("Opens the window with watches"))
        h(ShowMessages, _("show the message window")) h(UserScreen,
                                                        _
                                                        ("show the user screen"))
// Help
        h(HelpMenu,
          _("Several menu entries for getting help"))
        h(Help,
          _("opens the Info Viewer"))
        h(HelpIndex, _("The main index of the RHIDE help"))
// Help/Syntax help
        h(Syntax,
          _("Submenu for syntax help"))
        h(SyntaxHelp,
          _("Help for the function under the cursor"))
        h(SyntaxFiles,
          _("Files to be searched for syntax help"))
        h(SyntaxOptions,
          _("Options for the syntax help"))
        h(SyntaxIndex,
          _("Get a list with all available syntax help topics"))
        h(LibcHelp,
          _("The main index of the libc reference"))
        h(HelpHelp,
          _("Get help about using the helpsystem"))
        h(LastHelp,
          _("Shows the last opened help"))
    h(ClosedWindows, _("How many closed windows should I remember")) default:
      return "";
  }
}

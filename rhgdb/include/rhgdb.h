/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#ifndef __RHGDB_H__
#define __RHGDB_H__

const ushort
  cmStep = 300,
  cmTrace = 301,
  cmRun = 302,
  cmReset = 303,
  cmGoto = 304,
  cmOpen = 305,
  cmQUIT = 306,
  cmUserScreen = 307,
  cmWindowList = 308,
  cmToggleBreak = 309,
  cmEvaluate = 310,
  cmAddWatchEntry = 311,
  cmGotoFunction = 312,
  cmBreakPoints = 313,
  cmSaveOptions = 314,
  cmLoadOptions = 315,
  cmSrcDirs = 316,
  cmFunctionList = 317,
  cmPreferences = 318,
  cmViewerAnswer = 319,
  cmGotoLine = 320,
  cmProgArgs = 321,
  cmCallStack = 322,
  cmJumpFunction = 323,
  cmStepNo = 324,
  cmTraceNo = 325,
  cmGotoNo = 326,
  cmAbout = 327,
  cmMainFunction = 328,
  cmOpenEditor = 328,
  cmEditorAnswer = 329,
  cmDisWindow = 330,
  cmShowWatchWindow = 331,
  cmDataWindow = 332, cmStackWindow = 333, cmInspectData = 334;

extern char *progname;
extern char **files_on_desktop;
extern int files_on_desktop_count;
extern int tabsize;
extern int use_dual_display;
class TDirList;
extern TDirList *src_dirs;
class TDisassemblerWindow;
extern TDisassemblerWindow *dis_win;
class TDataWindow;

void STEP(int _switch_to_user = 1);
void TRACE(int _switch_to_user = 1);
void RUN();
void RESET();
void GOTO(int _switch_to_user = 1);

Boolean OpenViewer(char *fname, int line, Boolean from_debugger,
                   Boolean only_focus = False);
void CenterCursor();
char *WhereIsCursor(int &line, int &column, char *&bname);
void ToggleBreak();
void DeleteAllBreakpoints();
void SaveOptions();
void LoadOptions();
void FileOpen();
void SourceDirectories();
void repaint();
void Preferences();
char *RHGDBWordUnderCursor(void);
void init_syntax();
void ClearCPULine(void);

#endif

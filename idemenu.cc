/* Copyright (C) 1996-2000 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#include <string.h>
#define Uses_TRect
#define Uses_TKeys
#define Uses_TKeys_Extended
#define Uses_IDEConst
#define Uses_TMenuBar
#define Uses_TSubMenu
#define Uses_editCommands
#define Uses_TApplication
#define Uses_TDialog
#define Uses_TSortedListBox
#define Uses_TStringCollection
#define Uses_TScroller
#define Uses_TNoCaseStringCollection
#define Uses_ideCommands
#define Uses_SOStack
#define Uses_TNoCaseSOSStringCollection
#include <libide.h>
#include <settvuti.h>

#include "rhide.h"
#include "ideapp.h"
#include <inf.h>

#define Uses_TCEditor_Commands
#define Uses_TCEditor_External
#include <ceditor.h>

#define kbNoKey (ushort)0

// This is a little trick to reduce the size

class _TMenuItem:public TMenuItem
{
public:
  _TMenuItem(const char *aName,
             ushort aCommand,
             ushort aKeyCode,
             ushort aHelpCtx = hcNoContext,
             char *p = 0, TMenuItem * aNext = 0);
    _TMenuItem(const char *aName,
               ushort aKeyCode,
               TMenu * aSubMenu,
               ushort aHelpCtx = hcNoContext, TMenuItem * aNext = 0);

};

_TMenuItem::_TMenuItem(const char *aName,
                       ushort aCommand,
                       ushort aKeyCode,
                       ushort aHelpCtx, char *p, TMenuItem * aNext):
TMenuItem(_(aName), aCommand, aKeyCode, aHelpCtx, p, aNext)
{
}

_TMenuItem::_TMenuItem(const char *aName,
                       ushort aKeyCode,
                       TMenu * aSubMenu, ushort aHelpCtx, TMenuItem * aNext):
TMenuItem(_(aName), aKeyCode, aSubMenu, aHelpCtx, aNext)
{
}

TMenuBar *
IDE::initMenuBar(TRect r)
{
  TMenuBar *bar;

  r = TProgram::application->getExtent();
  r.b.y = r.a.y + 1;

  TMenuItem & syntax_sub = *new TSubMenu(_("~S~yntax help"), kbAlS, hcSyntax)
    + *new _TMenuItem(__("~S~yntax help"), cmSyntaxHelp, kbCtF1, hcSyntaxHelp,
                      "Ctrl+F1") + *new _TMenuItem(__("~F~iles to search"),
                                                   cmSyntaxFiles, kbNoKey,
                                                   hcSyntaxFiles) +
    *new _TMenuItem(__("~O~ptions for searching"), cmSyntaxOptions, kbNoKey,
                    hcSyntaxOptions);

  TSubMenu & help_menu = *new TSubMenu(_("~H~elp"), kbAlH, hcHelpMenu)
    + *new _TMenuItem(__("~H~elp"), cmHelp, kbF1, hcHelp, "F1")
    + *new _TMenuItem(__("RHIDE Help ~i~ndex"), cmHelpIndex, kbShF1,
                      hcHelpIndex,
                      "Shift+F1") + syntax_sub +
    *new _TMenuItem(__("Index for s~y~ntax help"), cmSyntaxIndex, kbNoKey,
                    hcSyntaxIndex) + *new _TMenuItem(__("lib~c~ reference"),
                                                     cmLibcHelp, kbNoKey,
                                                     hcLibcHelp) +
    *new _TMenuItem(__("Help ~a~bout help"), cmHelpHelp, kbNoKey,
                    hcHelpHelp) + *new _TMenuItem(__("~L~ast help"),
                                                  cmLastHelp, kbAlF1,
                                                  hcLastHelp, "Alt+F1");

  TSubMenu & file_menu = *new TSubMenu(_("~F~ile"), kbAlF, hcFile)
    + *new _TMenuItem(__("~O~pen"), cmOpenEditor, kbF3, hcOpenEditor, "F3")
    + *new _TMenuItem(__("~N~ew"), cmNewEditor, kbNoKey, hcNewEditor)
    + *new _TMenuItem(__("~S~ave"), cmSaveEditor, kbF2, hcSaveEditor, "F2")
    + *new _TMenuItem(__("S~a~ve as..."), cmcSaveAs, kbNoKey, hcSaveAs)
    + *new _TMenuItem(__("Save a~l~l"), cmSaveAll, kbNoKey, hcSaveAll)
    + newLine()
    + *new _TMenuItem(__("~P~rint"), cmPrint, kbNoKey, hcPrint)
    + *new _TMenuItem(__("Printer set~u~p"), cmPrintSetup, kbNoKey,
                      hcPrintSetup) + newLine()
#ifdef __DJGPP__
    + *new _TMenuItem(__("~D~OS shell"), cmDosShell, kbNoKey, hcDosShell)
#else
    + *new _TMenuItem(__("S~h~ell"), cmDosShell, kbNoKey, hcDosShell)
#endif
    + *new _TMenuItem(__("E~x~it"), cmQUIT, kbAlX, hcQUIT, "Alt+X");

  TMenuItem & macro_sub = *new TSubMenu(_("~M~acro"), kbAlM, hcMacro)
    + *new _TMenuItem(__("~R~ecord"), cmcRecordMacro, kbShF10, hcRecord,
                      "Shift+F10") + *new _TMenuItem(__("~S~top"),
                                                     cmcStopMacro, kbAlF10,
                                                     hcStop,
                                                     "Alt+F10") +
    *new _TMenuItem(__("~P~lay"), cmcPlayMacro, kbCtF10, hcPlay,
                    "Ctrl+F10") + newLine() + *new _TMenuItem(__("~C~hoose"),
                                                              cmcChooseMacro,
                                                              kbNoKey,
                                                              hcChooseMacro) +
    *new _TMenuItem(__("R~e~peat"), cmcRepeatMacro, kbShF3, hcRepeatMacro,
                    "Shift+F3") + *new _TMenuItem(__("~G~enerate Code"),
                                                  cmcGenCodeForMacro, kbNoKey,
                                                  hcGenCodeForMacro);

  TMenuItem & rectangle_sub =
    *new TSubMenu(_("Recta~n~gle"), kbAlN,
                  hcRectangle) + *new _TMenuItem(__("~S~tart"),
                                                 cmcSelRectStart, kbNoKey,
                                                 hcSelRectStart,
                                                 "Ctrl-K-Shift-B") +
    *new _TMenuItem(__("~E~nd"), cmcSelRectEnd, kbNoKey, hcSelRectEnd,
                    "Ctrl-K-Shift-K") + *new _TMenuItem(__("~H~ide"),
                                                        cmcSelRectHide,
                                                        kbNoKey,
                                                        hcSelRectHide,
                                                        "Ctrl-K-Shift-H") +
    newLine() + *new _TMenuItem(__("~C~opy"), cmcSelRectCopy, kbNoKey,
                                hcSelRectCopy,
                                "Ctrl-K-Shift-C") +
    *new _TMenuItem(__("~P~aste"), cmcSelRectPaste, kbNoKey, hcSelRectPaste,
                    "Ctrl-K-Shift-P") + *new _TMenuItem(__("Cu~t~"),
                                                        cmcSelRectCut,
                                                        kbNoKey, hcSelRectCut,
                                                        "Ctrl-K-Shift-T") +
    *new _TMenuItem(__("C~l~ear"), cmcSelRectDel, kbNoKey, hcSelRectDel,
                    "Ctrl-K-Shift-L") + *new _TMenuItem(__("~M~ove"),
                                                        cmcSelRectMove,
                                                        kbNoKey,
                                                        hcSelRectMove,
                                                        "Ctrl-K-Shift-M");

  TSubMenu & edit_menu = *new TSubMenu(_("~E~dit"), kbAlE, hcEdit)
    + *new _TMenuItem(__("~U~ndo"), cmcUndo, kbAlBackSpace, hcUndo,
                      "Alt+Backspace") + *new _TMenuItem(__("~R~edo"),
                                                         cmcRedo, kbNoKey,
                                                         hcRedo) + newLine() +
    *new _TMenuItem(__("Cu~t~"), cmcCut, kbShDelete, hcCut,
                    "Shift+Del") + *new _TMenuItem(__("~C~opy"), cmcCopy,
                                                   kbCtInsert, hcCopy,
                                                   "Ctrl+Ins") +
    *new _TMenuItem(__("~P~aste"), cmcPaste, kbShInsert, hcPaste,
                    "Shift+Ins") + *new _TMenuItem(__("~S~how clipboard"),
                                                   cmShowClip, kbNoKey,
                                                   hcShowClip) + newLine() +
    *new _TMenuItem(__("C~l~ear"), cmcClear, kbCtDelete, hcClear,
                    "Ctrl+Del") + newLine() + rectangle_sub + newLine()
#ifdef __DJGPP__
    + *new _TMenuItem(__("Cop~y~ to Windows Clipboard"), cmcCopyClipWin,
                      kbNoKey,
                      hcCopyClipWin) +
    *new _TMenuItem(__("Paste from ~W~indows Clipboard"), cmcPasteClipWin,
                    kbNoKey, hcPasteClipWin) + newLine()
#endif
    + *new _TMenuItem(__("E~x~pand all tabs"), cmcExpandAllTabs, kbNoKey,
                      hcExpandAllTabs) + *new _TMenuItem(__("C~o~mpact text"),
                                                         cmcCompactBuffer,
                                                         kbNoKey,
                                                         hcCompactBuffer) +
    newLine() + macro_sub + newLine() + *new _TMenuItem(__("Profile Editor"),
                                                        cmcProfileEditor,
                                                        kbNoKey,
                                                        hcProfileEditor);

  TSubMenu & search_menu = *new TSubMenu(_("~S~earch"), kbAlS, hcSearch)
    + *new _TMenuItem(__("~F~ind..."), cmcFind, kbNoKey, hcFind)
    + *new _TMenuItem(__("~R~eplace..."), cmcReplace, kbNoKey, hcReplace)
    + *new _TMenuItem(__("~S~earch again"), cmcSearchAgain, kbNoKey,
                      hcSearchAgain,
                      "Ctrl+L") + *new _TMenuItem(__("~G~oto Line..."),
                                                  cmGotoLine, kbCtJ,
                                                  hcGotoLine,
                                                  "Ctrl+J") +
    *new _TMenuItem(__("~J~ump to function"), cmJumpToFunction, kbAlF2,
                    hcJumpToFunction,
                    "Alt+F2") + newLine() +
    *new _TMenuItem(__("~N~ext message"), cmNextMessage, kbAlF8,
                    hcNextMessage,
                    "Alt+F8") + *new _TMenuItem(__("Previ~o~us message"),
                                                cmPrevMessage, kbAlF7,
                                                hcPrevMessage, "Alt+F7");

  TSubMenu & run_menu = *new TSubMenu(_("~R~un"), kbAlR, hcRunMenu)
    + *new _TMenuItem(__("~R~un"), cmRun, kbCtF9, hcRun, "Ctrl+F9")
#ifdef INTERNAL_DEBUGGER
    + *new _TMenuItem(__("~S~tep over"), cmStep, kbF8, hcStep, "F8")
    + *new _TMenuItem(__("Step over (No display swap)"), cmStepNo, kbShF8,
                      hcStepNo,
                      "Shift+F8") + *new _TMenuItem(__("~T~race into"),
                                                    cmTrace, kbF7, hcTrace,
                                                    "F7") +
    *new _TMenuItem(__("Trace into (No display swap)"), cmTraceNo, kbShF7,
                    hcTraceNo,
                    "Shift+F7") + *new _TMenuItem(__("~G~o to cursor"),
                                                  cmGoto, kbF4, hcGoto,
                                                  "F4") +
    *new _TMenuItem(__("Go to cursor (No display swap)"), cmGotoNo, kbShF4,
                    hcGotoNo,
                    "Shift+F4") + *new _TMenuItem(__("~U~ntil return"),
                                                  cmFinish, kbNoKey,
                                                  hcFinish) +
    *new _TMenuItem(__("~P~rogram reset"), cmReset, kbCtF2, hcReset,
                    "Ctrl+F2") + newLine() +
    *new _TMenuItem(__("~M~ain function"), cmMainFunction, kbNoKey,
                    hcMainFunction)
#endif
    + *new _TMenuItem(__("~A~rguments ..."), cmProgArgs, kbNoKey, hcProgArgs);

  TSubMenu & compile_menu =
    *new TSubMenu(_("~C~ompile"), kbAlC,
                  hcCompileMenu) + *new _TMenuItem(__("~C~ompile"), cmCompile,
                                                   kbAlF9, hcCompile,
                                                   "Alt+F9") +
    *new _TMenuItem(__("~M~ake"), cmMake, kbF9, hcMake,
                    "F9") + *new _TMenuItem(__("~L~ink"), cmLink, kbNoKey,
                                            hcLink) +
    *new _TMenuItem(__("~B~uild all"), cmBuild, kbNoKey, hcBuild);

  TSubMenu & project_menu =
    *new TSubMenu(_("~P~roject"), kbAlP,
                  hcProjectMenu) + *new _TMenuItem(__("Open ~p~roject"),
                                                   cmOpenProject, kbNoKey,
                                                   hcOpenProject) +
    *new _TMenuItem(__("~C~lose project"), cmCloseProject, kbNoKey,
                    hcCloseProject) + *new _TMenuItem(__("~A~dd item"),
                                                      cmAddProjectItem,
                                                      kbNoKey,
                                                      hcAddProjectItem) +
    *new _TMenuItem(__("D~e~lete item"), cmDelProjectItem, kbNoKey,
                    hcDelProjectItem) + *new _TMenuItem(__("~L~ocal options"),
                                                        cmLocalOptions,
                                                        kbNoKey,
                                                        hcLocalOptions) +
    *new _TMenuItem(__("~I~ncludes"), cmShowIncludes, kbNoKey,
                    hcShowIncludes) + newLine() +
    *new _TMenuItem(__("main target~n~ame"), cmTargetName, kbNoKey,
                    hcTargetName) + *new _TMenuItem(__("Primary ~f~ile"),
                                                    cmPrimaryFile, kbNoKey,
                                                    hcPrimaryFile) +
    *new _TMenuItem(__("Clea~r~ dependencies"), cmClearDependencies, kbNoKey,
                    hcClearDependencies) +
    *new _TMenuItem(__("~D~elete rebuildable files"), cmMakeClear, kbNoKey,
                    hcMakeClear)
#if 0
    + *new _TMenuItem(__("Crea~t~e dependencies"), cmCreateDependencies,
                      kbNoKey, hcCreateDependencies)
#endif
    + newLine()
    + *new _TMenuItem(__("~W~rite Makefile"), cmWriteMake, kbNoKey,
                      hcWriteMake);

  TMenuItem & dir_sub =
    *new TSubMenu(_("~D~irectories"), kbNoKey,
                  hcDirectories) + *new _TMenuItem(__("~I~nclude"),
                                                   cmIncludeDir, kbNoKey,
                                                   hcIncludeDir) +
    *new _TMenuItem(__("~L~ibraries"), cmLibDir, kbNoKey,
                    hcLibDir) + *new _TMenuItem(__("~O~bject"), cmObjDir,
                                                kbNoKey,
                                                hcObjDir) +
    *new _TMenuItem(__("~S~ources"), cmSrcDir, kbNoKey,
                    hcSrcDir) + newLine() +
    *new _TMenuItem(__("S~t~andard headers"), cmStandardIncludeDir, kbNoKey,
                    hcStandardIncludeDir);

  TMenuItem & c_compile_sub =
    *new TSubMenu(_("~C~ompilers"), kbNoKey,
                  hcC_CXXCompiler) + *new _TMenuItem(__("~W~arnings"),
                                                     cmWarningFlags, kbNoKey,
                                                     hcWarningFlags) +
    *new _TMenuItem(__("~O~ptimizations"), cmOptimizationFlags, kbNoKey,
                    hcOptimizationFlags) + *new _TMenuItem(__("De~b~ugging"),
                                                           cmDebugFlags,
                                                           kbNoKey,
                                                           hcDebugFlags) +
    *new _TMenuItem(__("~C~-Flags"), cmCFlags, kbNoKey,
                    hcCFlags) + *new _TMenuItem(__("C~X~X-Flags"), cmCXXFlags,
                                                kbNoKey,
                                                hcCXXFlags) +
    *new _TMenuItem(__("P~a~scal-Flags"), cmPascalFlags, kbNoKey,
                    hcPascalFlags) + *new _TMenuItem(__("~F~ortran-Flags"),
                                                     cmFortranFlags, kbNoKey,
                                                     hcFortranFlags) +
    *new _TMenuItem(__("A~d~a-Flags"), cmAdaFlags, kbNoKey, hcAdaFlags);

  TMenuItem & edi_opt = *new TSubMenu(_("~E~ditor"), kbNoKey, hcEditor)
    + *new _TMenuItem(__("~E~ditor options"), cmSetGlobalOptions, kbNoKey,
                      hcSetGlobalOptions) +
    *new _TMenuItem(__("E~d~it key assignment"), cmEditKeyBind, kbNoKey,
                    hcEditKeyBind) +
    *new _TMenuItem(__("~A~lt-Keys settings"), cmSetUpAltKeys, kbNoKey,
                    hcSetUpAltKeys) +
    *new _TMenuItem(__("~R~estore default editor keys"), cmKbBackDefault,
                    kbNoKey, hcKbBackDefault);

  TMenuItem & envi = *new TSubMenu(_("~E~nvironment"), kbNoKey, hcEnvironment)
    + *new _TMenuItem(__("~C~olors"), cmColors, kbNoKey, hcColors)
    + edi_opt
    + *new _TMenuItem(__("~P~references"), cmPreferences, kbNoKey,
                      hcPreferences) + *new _TMenuItem(__("~M~ouse options"),
                                                       cmMouseDlg, kbNoKey,
                                                       hcMouseDlg) +
    *new _TMenuItem(__("~R~eserved words"), cmEditReserved, kbNoKey,
                    hcEditReserved) +
    *new _TMenuItem(__("P~a~scal Reserved words"), cmEditGPCReserved, kbNoKey,
                    hcEditGPCReserved) + *new _TMenuItem(__("C-~F~lags"),
                                                         cmEditCFlags,
                                                         kbNoKey,
                                                         hcEditCFlags) +
    *new _TMenuItem(__("C~X~X-Flags"), cmEditCXXFlags, kbNoKey,
                    hcEditCXXFlags) + *new _TMenuItem(__("P~a~scal-Flags"),
                                                      cmEditPascalFlags,
                                                      kbNoKey,
                                                      hcEditPascalFlags) +
    *new _TMenuItem(__("Fortra~n~-Flags"), cmEditFortranFlags, kbNoKey,
                    hcEditFortranFlags) + *new _TMenuItem(__("Ada-~F~lags"),
                                                          cmEditAdaFlags,
                                                          kbNoKey,
                                                          hcEditAdaFlags) +
    *new _TMenuItem(__("~W~arning-Flags"), cmEditWarnFlags, kbNoKey,
                    hcEditWarnFlags) +
    *new _TMenuItem(__("~D~ebugging-Flags"), cmEditDebugFlags, kbNoKey,
                    hcEditDebugFlags) +
    *new _TMenuItem(__("~O~ptimization-Flags"), cmEditOptFlags, kbNoKey,
                    hcEditOptFlags) + *new _TMenuItem(__("~U~ser words"),
                                                      cmEditUserWords,
                                                      kbNoKey,
                                                      hcEditUserWords);

  TSubMenu & option_menu = *new TSubMenu(_("~O~ptions"), kbAlO, hcOptionsMenu)
    + dir_sub
    + c_compile_sub
    + *new _TMenuItem(__("~L~ibraries"), cmLibraries, kbNoKey, hcLibraries)
    + *new _TMenuItem(__("L~i~nker Options"), cmLinkerOptions, kbNoKey,
                      hcLinkerOptions) +
    *new _TMenuItem(__("Compiler ~O~ptions"), cmCompilerOptions, kbNoKey,
                    hcCompilerOptions) + envi +
    *new _TMenuItem(__("~S~ave options"), cmSaveOptions, kbNoKey,
                    hcSaveOptions) + *new _TMenuItem(__("Loa~d~ options"),
                                                     cmLoadOptions, kbNoKey,
                                                     hcLoadOptions);
  TSubMenu & window_menu =
    *new TSubMenu(_("~W~indows"), kbAlW,
                  hcWindows) + *new _TMenuItem(__("~S~ize/move"), cmResize,
                                               kbCtF5, hcResize,
                                               "Ctrl+F5") +
    *new _TMenuItem(__("~Z~oom"), cmZoom, kbF5, hcZoom,
                    "F5") + *new _TMenuItem(__("~T~ile"), cmTile, kbNoKey,
                                            hcTile) +
    *new _TMenuItem(__("C~a~scade"), cmCascade, kbNoKey,
                    hcCascade) + *new _TMenuItem(__("~N~ext"), cmNext, kbF6,
                                                 hcNext,
                                                 "F6") +
    *new _TMenuItem(__("~P~revious"), cmPrev, kbShF6, hcPrev,
                    "Shift+F6") + *new _TMenuItem(__("~C~lose"), cmClose,
                                                  kbAlF3, hcClose,
                                                  "Alt+F3") +
    *new _TMenuItem(__("~L~ist"), cmShowWindowList, kbAl0, hcShowWindowList,
                    "Alt+0") + newLine() + *new _TMenuItem(__("Pro~j~ect"),
                                                           cmShowProject,
                                                           kbNoKey,
                                                           hcShowProject) +
    *new _TMenuItem(__("~W~atches"), cmShowWatchWindow, kbNoKey,
                    hcShowWatchWindow) + *new _TMenuItem(__("~M~essages"),
                                                         cmShowMessages,
                                                         kbNoKey,
                                                         hcShowMessages) +
    *new _TMenuItem(__("~U~serScreen"), cmUserScreen, kbAlF5, hcUserScreen,
                    "Alt+F5");

  TSubMenu & space_menu = *new TSubMenu(_("~ð~"), kbAlSpace)
    + *new _TMenuItem(__("about RH~I~DE"), cmAbout, kbNoKey, hcAbout)
    + *new _TMenuItem(__("~s~mall bug report"), cmBugReportSmall, kbNoKey,
                      hcBugReportSmall) + *new _TMenuItem(__("~B~ug report"),
                                                          cmBugReport,
                                                          kbNoKey,
                                                          hcBugReport) +
    newLine()
#ifdef __DJGPP__
    + *new _TMenuItem(__("~F~SDB"), cmFSDB, kbNoKey, hcFSDB)
#endif
    + *new _TMenuItem(__("~G~REP ..."), cmGREP, kbShF2, hcGREP, "Shift+F2")
    + *new _TMenuItem(__("G~D~B"), cmGDB, kbNoKey, hcGDB)
    + newLine()
    + *new _TMenuItem(__("Refresh desk~t~op"), cmRepaint, kbNoKey, hcRepaint)
    + newLine()
    + *new _TMenuItem(__("~C~alculator"), cmShowCalculator, kbAlF4,
                      hcShowCalculator,
                      "Alt+F4") + *new _TMenuItem(__("~P~uzzle"), cmPuzzle,
                                                  kbNoKey,
                                                  hcPuzzle) +
    *new _TMenuItem(__("Ca~l~endar"), cmCalendar, kbNoKey,
                    hcCalendar) + *new _TMenuItem(__("~A~SCII table"),
                                                  cmAscii, kbNoKey, hcAscii);

#ifdef INTERNAL_DEBUGGER
  TSubMenu & debug_menu = *new TSubMenu(_("~D~ebug"), kbAlD, hcDebug)
    + *new _TMenuItem(__("~S~et/Reset Breakpoint"), cmToggleBreak, kbCtF8,
                      hcToggleBreak,
                      "Ctrl+F8") + *new _TMenuItem(__("~E~valuate/Modify"),
                                                   cmEvaluate, kbCtF4,
                                                   hcEvaluate,
                                                   "Ctrl+F4") +
    *new _TMenuItem(__("~W~atch an expression"), cmAddWatchEntry, kbCtF7,
                    hcAddWatchEntry,
                    "Ctrl+F7") +
    *new _TMenuItem(__("~I~nspect an expression"), cmInspect, kbCtF6,
                    hcInspect,
                    "Ctrl+F6") + *new _TMenuItem(__("~B~reakpoints"),
                                                 cmBreakPoints, kbNoKey,
                                                 hcBreakPoints) + newLine() +
    *new _TMenuItem(__("~D~isassembler window"), cmDisWindow, kbNoKey,
                    hcDisWindow) + *new _TMenuItem(__("~C~all stack"),
                                                   cmCallStack, kbCtF3,
                                                   hcCallStack,
                                                   "Ctrl+F3") +
    *new _TMenuItem(__("List of ~F~unctions"), cmFunctionList, kbNoKey,
                    hcFunctionList) + *new _TMenuItem(__("D~a~ta window"),
                                                      cmAddDataWindow,
                                                      kbNoKey,
                                                      hcAddDataWindow) +
    *new _TMenuItem(__("S~t~ack window"), cmShowStackWindow, kbNoKey,
                    hcShowStackWindow);
#endif

  bar = new TMenuBar(r,
                     space_menu
                     + file_menu
                     + edit_menu + search_menu + run_menu + compile_menu
#ifdef INTERNAL_DEBUGGER
                     + debug_menu
#endif
                     + project_menu + option_menu + window_menu + help_menu);

  return bar;
}

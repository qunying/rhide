/* Copyright (C) 1996,1997 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
/*
 $Id$
*/
#define Uses_TRect
#define Uses_TKeys
#define Uses_RawKeys
#define Uses_IDEConst
#define Uses_TMenuBar
#define Uses_TSubMenu
#define Uses_editCommands
#define Uses_TApplication
#define Uses_TDialog
#define Uses_TSortedListBox
#define Uses_TStringCollection

#define Uses_ideCommands
#include <libide.h>

#include <rhide.h>
#include <ideapp.h>
#include <string.h>
#include <inf.h>

#define Uses_TCEditor_Commands
#define Uses_TCEditor_External
#include <ceditor.h>

// This is ony a little trick to reduce the size

class _TMenuItem : public TMenuItem
{
public:
    _TMenuItem( const char *aName,
               ushort aCommand,
               ushort aKeyCode,
               ushort aHelpCtx = hcNoContext,
               char *p = 0,
               TMenuItem *aNext = 0
             );
    _TMenuItem( const char *aName,
               ushort aKeyCode,
               TMenu *aSubMenu,
               ushort aHelpCtx = hcNoContext,
               TMenuItem *aNext = 0
             );

};

_TMenuItem::_TMenuItem( const char *aName,
           ushort aCommand,
           ushort aKeyCode,
           ushort aHelpCtx,
           char *p,
           TMenuItem *aNext
         ) :
  TMenuItem(_(aName),aCommand,aKeyCode,aHelpCtx,p,aNext)
{
}

_TMenuItem::_TMenuItem( const char *aName,
           ushort aKeyCode,
           TMenu *aSubMenu,
           ushort aHelpCtx,
           TMenuItem *aNext
         ) :
  TMenuItem(_(aName),aKeyCode,aSubMenu,aHelpCtx,aNext)
{
}

TMenuBar *IDE::initMenuBar( TRect r )
{
  TMenuBar *bar;
  r = TProgram::application->getExtent();
  r.b.y = r.a.y + 1;

  TMenuItem &syntax_sub = *new TSubMenu(_("~S~yntax help"), kbAltS, hcSyntax)
    +*new _TMenuItem( __("~S~yntax help"), cmSyntaxHelp, kbCtrlF1, hcSyntaxHelp, "Ctrl+F1")
    +*new _TMenuItem( __("~F~iles to search"), cmSyntaxFiles, kbNoKey, hcSyntaxFiles)
    +*new _TMenuItem( __("~O~ptions for searching"), cmSyntaxOptions, kbNoKey, hcSyntaxOptions)
  ;

  TSubMenu &help_menu = *new TSubMenu( _("~H~elp"), kbAltH , hcHelpMenu )
    +*new _TMenuItem( __("~H~elp"), cmHelp, kbF1, hcHelp, "F1" )
    +*new _TMenuItem( __("RHIDE Help ~i~ndex"), cmHelpIndex, kbShiftF1, hcHelpIndex, "Shift+F1" )
    +     syntax_sub
    +*new _TMenuItem( __("Index for s~y~ntax help"), cmSyntaxIndex, kbNoKey, hcSyntaxIndex)
    +*new _TMenuItem( __("lib~c~ reference"), cmLibcHelp, kbNoKey, hcLibcHelp )
    +*new _TMenuItem( __("Help ~a~bout help"), cmHelpHelp, kbNoKey, hcHelpHelp )
    +*new _TMenuItem( __("~L~ast help"), cmLastHelp, kbAltF1, hcLastHelp, "Alt+F1" )
  ;

  TSubMenu& file_menu = *new TSubMenu( _("~F~ile"), kbAltF, hcFile )
    +*new _TMenuItem( __("~O~pen"), cmOpenEditor, kbF3, hcOpenEditor, "F3" )
    +*new _TMenuItem( __("~N~ew"), cmNewEditor, kbNoKey, hcNewEditor )
    +*new _TMenuItem( __("~S~ave"), cmSaveEditor, kbF2, hcSaveEditor, "F2" )
    +*new _TMenuItem( __("S~a~ve as..."), cmSaveAs, kbNoKey, hcSaveAs )
    +*new _TMenuItem( __("Save a~l~l"), cmSaveAll, kbNoKey, hcSaveAll )
    +     newLine()
#ifdef __DJGPP__
    +*new _TMenuItem( __("~D~OS shell"), cmDosShell, kbNoKey, hcDosShell )
#else
    +*new _TMenuItem( __("S~h~ell"), cmDosShell, kbNoKey, hcDosShell )
#endif
    +*new _TMenuItem( __("E~x~it"), cmQuit, kbAltX, hcQuit, "Alt+X" )
  ;

  TMenuItem& macro_sub = *new TSubMenu( _("~M~acro"), kbAltM, hcMacro )
    +*new _TMenuItem( __("~R~ecord"), cmcRecordMacro, kbShiftF10, hcRecord, "Shift+F10" )
    +*new _TMenuItem( __("~S~top"), cmcStopMacro, kbAltF10, hcStop, "Alt+F10" )
    +*new _TMenuItem( __("~P~lay"), cmcPlayMacro, kbCtrlF10, hcPlay, "Ctrl+F10" )
  ;

  TSubMenu& edit_menu = *new TSubMenu( _("~E~dit"), kbAltE, hcEdit )
    +*new _TMenuItem( __("~U~ndo"), cmcUndo, kbAltBack, hcUndo, "Alt+Backspace" )
    +*new _TMenuItem( __("~R~edo"), cmcRedo, kbNoKey, hcRedo )
    +     newLine()
    +*new _TMenuItem( __("Cu~t~"), cmcCut, kbShiftDel, hcCut, "Shift+Del" )
    +*new _TMenuItem( __("~C~opy"), cmcCopy, kbCtrlIns, hcCopy, "Ctrl+Ins" )
    +*new _TMenuItem( __("~P~aste"), cmcPaste, kbShiftIns, hcPaste, "Shift+Ins" )
    +*new _TMenuItem( __("~S~how clipboard"), cmShowClip, kbNoKey, hcShowClip )
    +     newLine()
    +*new _TMenuItem( __("C~l~ear"), cmcClear, kbCtrlDel, hcClear, "Ctrl+Del" )
    +     newLine()
#ifdef __DJGPP__
    +*new _TMenuItem( __("Cop~y~ to Windows Clipboard"), cmcCopyClipWin, kbNoKey, hcCopyClipWin)
    +*new _TMenuItem( __("Pas~t~e from Windows Clipboard"), cmcPasteClipWin, kbNoKey, hcPasteClipWin)
    +     newLine()
#endif
    +*new _TMenuItem( __("E~x~pand all tabs"), cmcExpandAllTabs, kbNoKey, hcExpandAllTabs )
    +*new _TMenuItem( __("C~o~mpact text"), cmcCompactBuffer, kbNoKey, hcCompactBuffer )
    +     newLine()
    +     macro_sub
    +     newLine()
    +*new _TMenuItem( __("Profile Editor"), cmcProfileEditor, kbNoKey,hcProfileEditor )
  ;

  TSubMenu& search_menu = *new TSubMenu( _("~S~earch"), kbAltS, hcSearch )
    +*new _TMenuItem( __("~F~ind..."), cmcFind, kbNoKey, hcFind )
    +*new _TMenuItem( __("~R~eplace..."), cmcReplace, kbNoKey, hcReplace )
    +*new _TMenuItem( __("~S~earch again"), cmcSearchAgain, kbNoKey, hcSearchAgain, "Ctrl+L" )
    +*new _TMenuItem( __("~G~oto Line..."), cmGotoLine, kbNoKey, hcGotoLine, "Ctrl+J" )
    +*new _TMenuItem( __("~J~ump to function"), cmJumpToFunction, kbAltF2, hcJumpToFunction, "Alt+F2" )
    +     newLine()
    +*new _TMenuItem( __("~N~ext message"), cmNextMessage, kbAltF8, hcNextMessage, "Alt+F8")
    +*new _TMenuItem( __("Previ~o~us message"), cmPrevMessage, kbAltF7, hcPrevMessage, "Alt+F7")
  ;

  TSubMenu & run_menu = *new TSubMenu( _("~R~un"), kbAltR, hcRunMenu )
    +*new _TMenuItem( __("~R~un"), cmRun, kbCtrlF9, hcRun, "Ctrl+F9")
#ifdef INTERNAL_DEBUGGER
    +*new _TMenuItem( __("~S~tep over"), cmStep, kbF8, hcStep, "F8" )
    +*new _TMenuItem( __("Step over (No display swap)"), cmStepNo, kbShiftF8, hcStepNo, "Shift+F8" )
    +*new _TMenuItem( __("~T~race into"), cmTrace, kbF7, hcTrace, "F7" )
    +*new _TMenuItem( __("Trace into (No display swap)"), cmTraceNo, kbShiftF7, hcTraceNo, "Shift+F7" )
    +*new _TMenuItem( __("~G~o to cursor"), cmGoto, kbF4, hcGoto, "F4" )
    +*new _TMenuItem( __("Go to cursor (No display swap)"), cmGotoNo, kbShiftF4, hcGotoNo, "Shift+F4" )
    +*new _TMenuItem( __("~U~ntil return"), cmFinish, kbNoKey, hcFinish)
    +*new _TMenuItem( __("~P~rogram reset"), cmReset, kbCtrlF2, hcReset, "Ctrl+F2" )
    +      newLine()
    +*new _TMenuItem( __("~M~ain function"), cmMainFunction, kbNoKey, hcMainFunction)
#endif
    +*new _TMenuItem( __("~A~rguments ..."), cmProgArgs, kbNoKey, hcProgArgs)
  ;

  TSubMenu& compile_menu = *new TSubMenu( _("~C~ompile"), kbAltC, hcCompileMenu )
    +*new _TMenuItem( __("~C~ompile"), cmCompile, kbAltF9, hcCompile, "Alt+F9" )
    +*new _TMenuItem( __("~M~ake"), cmMake, kbF9, hcMake, "F9")
    +*new _TMenuItem( __("~L~ink"), cmLink, kbNoKey, hcLink)
    +*new _TMenuItem( __("~B~uild all"), cmBuild, kbNoKey, hcBuild)
  ;

  TSubMenu& project_menu = *new TSubMenu( _("~P~roject"), kbAltP,hcProjectMenu )
    +*new _TMenuItem( __("Open ~p~roject"), cmOpenProject, kbNoKey, hcOpenProject )
    +*new _TMenuItem( __("~C~lose project"), cmCloseProject, kbNoKey, hcCloseProject )
    +*new _TMenuItem( __("~A~dd item"), cmAddProjectItem, kbNoKey, hcAddProjectItem )
    +*new _TMenuItem( __("D~e~lete item"), cmDelProjectItem, kbNoKey, hcDelProjectItem )
    +*new _TMenuItem( __("~L~ocal options"), cmLocalOptions, kbNoKey, hcLocalOptions )
    +*new _TMenuItem( __("~I~ncludes"), cmShowIncludes, kbNoKey, hcShowIncludes )
    +     newLine()
    +*new _TMenuItem( __("main target~n~ame"), cmTargetName, kbNoKey, hcTargetName)
    +*new _TMenuItem( __("Primary ~f~ile"), cmPrimaryFile, kbNoKey, hcPrimaryFile)
    +*new _TMenuItem( __("Clea~r~ dependencies"), cmClearDependencies, kbNoKey, hcClearDependencies)
    +*new _TMenuItem( __("~D~elete rebuildable files"), cmMakeClear, kbNoKey, hcMakeClear)
#if 0
    +*new _TMenuItem( __("Crea~t~e dependencies"), cmCreateDependencies, kbNoKey, hcCreateDependencies)
#endif
    +     newLine()
    +*new _TMenuItem( __("~W~rite Makefile"), cmWriteMake, kbNoKey, hcWriteMake )
  ;

  TMenuItem &dir_sub = *new TSubMenu(_("~D~irectories"), kbNoKey, hcDirectories)
    +*new _TMenuItem( __("~I~nclude"), cmIncludeDir, kbNoKey, hcIncludeDir )
    +*new _TMenuItem( __("~L~ibraries"), cmLibDir, kbNoKey, hcLibDir )
    +*new _TMenuItem( __("~O~bject"), cmObjDir, kbNoKey, hcObjDir )
    +*new _TMenuItem( __("~S~ources"), cmSrcDir, kbNoKey, hcSrcDir )
    +     newLine()
    +*new _TMenuItem( __("S~t~andard headers"), cmStandardIncludeDir, kbNoKey, hcStandardIncludeDir)
  ;

  TMenuItem &c_compile_sub = *new TSubMenu(_("~C~ompilers"), kbNoKey, hcC_CXXCompiler)
    +*new _TMenuItem( __("~W~arnings"), cmWarningFlags, kbNoKey, hcWarningFlags)
    +*new _TMenuItem( __("~O~ptimizations"), cmOptimizationFlags, kbNoKey, hcOptimizationFlags)
    +*new _TMenuItem( __("De~b~ugging"), cmDebugFlags, kbNoKey, hcDebugFlags )
    +*new _TMenuItem( __("~C~-Flags"), cmCFlags, kbNoKey, hcCFlags )
    +*new _TMenuItem( __("C~X~X-Flags"), cmCXXFlags, kbNoKey, hcCXXFlags)
    +*new _TMenuItem( __("P~a~scal-Flags"), cmPascalFlags, kbNoKey, hcPascalFlags)
    +*new _TMenuItem( __("~F~ortran-Flags"), cmFortranFlags, kbNoKey, hcFortranFlags)
    +*new _TMenuItem( __("A~d~a-Flags"), cmAdaFlags, kbNoKey, hcAdaFlags)
  ;

  TMenuItem &envi = *new TSubMenu(_("~E~nvironment"), kbNoKey, hcEnvironment)
    +*new _TMenuItem( __("~C~olors"), cmColors, kbNoKey, hcColors )
    +*new _TMenuItem( __("~E~ditor options"), cmSetGlobalOptions, kbNoKey, hcSetGlobalOptions)
    +*new _TMenuItem( __("~P~references"), cmPreferences, kbNoKey, hcPreferences)
    +*new _TMenuItem( __("~M~ouse options"), cmMouseDlg, kbNoKey, hcMouseDlg)
    +*new _TMenuItem( __("~R~eserved words"), cmEditReserved, kbNoKey, hcEditReserved)
    +*new _TMenuItem( __("P~a~scal Reserved words"), cmEditGPCReserved, kbNoKey, hcEditGPCReserved)
    +*new _TMenuItem( __("C-~F~lags"), cmEditCFlags, kbNoKey, hcEditCFlags)
    +*new _TMenuItem( __("C~X~X-Flags"), cmEditCXXFlags, kbNoKey, hcEditCXXFlags)
    +*new _TMenuItem( __("P~a~scal-Flags"), cmEditPascalFlags, kbNoKey, hcEditPascalFlags)
    +*new _TMenuItem( __("Fortra~n~-Flags"), cmEditFortranFlags, kbNoKey, hcEditFortranFlags)
    +*new _TMenuItem( __("Ada-~F~lags"), cmEditAdaFlags, kbNoKey, hcEditAdaFlags)
    +*new _TMenuItem( __("~W~arning-Flags"), cmEditWarnFlags, kbNoKey, hcEditWarnFlags)
    +*new _TMenuItem( __("~D~ebugging-Flags"), cmEditDebugFlags, kbNoKey, hcEditDebugFlags)
    +*new _TMenuItem( __("~O~ptimization-Flags"), cmEditOptFlags, kbNoKey, hcEditOptFlags)
    +*new _TMenuItem( __("~U~ser words"), cmEditUserWords, kbNoKey, hcEditUserWords)
  ;

  TSubMenu& option_menu = *new TSubMenu( _("~O~ptions"), kbAltO, hcOptionsMenu )
    +     dir_sub
    +     c_compile_sub
    +*new _TMenuItem( __("~L~ibraries"), cmLibraries, kbNoKey, hcLibraries )
    +*new _TMenuItem( __("L~i~nker Options"), cmLinkerOptions, kbNoKey, hcLinkerOptions )
    +*new _TMenuItem( __("Compiler ~O~ptions"), cmCompilerOptions, kbNoKey, hcCompilerOptions )
    +     envi
    +*new _TMenuItem( __("~S~ave options"), cmSaveOptions, kbNoKey, hcSaveOptions )
    +*new _TMenuItem( __("Loa~d~ options"), cmLoadOptions, kbNoKey, hcLoadOptions )
  ;
  TSubMenu& window_menu = *new TSubMenu( _("~W~indows"), kbAltW, hcWindows )
    +*new _TMenuItem( __("~S~ize/move"), cmResize, kbCtrlF5, hcResize, "Ctrl+F5" )
    +*new _TMenuItem( __("~Z~oom"), cmZoom, kbF5, hcZoom, "F5" )
    +*new _TMenuItem( __("~T~ile"), cmTile, kbNoKey, hcTile )
    +*new _TMenuItem( __("C~a~scade"), cmCascade, kbNoKey, hcCascade )
    +*new _TMenuItem( __("~N~ext"), cmNext, kbF6, hcNext, "F6" )
    +*new _TMenuItem( __("~P~revious"), cmPrev, kbShiftF6, hcPrev, "Shift+F6" )
    +*new _TMenuItem( __("~C~lose"), cmClose, kbAltF3, hcClose, "Alt+F3" )
    +*new _TMenuItem( __("~L~ist"), cmShowWindowList, kbAlt0, hcShowWindowList, "Alt+0" )
    +*new _TMenuItem( __("Pro~j~ect"), cmShowProject, kbNoKey, hcShowProject)
    +*new _TMenuItem( __("~W~atches"), cmShowWatchWindow, kbNoKey, hcShowWatchWindow )
    +     newLine()
    +*new _TMenuItem( __("~U~serScreen"), cmUserScreen, kbAltF5, hcUserScreen, "Alt+F5" )
  ;

  TSubMenu& space_menu = *new TSubMenu( _("~ð~"), kbAltSpace )
    +*new _TMenuItem( __("about RH~I~DE"), cmAbout, kbNoKey, hcAbout )
    +*new _TMenuItem( __("~s~mall bug report"), cmBugReportSmall, kbNoKey, hcBugReportSmall)
    +*new _TMenuItem( __("~B~ug report"), cmBugReport, kbNoKey, hcBugReport)
    +     newLine()
#ifdef __DJGPP__
    +*new _TMenuItem( __("~F~SDB"), cmFSDB, kbNoKey, hcFSDB )
#endif
    +*new _TMenuItem( __("~G~REP ..."), cmGREP, kbShiftF2, hcGREP, "Shift+F2" )
    +*new _TMenuItem( __("G~D~B"), cmGDB, kbNoKey, hcGDB )
    +     newLine()
    +*new _TMenuItem( __("Refresh desk~t~op"), cmRepaint, kbNoKey, hcRepaint )
    +     newLine()
    +*new _TMenuItem( __("~C~alculator"), cmShowCalculator, kbAltF4, hcShowCalculator, "Alt+F4" )
    +*new _TMenuItem( __("~P~uzzle"), cmPuzzle, kbNoKey, hcPuzzle )
    +*new _TMenuItem( __("Ca~l~endar"), cmCalendar, kbNoKey, hcCalendar )
    +*new _TMenuItem( __("~A~SCII table"), cmAscii, kbNoKey, hcAscii )
  ;

#ifdef INTERNAL_DEBUGGER
  TSubMenu& debug_menu = *new TSubMenu( _("~D~ebug"), kbAltD, hcDebug )
    +*new _TMenuItem( __("~S~et/Reset Breakpoint"), cmToggleBreak, kbCtrlF8, hcToggleBreak, "Ctrl+F8" )
    +*new _TMenuItem( __("~E~valuate/Modify"), cmEvaluate, kbCtrlF4, hcEvaluate, "Ctrl+F4" )
    +*new _TMenuItem( __("~W~atch an expression"), cmAddWatchEntry, kbCtrlF7, hcAddWatchEntry, "Ctrl+F7" )
    +*new _TMenuItem( __("~B~reakpoints"),cmBreakPoints,kbNoKey,hcBreakPoints)
    +     newLine()
    +*new _TMenuItem( __("~D~isassembler window"),cmDisWindow,kbNoKey,hcDisWindow)
    +*new _TMenuItem( __("~C~all stack"),cmCallStack,kbCtrlF3,hcCallStack,"Ctrl+F3")
    +*new _TMenuItem( __("List of ~F~unctions"), cmFunctionList, kbNoKey, hcFunctionList)
  ;
#endif

  bar = new TMenuBar( r,
	   space_menu
	 + file_menu
	 + edit_menu
	 + search_menu
	 + run_menu
	 + compile_menu
#ifdef INTERNAL_DEBUGGER
	 + debug_menu
#endif
         + project_menu
         + option_menu
	 + window_menu
         + help_menu
        );

  return bar;
}



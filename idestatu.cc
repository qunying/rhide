/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
/*
 $Id$
*/
#define Uses_TRect
#define Uses_TKeys
#define Uses_RawKeys
#define Uses_TStatusDef
#define Uses_TStatusItem
#define Uses_TDialog
#define Uses_TEditor
#define Uses_editCommands
#define Uses_TSortedListBox
#define Uses_TStringCollection
#define Uses_TApplication
#define Uses_TFileViewer

#define Uses_ideCommands
#include <libide.h>

#define Uses_THintStatusLine
#define Uses_TSCollection
#define Uses_tvutilCommands
#include <libtvuti.h>

#define Uses_IDEConst
#include <rhide.h>
#include <ideapp.h>

#ifdef INTERNAL_DEBUGGER
#define Uses_tvgdbCommands
#include <libtvgdb.h>
#endif

#include <string.h>
#include <inf.h>

#define Uses_TCEditor_Commands
#define Uses_TCEditor_External
#include <ceditor.h>

TStatusLine *IDE::initStatusLine( TRect r )
{
  r = TProgram::application->getExtent();
  r.a.y = r.b.y-1;
  TStatusLine *line;
  line = new THintStatusLine( r,
     *new TStatusDef( hcFileViewer, hcFileViewer )
      +*new TStatusItem( _("~F1~ Help"), kbF1, cmHelp )
//      +*new TStatusItem( NULL, kbShiftF1, cmHelpIndex )
      +*new TStatusItem( _("~F2~ Save"), kbF2, cmSaveFile )
      +*new TStatusItem( _("~F3~ Open"), kbF3, cmOpenEditor )
      +*new TStatusItem( _("~F5~ Zoom"), kbF5, cmZoom )
      +*new TStatusItem( _("~F6~ Next"), kbF6, cmNext )
      +*new TStatusItem( _("~F10~ Menu"), kbF10, cmMenu )
      +*new TStatusItem( _("~Alt+X~ Quit"), kbAltX, cmQuit )
    +*new TStatusDef( hcProjectListBox, hcProjectListBox )
//      +*new TStatusItem( NULL, kbF1, cmHelp )
//      +*new TStatusItem( NULL, kbShiftF1, cmHelpIndex )
      +*new TStatusItem( _("~Ins~ Add"), kbIns, cmAddProjectItem )
      +*new TStatusItem( _("~Del~ Delete"), kbDel, cmDelProjectItem )
      +*new TStatusItem( _("~^O~ Local"), kbCtrlO, cmLocalOptions )
      +*new TStatusItem( _("~^I~ Includes"), kbCtrlI, cmShowIncludes )
      +*new TStatusItem( _("~F5~ Zoom"), kbF5, cmZoom )
      +*new TStatusItem( _("~Alt+F9~ Compile"), kbAltF9, cmCompile )
      +*new TStatusItem( _("~Alt+X~ Quit"), kbAltX, cmQuit )
      +*new TStatusItem( NULL, kbF10, cmMenu )
    +*new TStatusDef(  hcMsgListBox, hcMsgListBox )
//      +*new TStatusItem( NULL, kbShiftF1, cmHelpIndex )
      +*new TStatusItem( _("~Enter~ Jump to source"), kbF1, cmHelp )
//      +*new TStatusItem( _("~F5~ Zoom"), kbF5, cmZoom )
//      +*new TStatusItem( _("~F6~ Next"), kbF6, cmNext )
      +*new TStatusItem( _("~Alt+F9~ Compile"), kbAltF9, cmCompile )
//      +*new TStatusItem( _("~F10~ Menu"), kbF10, cmMenu )
      +*new TStatusItem( _("~Del~ Delete"), kbDel, cmClearMessage)
      +*new TStatusItem( _("~Ctrl+Del~ Delete all"), kbCtrlDel, cmClearMessages)
      +*new TStatusItem( _("~Alt+X~ Quit"), kbAltX, cmQuit )
    +*new TStatusDef( hcWindowList, hcWindowList )
//      +*new TStatusItem( NULL, kbF1, cmHelp )
//      +*new TStatusItem( NULL, kbShiftF1, cmHelpIndex )
      +*new TStatusItem( _("~Enter~ Select"), kbEnter, cmSelectWindow )
      +*new TStatusItem( _("~Del~ Close"), kbDel, cmDeleteWindow )
      +*new TStatusItem( NULL, kbCtrlF5, cmResize )
    +*new TStatusDef( hcIDEFileEditor, hcIDEFileEditor )
//      +*new TStatusItem( NULL, kbF1, cmHelp )
//      +*new TStatusItem( NULL, kbShiftF1, cmHelpIndex )
//      +*new TStatusItem( NULL, kbCtrlF1, cmSyntaxHelp )
      +*new TStatusItem( _("~F2~ Save"), kbF2, cmSaveEditor )
      +*new TStatusItem( _("~F3~ Open"), kbF3, cmOpenEditor )
      +*new TStatusItem( _("~F5~ Zoom"), kbF5, cmZoom )
      +*new TStatusItem( _("~F6~ Next"), kbF6, cmNext )
      +*new TStatusItem( _("~Alt+F9~ Compile"), kbAltF9, cmCompile )
      +*new TStatusItem( _("~F10~ Menu"), kbF10, cmMenu )
      +*new TStatusItem( _("~Alt+X~ Quit"), kbAltX, cmQuit )
      +*new TStatusItem( NULL, kbCtrlJ, cmGotoLine )
      +*new TStatusItem( NULL, kbAltL, cmcSetLocalOptions )
    +*new TStatusDef( hcInfView, hcInfView )
//      +*new TStatusItem( NULL, kbShiftF1, cmHelpIndex )
      +*new TStatusItem( _("~F1~ Help on InfoViewer"), kbF1, cmInfHelp )
      +*new TStatusItem( _("~Alt+F1~ last help"), kbAltF1, cmLastHelp )
      +*new TStatusItem( _("~Alt+F10~ local menu"), kbAltF10, cmInfControl )
      +*new TStatusItem( NULL, kbF10, cmMenu )
      +*new TStatusItem( _("~^F1~ Bookmarks"), kbCtrlF1, cmInfBookM )
      +*new TStatusItem( NULL, kbCtrlI, cmInfPasteIn )
//      +*new TStatusItem( NULL, kbCtrlF5, cmResize )
#ifdef INTERNAL_DEBUGGER
    +*new TStatusDef(hcBreakListDialog, hcBreakListDialog)
      +*new TStatusItem( _("~Ins~ Add"), kbIns, cmNewBreak)
      +*new TStatusItem( _("~Del~ Delete"), kbDel, cmDeleteBreak)
      +*new TStatusItem( _("~Enter~ Jump to source"), kbEnter, cmSelectBreak)
    +*new TStatusDef(hcFunctionListBox,hcFunctionListBox)
      +*new TStatusItem( _("~F1~ Help"), kbF1, cmHelp )
      +*new TStatusItem( _("~Enter~ Jump to source"), kbCtrlF5, cmResize )
    +*new TStatusDef(hcFunctionInputLine,hcFunctionInputLine)
      +*new TStatusItem(_("~Ctrl+F1~ Select from a list"), kbCtrlF1, cmSelectFunction)
    +*new TStatusDef( hcWatchListBox, hcWatchListBox )
      +*new TStatusItem( _("~F1~ Help"), kbF1, cmHelp )
//      +*new TStatusItem( NULL, kbShiftF1, cmHelpIndex )
      +*new TStatusItem( _("~Ins~ Add"), kbIns, cmAddWatchEntry )
      +*new TStatusItem( _("~Del~ Delete"), kbDel, cmRemoveWatch )
      +*new TStatusItem( _("~Enter~ Edit"), kbEnter, cmEditWatch )
      +*new TStatusItem( _("~F5~ Zoom"), kbF5, cmZoom )
      +*new TStatusItem( _("~Alt+X~ Quit"), kbAltX, cmQuit )
      +*new TStatusItem( _("~F10~ Menu"), kbF10, cmMenu )
#endif
    +*new TStatusDef( hcOptFlagList, hcAdaFlagList )
      +*new TStatusItem( _("~F1~ Help"), kbF1, cmHelp )
      +*new TStatusItem( _("~Esc~ Cancel"), kbEsc, cmCancel )
      +*new TStatusItem( _("~Space~ select"), RawkbSpace, cmEnter )
      +*new TStatusItem( NULL, kbShiftF1, cmHelpIndex )
      +*new TStatusItem( NULL, kbCtrlF5, cmResize )
    +*new TStatusDef( 0, 99 )
//      +*new TStatusItem( NULL, kbF1, cmHelp )
//      +*new TStatusItem( NULL, kbShiftF1, cmHelpIndex )
      +*new TStatusItem( _("~F2~ Save"), kbF2, cmSaveEditor )
      +*new TStatusItem( _("~F3~ Open"), kbF3, cmOpenEditor )
      +*new TStatusItem( _("~F5~ Zoom"), kbF5, cmZoom )
      +*new TStatusItem( _("~F6~ Next"), kbF6, cmNext )
      +*new TStatusItem( _("~Alt+F9~ Compile"), kbAltF9, cmCompile )
      +*new TStatusItem( _("~F10~ Menu"), kbF10, cmMenu )
      +*new TStatusItem( _("~Alt+X~ Quit"), kbAltX, cmQuit )
//      +*new TStatusItem( NULL, kbCtrlF5, cmResize )
    +*new TStatusDef( hcPreferenceCheckbox, hcPreferenceCheckbox+31)
      +*new TStatusItem( _("~F1~ Help"), kbF1, cmHelp )
      +*new TStatusItem( _("~Space~ select"), RawkbSpace, cmEnter )
      +*new TStatusItem( _("~Esc~ Cancel"), kbEsc, cmCancel )
    +*new TStatusDef( hcPreferenceCheckbox1, hcPreferenceCheckbox1+31)
      +*new TStatusItem( _("~F1~ Help"), kbF1, cmHelp )
      +*new TStatusItem( _("~Space~ select"), RawkbSpace, cmEnter )
      +*new TStatusItem( _("~Esc~ Cancel"), kbEsc, cmCancel )
    +*new TStatusDef( 100, 555)
//      +*new TStatusItem( NULL, kbShiftF1, cmHelpIndex )
      +*new TStatusItem( _("~F1~ Help"), kbF1, cmHelp )
      +*new TStatusItem( NULL, kbF10, cmMenu )
#ifdef INTERNAL_DEBUGGER
    +*new TStatusDef( hcDisassemblerViewer, hcDisassemblerViewer)
      +*new TStatusItem( _("~F1~ Help"), kbF1, cmHelp)
      +*new TStatusItem( _("~^G~ Goto"), kbCtrlG, cmGotoDisass)
      +*new TStatusItem( _("~Alt+F3~ Close"), kbAltF3, cmClose)
    +*new TStatusDef( hcRegister, hcRegister)
      +*new TStatusItem( _("~F1~ Help"), kbF1, cmHelp)
      +*new TStatusItem( _("~Enter~ Modify register"), kbEnter, cmModifyRegister)
      +*new TStatusItem( _("~Alt+F3~ Close"), kbAltF3, cmClose)
#endif
    +*new TStatusDef( 0, 0xFFFF )
      +*new TStatusItem( _("~F1~ Help"), kbF1, cmHelp )
      +*new TStatusItem( NULL, kbShiftF1, cmHelpIndex )
      +*new TStatusItem( _("~F2~ Save"), kbF2, cmSaveEditor )
      +*new TStatusItem( _("~F3~ Open"), kbF3, cmOpenEditor )
      +*new TStatusItem( _("~F5~ Zoom"), kbF5, cmZoom )
      +*new TStatusItem( _("~F6~ Next"), kbF6, cmNext )
      +*new TStatusItem( _("~Alt+F9~ Compile"), kbAltF9, cmCompile )
      +*new TStatusItem( _("~F10~ Menu"), kbF10, cmMenu )
      +*new TStatusItem( _("~Alt+X~ Quit"), kbAltX, cmQuit )
    ,IDEHint);
  return line;
}



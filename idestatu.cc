/* Copyright (C) 1996-2000 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#include <string.h>
#define Uses_TRect
#define Uses_TKeys
#define Uses_TKeys_Extended
#define Uses_TStatusDef
#define Uses_TStatusItem
#define Uses_TDialog
#define Uses_editCommands
#define Uses_TSortedListBox
#define Uses_TStringCollection
#define Uses_TApplication
#define Uses_TFileViewer
#define Uses_TNSSortedCollection
#define Uses_TStringCollection
#define Uses_SOStack
#define Uses_TNoCaseSOSStringCollection
#define Uses_TNoCaseStringCollection
#define Uses_ideCommands
#include <libide.h>
#include <settvuti.h>

#define Uses_THintStatusLine
#define Uses_TSCollection
#define Uses_tvutilCommands
#include <libtvuti.h>

#define Uses_IDEConst
#include "rhide.h"
#include "ideapp.h"

#ifdef INTERNAL_DEBUGGER
#define Uses_tvgdbCommands
#include <libtvgdb.h>
#endif

#include <inf.h>

#define Uses_TCEditor_Commands
#define Uses_TCEditor_External
#include <ceditor.h>

TStatusLine *
IDE::initStatusLine(TRect r)
{
  r = TProgram::application->getExtent();
  r.a.y = r.b.y - 1;
  TStatusLine *line;

  line = new THintStatusLine(r,
                             *new TStatusDef(hcFileViewer, hcFileViewer)
                             + *new TStatusItem(_("~F1~ Help"), kbF1, cmHelp)
                             + *new TStatusItem(_("~F2~ Save"), kbF2,
                                                cmSaveFile) +
                             *new TStatusItem(_("~F3~ Open"), kbF3,
                                              cmOpenEditor) +
                             *new TStatusItem(_("~F5~ Zoom"), kbF5,
                                              cmZoom) +
                             *new TStatusItem(_("~F6~ Next"), kbF6,
                                              cmNext) +
                             *new TStatusItem(_("~F10~ Menu"), kbF10,
                                              cmMenu) +
                             *new TStatusItem(_("~Alt+X~ Quit"), kbAlX,
                                              cmQUIT) +
                             *new TStatusDef(hcProjectListBox,
                                             hcProjectListBox) +
                             *new TStatusItem(_("~Ins~ Add"), kbInsert,
                                              cmAddProjectItem) +
                             *new TStatusItem(_("~Del~ Delete"), kbDelete,
                                              cmDelProjectItem) +
                             *new TStatusItem(_("~^O~ Local"), kbCtO,
                                              cmLocalOptions) +
                             *new TStatusItem(_("~^I~ Includes"), kbCtI,
                                              cmShowIncludes) +
                             *new TStatusItem(NULL, kbCtTab,
                                              cmShowIncludes) +
                             *new TStatusItem(_("~F5~ Zoom"), kbF5,
                                              cmZoom) +
                             *new TStatusItem(_("~Alt+F9~ Compile"), kbAlF9,
                                              cmCompile) +
                             *new TStatusItem(_("~Alt+X~ Quit"), kbAlX,
                                              cmQUIT) + *new TStatusItem(NULL,
                                                                         kbF10,
                                                                         cmMenu)
                             + *new TStatusDef(hcMsgListBox,
                                               hcMsgListBox) +
                             *new TStatusItem(_("~Enter~ Jump to source"),
                                              kbF1,
                                              cmHelp) +
                             *new TStatusItem(_("~Alt+F9~ Compile"), kbAlF9,
                                              cmCompile) +
                             *new TStatusItem(NULL, kbF10,
                                              cmMenu) +
                             *new TStatusItem(_("~Del~ Delete"), kbDelete,
                                              cmClearMessage) +
                             *new TStatusItem(_("~Ctrl+Del~ Delete all"),
                                              kbCtDelete,
                                              cmClearMessages) +
                             *new TStatusItem(_("~Alt+X~ Quit"), kbAlX,
                                              cmQUIT) +
                             *new TStatusDef(hcWindowList,
                                             hcWindowList) +
                             *new TStatusItem(_("~Enter~ Select"), kbEnter,
                                              cmSelectWindow) +
                             *new TStatusItem(_("~Del~ Close"), kbDelete,
                                              cmDeleteWindow) +
                             *new TStatusItem(NULL, kbCtF5,
                                              cmResize) +
                             *new TStatusDef(hcIDEFileEditor,
                                             hcIDEFileEditor) +
                             *new TStatusItem(_("~F2~ Save"), kbF2,
                                              cmSaveEditor) +
                             *new TStatusItem(_("~F3~ Open"), kbF3,
                                              cmOpenEditor) +
                             *new TStatusItem(_("~F5~ Zoom"), kbF5,
                                              cmZoom) +
                             *new TStatusItem(_("~F6~ Next"), kbF6,
                                              cmNext) +
                             *new TStatusItem(_("~Alt+F9~ Compile"), kbAlF9,
                                              cmCompile) +
                             *new TStatusItem(_("~F10~ Menu"), kbF10,
                                              cmMenu) +
                             *new TStatusItem(_("~Alt+X~ Quit"), kbAlX,
                                              cmQUIT) + *new TStatusItem(NULL,
                                                                         kbCtJ,
                                                                         cmGotoLine)
                             + *new TStatusItem(NULL, kbAlL,
                                                cmcSetLocalOptions) +
                             *new TStatusDef(hcInfView,
                                             hcInfView) +
                             *new TStatusItem(_("~F1~ Help on InfoViewer"),
                                              kbF1,
                                              cmInfHelp) +
                             *new TStatusItem(_("~Alt+F1~ last help"), kbAlF1,
                                              cmLastHelp) +
                             *new TStatusItem(_("~Alt+F10~ local menu"),
                                              kbAlF10,
                                              cmInfControl) +
                             *new TStatusItem(NULL, kbF10,
                                              cmMenu) +
                             *new TStatusItem(_("~^F1~ Bookmarks"), kbCtF1,
                                              cmInfBookM) +
                             *new TStatusItem(NULL, kbCtI, cmInfPasteIn)
#ifdef INTERNAL_DEBUGGER
                             + *new TStatusDef(hcBreakListDialog,
                                               hcBreakListDialog) +
                             *new TStatusItem(_("~Ins~ Add"), kbInsert,
                                              cmNewBreak) +
                             *new TStatusItem(_("~Del~ Delete"), kbDelete,
                                              cmDeleteBreak) +
                             *new TStatusItem(_("~Enter~ Jump to source"),
                                              kbEnter,
                                              cmSelectBreak) +
                             *new TStatusDef(hcInspect,
                                             hcInspect) +
                             *new TStatusItem(_("~F1~ Help"), kbF1,
                                              cmHelp) +
                             *new TStatusItem(_("~F5~ Zoom"), kbF5,
                                              cmZoom) + *new TStatusItem(NULL,
                                                                         kbEsc,
                                                                         cmClose)
                             + *new TStatusDef(hcFunctionListBox,
                                               hcFunctionListBox) +
                             *new TStatusItem(_("~F1~ Help"), kbF1,
                                              cmHelp) +
                             *new TStatusItem(_("~Enter~ Jump to source"),
                                              kbCtF5,
                                              cmResize) +
                             *new TStatusDef(hcFunctionInputLine,
                                             hcFunctionInputLine) +
                             *new
                             TStatusItem(_("~Ctrl+F1~ Select from a list"),
                                         kbCtF1,
                                         cmSelectFunction) +
                             *new TStatusDef(hcWatchListBox,
                                             hcWatchListBox) +
                             *new TStatusItem(_("~F1~ Help"), kbF1,
                                              cmHelp) +
                             *new TStatusItem(_("~Ins~ Add"), kbInsert,
                                              cmAddWatchEntry) +
                             *new TStatusItem(_("~Del~ Delete"), kbDelete,
                                              cmRemoveWatch) +
                             *new TStatusItem(_("~Enter~ Edit"), kbEnter,
                                              cmEditWatch) +
                             *new TStatusItem(_("~F5~ Zoom"), kbF5,
                                              cmZoom) +
                             *new TStatusItem(_("~Alt+X~ Quit"), kbAlX,
                                              cmQUIT) +
                             *new TStatusItem(_("~F10~ Menu"), kbF10, cmMenu)
#endif
                             + *new TStatusDef(hcOptFlagList, hcAdaFlagList)
                             + *new TStatusItem(_("~F1~ Help"), kbF1, cmHelp)
                             + *new TStatusItem(_("~Esc~ Cancel"), kbEsc,
                                                cmCancel) +
                             *new TStatusItem(_("~Space~ select"), kbSpace,
                                              cmEnter) +
                             *new TStatusItem(NULL, kbShF1,
                                              cmHelpIndex) +
                             *new TStatusItem(NULL, kbCtF5,
                                              cmResize) + *new TStatusDef(0,
                                                                          99)
                             + *new TStatusItem(_("~F2~ Save"), kbF2,
                                                cmSaveEditor) +
                             *new TStatusItem(_("~F3~ Open"), kbF3,
                                              cmOpenEditor) +
                             *new TStatusItem(_("~F5~ Zoom"), kbF5,
                                              cmZoom) +
                             *new TStatusItem(_("~F6~ Next"), kbF6,
                                              cmNext) +
                             *new TStatusItem(_("~Alt+F9~ Compile"), kbAlF9,
                                              cmCompile) +
                             *new TStatusItem(_("~F10~ Menu"), kbF10,
                                              cmMenu) +
                             *new TStatusItem(_("~Alt+X~ Quit"), kbAlX,
                                              cmQUIT) +
                             *new TStatusDef(hcPreferenceCheckbox,
                                             hcPreferenceCheckbox + 31) +
                             *new TStatusItem(_("~F1~ Help"), kbF1,
                                              cmHelp) +
                             *new TStatusItem(_("~Space~ select"), kbSpace,
                                              cmEnter) +
                             *new TStatusItem(_("~Esc~ Cancel"), kbEsc,
                                              cmCancel) +
                             *new TStatusDef(hcPreferenceCheckbox1,
                                             hcPreferenceCheckbox1 + 31) +
                             *new TStatusItem(_("~F1~ Help"), kbF1,
                                              cmHelp) +
                             *new TStatusItem(_("~Space~ select"), kbSpace,
                                              cmEnter) +
                             *new TStatusItem(_("~Esc~ Cancel"), kbEsc,
                                              cmCancel) + *new TStatusDef(100,
                                                                          555)
                             + *new TStatusItem(_("~F1~ Help"), kbF1,
                                                cmHelp) +
                             *new TStatusItem(NULL, kbF10, cmMenu)
#ifdef INTERNAL_DEBUGGER
                             + *new TStatusDef(hcDisassemblerViewer,
                                               hcDisassemblerViewer) +
                             *new TStatusItem(_("~F1~ Help"), kbF1,
                                              cmHelp) +
                             *new TStatusItem(_("~^G~ Goto"), kbCtG,
                                              cmGotoDisass) +
                             *new TStatusItem(_("~Alt+F3~ Close"), kbAlF3,
                                              cmClose) +
                             *new TStatusDef(hcRegister,
                                             hcRegister) +
                             *new TStatusItem(_("~F1~ Help"), kbF1,
                                              cmHelp) +
                             *new TStatusItem(_("~Enter~ Modify register"),
                                              kbEnter,
                                              cmModifyRegister) +
                             *new TStatusItem(_("~Alt+F3~ Close"), kbAlF3,
                                              cmClose)
#endif
                             + *new TStatusDef(0, 0xFFFF)
                             + *new TStatusItem(_("~F1~ Help"), kbF1, cmHelp)
                             + *new TStatusItem(NULL, kbShF1, cmHelpIndex)
                             + *new TStatusItem(_("~F2~ Save"), kbF2,
                                                cmSaveEditor) +
                             *new TStatusItem(_("~F3~ Open"), kbF3,
                                              cmOpenEditor) +
                             *new TStatusItem(_("~F5~ Zoom"), kbF5,
                                              cmZoom) +
                             *new TStatusItem(_("~F6~ Next"), kbF6,
                                              cmNext) +
                             *new TStatusItem(_("~Alt+F9~ Compile"), kbAlF9,
                                              cmCompile) +
                             *new TStatusItem(_("~F10~ Menu"), kbF10,
                                              cmMenu) +
                             *new TStatusItem(_("~Alt+X~ Quit"), kbAlX,
                                              cmQUIT), IDEHint);
  return line;
}

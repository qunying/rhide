/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#define Uses_IDEConst
#include "rhide.h"
#include <rhutils.h>

#define Uses_tvutilFunctions
#include <libtvuti.h>

#define Uses_TCEditor_Commands
#include <ceditor.h>

#undef __CM
#undef __HC
#define __CM(x,y) add_command_constant("cm"#x,cm##x);
#define __HC(x,y) add_help_constant("hc"#x,hc##x);

CONSTRUCTOR_FUNCTION(init_rhide_commands)
{
  __RHIDE_COMMAND_CODES__ __RHIDE_HELP_CODES__
    /*
       Now the commands for the editor 
     */
#undef __BASE
#define __BASE 600
#define __RHIDE_EDITOR_COMMANDS__\
  __CM(ToLower,1)\
  __CM(SelLength,2)\
  __CM(FirstLineInScreen,3)\
  __CM(LastLineInScreen,4)\
  __CM(IndentBlkOne,5)\
  __CM(UnIndentBlkOne,6)\
  __CM(IndentBlk,7)\
  __CM(UnIndentBlk,8)\
  __CM(SearchStart,9)\
  __CM(SearchEnd,10)\
  __CM(ExpandCode,11)\
  __CM(DelPrevWord,12)\
  __CM(EndSelect,13)\
  __CM(SmartUnIndent,14)\
  __CM(SmartIndent,15)\
  __CM(ReplaceSelect,16)\
  __CM(GotoEditorLine,17)\
  __CM(GoBeginBlock,18)\
  __CM(GoEndBlock,19)\
  __CM(MarkLine,20)\
  __CM(MarkWord,21)\
  __CM(MoveBlock,22)\
  __CM(ScrollDown,23)\
  __CM(ScrollUp,24)\
  __CM(PutMark0,25)\
  __CM(PutMark1,26)\
  __CM(PutMark2,27)\
  __CM(PutMark3,28)\
  __CM(PutMark4,29)\
  __CM(PutMark5,30)\
  __CM(PutMark6,31)\
  __CM(PutMark7,32)\
  __CM(PutMark8,33)\
  __CM(PutMark9,34)\
  __CM(GotoMark0,35)\
  __CM(GotoMark1,36)\
  __CM(GotoMark2,37)\
  __CM(GotoMark3,38)\
  __CM(GotoMark4,39)\
  __CM(GotoMark5,40)\
  __CM(GotoMark6,41)\
  __CM(GotoMark7,42)\
  __CM(GotoMark8,43)\
  __CM(GotoMark9,44)\
  __CM(InsertText,45)\
  __CM(ReadBlock,46)\
  __CM(WriteBlock,47)\
  __CM(NewLine,48)\
  __CM(BackSpace,49)\
  __CM(CharLeft,50)\
  __CM(CharRight,51)\
  __CM(WordLeft,52)\
  __CM(WordRight,53)\
  __CM(LineStart,54)\
  __CM(LineEnd,55)\
  __CM(LineUp,56)\
  __CM(LineDown,57)\
  __CM(PageUp,58)\
  __CM(PageDown,59)\
  __CM(TextStart,60)\
  __CM(TextEnd,61)\
  __CM(DelChar,62)\
  __CM(DelWord,63)\
  __CM(DelStart,64)\
  __CM(DelEnd,65)\
  __CM(DelLine,66)\
  __CM(InsMode,67)\
  __CM(StartSelect,68)\
  __CM(HideSelect,69)\
  __CM(IndentMode,70)\
  __CM(UpdateTitle,71)\
  __CM(Cut,72)\
  __CM(Copy,73)\
  __CM(Paste,74)\
  __CM(Undo,75)\
  __CM(SearchAgain,76)\
  __CM(Clear,77)\
  __CM(Replace,78)\
  __CM(Find,79)\
  __CM(SearchOpPar,80)\
  __CM(SearchClPar,81)\
  __CM(SearchOpCor,82)\
  __CM(SearchClCor,83)\
  __CM(ToUpper,84)\
  __CM(Redo,85)\
  __CM(SetLocalOptions,86)\
  __CM(ExpandAllTabs,87)\
  __CM(CompactBuffer,88)\
  __CM(RecordMacro,89)\
  __CM(StopMacro,90)\
  __CM(PlayMacro,91)\
  __CM(SetGlobalOptions,92)\
  __CM(SaveAsUNIX,93)\
  __CM(CopyBlock,94)\
  __CM(SelRectStart,95)\
  __CM(SelRectEnd,96)\
  __CM(SelRectCopy,97)\
  __CM(SelRectPaste,98)\
  __CM(SelRectDel,99)\
  __CM(SelRectCut,100)\
  __CM(SelRectMove,101)\
  __CM(SelRectHide,102)\
  __CM(LastPosCur,103)\
  __CM(ToggleMoveOnPaste,104)\
  __CM(JumpToFunction,105)\
  __CM(ProfileEditor,106)\
  __CM(ColorsChanged,107)\
  __CM(LoadFileUnderCur,108)\
  __CM(SearchComplement,109)\
  __CM(CopyClipWin,110)\
  __CM(PasteClipWin,111)\
  __CM(IntelIndentMode,112)\
  __CM(DelCharClear,113)\
  __CM(Save,114)\
  __CM(SaveAs,115)\
  __CM(ChooseMacro,116)\
  __CM(RepeatMacro,117)\
  __CM(GenCodeForMacro,118)\
  __CM(SelectOn,119)\
  __CM(SelectOff,120)\
  __CM(SaveSameTime,121)\
  __CM(WrapOn,122)\
  __CM(WrapOff,123)\
  __CM(WrapToggle,124)\
  __CM(UpdateCodePage,125)
#undef __CM
#define __CM(x,y) add_command_constant("cmc"#x,cmc##x);
__RHIDE_EDITOR_COMMANDS__}

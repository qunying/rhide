/* Copyright (C) 1996-2000 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#if !defined( __tvutilCommands )
#define __tvutilCommands

#undef __BASE
#define __BASE 500

#define __TVUTIL_COMMAND_CODES__\
  __CM(SelectWindow,1)\
  __CM(DeleteWindow,2)\
  __CM(WindowOpened,3)\
  __CM(RadioItemSelected,4)\
  __CM(CheckBoxItemSelected,5)\
  __CM(FocusWindow,6)\
  __CM(OpenWindow,7)\
  __CM(GotoWindowLine,8)\
  __CM(GotoWindowLineColumn,9)\
  __CM(ShowWindowMsg,10)\
  __CM(NextMsg,11)\
  __CM(PrevMsg,12)\
  __CM(SelectClosedWindow,13)\
  __CM(DeleteClosedWindow,14)

#define __TVUTIL_HELP_CODES__\
  __HC(WindowList,1)\
  __HC(MsgListBox,2)

__TVUTIL_COMMAND_CODES__ __TVUTIL_HELP_CODES__
/*
  Description of some commands:
    cmFocusWindow
      This broadcast is sent, when the app should select a window.
      The name of the file is stored in infoPtr.

   cmOpenWindow
      Same as cmFocusWindow, but the app should open the window,
      if not already on the desktop

   cmGotoWindowLine
      This broadcast is sent only, when one of the above two
      messages was handled successfull. The app should remember,
      which window has handled the above messages.
      The linenumber is stored in infoInt

   cmGotoWindowLineColumn
      Same as cmGotoWindowLine, but now event.message.infoPtr
      points to a 'msg_rec *'

   cmShowWindowMsg
      This is sent, when selecting a window with the above
      broadcasts, then the message in infoPtr should be
      shown.
*/
#endif

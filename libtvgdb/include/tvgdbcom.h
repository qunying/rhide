/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#if !defined( __tvgdbCommands )
#define __tvgdbCommands

#undef __BASE
#define __BASE 550

#define __TVGDB_COMMAND_CODES__\
  __CM(EditWatch,1)\
  __CM(RemoveWatch,2)\
  __CM(Eval,3)\
  __CM(Change,4)\
  __CM(EditBreak,5)\
  __CM(DeleteBreak,6)\
  __CM(NewBreak,7)\
  __CM(DisableBreak,8)\
  __CM(EnableBreak,9)\
  __CM(SelectFunction,10)\
  __CM(FunctionChanged,11)\
  __CM(GotoDisass,12)\
  __CM(ModifyRegister,13)\
  __CM(SelectBreak,14)

#define __TVGDB_HELP_CODES__\
  __HC(WatchDialog,1)\
  __HC(WatchListBox,2)\
  __HC(FunctionInputLine,3)\
  __HC(FunctionListBox,4)\
  __HC(DisassemblerViewer,5)\
  __HC(Register,6)\
  __HC(BreakListDialog,7)\
  __HC(InspectDialog,8)\
  __HC(Inspector,9)

__TVGDB_COMMAND_CODES__ __TVGDB_HELP_CODES__
#endif

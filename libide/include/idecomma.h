/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#if !defined( ideCommands__ )
#define ideCommands

#undef __BASE
#define __BASE 450

#define __IDE_HELP_CODES__\
  __HC(FlagListBox,1)\
  __HC(ProjectWindow,2)\
  __HC(ProjectListBox,3)\
  __HC(IDEEditWindow,4)\
  __HC(IDEFileEditor,5)\
  __HC(NextMsg,6)\
  __HC(PrevMsg,7)\
  __HC(SaveEditor,8)

#define __IDE_COMMAND_CODES__\
  __CM(EditorAnswer,1)\
  __CM(GotoEditorLine,2)\
  __CM(Redraw,3)\
  __CM(SelectProject,4)\
  __CM(EditorModified,5)\
  __CM(TurnSyntaxOff,6)\
  __CM(TurnSyntaxOn,7)\
  __CM(EditorFilenameChanged,8)\
  __CM(SaveEditor,9)

__IDE_COMMAND_CODES__ __IDE_HELP_CODES__
#endif

/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#define Uses_TRect
#define Uses_TEvent
#define Uses_MsgBox
#define Uses_TGroup

#define Uses_TFunctionListBox
#define Uses_TFunctionInputLine
#define Uses_tvgdbCommands
#define Uses_tvgdbFunctions
#include <libtvgdb.h>

TFunctionInputLine::TFunctionInputLine(const TRect & bounds, int aMaxLen):
TEnterInputLine(bounds, aMaxLen)
{
  helpCtx = hcFunctionInputLine;
}

void
TFunctionInputLine::handleEvent(TEvent & event)
{
  TEnterInputLine::handleEvent(event);
  switch (event.what)
  {
    case evCommand:
      switch (event.message.command)
      {
        case cmSelectFunction:
        {
          static char regex[256];
          static char GDB_name[256];
          function_entry *func;

          regex[0] = 0;
          if (inputBox(_("Regular expression to list functions"),
                       _("~E~xpression"), regex, 255) == cmOK)
          {
            if (SelectFunction
                (_("Select a function"), regex, NULL, GDB_name,
                 &func) == cmOK)
            {
              setData(GDB_name);
              message(owner, evBroadcast, cmFunctionChanged, func);
            }
          }
          clearEvent(event);
          break;
        }
        default:
          break;
      }
      break;
    default:
      break;
  }
}

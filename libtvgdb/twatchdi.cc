/* Copyright (C) 1996-2000 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#define Uses_TEvent
#define Uses_TInputLine
#define Uses_TButton
#define Uses_TLabel
#define Uses_TRect
#define Uses_THistory

#define Uses_tvgdbCommands
#define Uses_TWatchDialog
#include <libtvgdb.h>
#include <tvgdbhis.h>

#define Uses_TLButton
#define Uses_tvutilFunctions
#include <libtvuti.h>

#define Uses_TInputLinePiped
#include <settvuti.h>

#include <rhutils.h>

#include <librhgdb.h>
#include <string.h>

TWatchDialog::TWatchDialog(const TRect & bounds, const char *Title,
                           const char *StartVal, int evaluate):
TDialog(bounds, Title), TWindowInit(TWatchDialog::initFrame)
{
  TRect r;

  helpCtx = hcWatchDialog;
  r.a.x = 2;
  r.a.y = 2;
  r.b.x = size.x - 4;
  r.b.y = r.a.y + 1;
  input = new TInputLinePiped(r, 255);
  InitHistoryID(tvgdb_History_Watch_Expression);
  insert(new THistory(TRect(r.b.x, r.a.y, r.b.x + 3, r.b.y), input,
                      tvgdb_History_Watch_Expression));
  if (StartVal)
  {
    char *tmp = string_dup(StartVal);

    input->setData(tmp);
    string_free(tmp);
  }
  insert(input);
  r.move(0, -1);
  insert(new TLabel(r, _("~E~xpression"), input));
  r.move(0, 3);
  if (evaluate)
  {
    result = new TInputLine(r, 255);
    insert(result);
    r.move(0, -1);
    insert(new TLabel(r, _("~R~esult"), result));
    r.move(0, 3);
    newval = new TInputLine(r, 255);
    InitHistoryID(tvgdb_History_Watch_Newvalue);
    insert(new THistory(TRect(r.b.x, r.a.y, r.b.x + 3, r.b.y), newval,
                        tvgdb_History_Watch_Newvalue));
    insert(newval);
    r.move(0, -1);
    insert(new TLabel(r, _("~N~ew value"), newval));
    r.move(0, 3);
  }
  r.b.x = r.a.x + 10;
  r.b.y = r.a.y + 2;
  if (evaluate)
  {
    insert(new TLButton(r, _("E~v~al"), cmEval, bfDefault));
    r.a.x = r.b.x + 2;
    r.b.x = r.a.x + 10;
    insert(new TLButton(r, _("~C~hange"), cmChange, bfNormal));
  }
  else
  {
    insert(new TLButton(r, _("~O~K"), cmOK, bfDefault));
  }
  r.a.x = r.b.x + 2;
  r.b.x = r.a.x + 10;
  insert(new TLButton(r, _("Cancel"), cmCancel, bfNormal));
  input->select();
  options |= ofCentered;
}

void
TWatchDialog::handleEvent(TEvent & event)
{
  char *ret;
  static char *empty_string = "";

  TDialog::handleEvent(event);
  switch (event.what)
  {
    case evCommand:
      switch (event.message.command)
      {
        case cmEval:
        {
          char input_buffer[256];

          input->getData(input_buffer);
          ret = EvaluateWatch(input_buffer, 0);
          if (!ret)
            ret = _("not available");
          result->setData(ret);
          newval->setData(empty_string);
          input->selectAll(True);
          clearEvent(event);
          break;
        }
        case cmChange:
        {
          char input_buffer[256], new_buffer[256], *ret;

          newval->getData(new_buffer);
          input->getData(input_buffer);
          ret = SetValue(input_buffer, new_buffer);
          if (!ret)
            ret = _("could not change");
          result->setData(ret);
          break;
        }
      }
      break;
  }
}

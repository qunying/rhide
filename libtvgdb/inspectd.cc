/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#define Uses_TEvent
#define Uses_TInputLine
#define Uses_TButton
#define Uses_TLabel
#define Uses_TRect
#define Uses_THistory

#define Uses_tvgdbCommands
#define Uses_TInspectDialog
#include <libtvgdb.h>
#include <tvgdbhis.h>

#define Uses_TLButton
#define Uses_tvutilFunctions
#include <libtvuti.h>

#define Uses_TInputLinePiped
#include <settvuti.h>

#include <librhgdb.h>
#include <string.h>

TInspectDialog::TInspectDialog(const TRect & bounds, char *Title,
                               char *StartVal):
TDialog(bounds, Title), TWindowInit(TInspectDialog::initFrame)
{
  TRect r;

  helpCtx = hcInspectDialog;
  r.a.x = 2;
  r.a.y = 2;
  r.b.x = size.x - 4;
  r.b.y = r.a.y + 1;
  input = new TInputLinePiped(r, 255);
  InitHistoryID(tvgdb_History_Inspect_Expression);
  insert(new THistory(TRect(r.b.x, r.a.y, r.b.x + 3, r.b.y), input,
                      tvgdb_History_Inspect_Expression));
  if (StartVal)
  {
    input->setData(StartVal);
  }
  insert(input);
  r.move(0, -1);
  insert(new TLabel(r, _("~E~xpression"), input));
  r.move(0, 3);

  r.b.x = r.a.x + 10;
  r.b.y = r.a.y + 2;
  insert(new TLButton(r, _("~I~nspect"), cmOK, bfDefault));
  r.a.x = r.b.x + 2;
  r.b.x = r.a.x + 10;
  insert(new TLButton(r, _("Cancel"), cmCancel, bfNormal));
  input->select();
  options |= ofCentered;
}


void
TInspectDialog::handleEvent(TEvent & event)
{
  TDialog::handleEvent(event);
}

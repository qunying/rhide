/* Copyright (C) 1996-2000 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#define Uses_MsgBox
#define Uses_TProgram
#define Uses_TDeskTop
#define Uses_TLabel
#define Uses_THistory
#define Uses_TDialog
#define Uses_IDEConst
#include "rhide.h"
#include "rhidehis.h"

#define Uses_TProject
#include <libide.h>

#define Uses_TLButton
#define Uses_tvutilFunctions
#include <libtvuti.h>

#include <rhutils.h>

#define Uses_TCEditor_External
#define Uses_TCEditor_Commands
#include <ceditor.h>
#include <calcu.h>

#define Uses_TInputLinePiped
#define Uses_TSOSSortedListBox
#include <settvuti.h>

#include <string.h>
#include <errno.h>

unsigned doEditDialog(int dialog, va_list first);

unsigned
IDEdoEditDialog(int dialog, ...)
{
  va_list args;

  va_start(args, dialog);
  unsigned ret = doEditDialog(dialog, args);

  va_end(args);
  return ret;
}

extern "C" int eval(char *mit, char **out);

/* I had to rewrite this to get the help-button available */

void
ShowCalculator()
{
  TRect r(10, 2, 72, 11);
  TDialog *dialog;
  TInputLine *input;
  TInputLine *result;
  ushort retval;
  char *startval;

  dialog = new TDialog(r, _("Calculator"));
  dialog->helpCtx = hcCalculatorDialog;
  r.a.x = 2;
  r.a.y = 2;
  r.b.x = dialog->size.x - 5;
  r.b.y = r.a.y + 1;
  input = new TInputLinePiped(r, 255);
  startval = WUC();
  if (startval)
  {
    input->setData(startval);
    string_free(startval);
  }
  dialog->insert(input);
  InitHistoryID(RHIDE_History_Calculator);
  dialog->insert(new THistory(TRect(r.b.x, r.a.y, r.b.x + 3, r.b.y), input,
                              RHIDE_History_Calculator));
  r.move(0, -1);
  dialog->insert(new TLabel(r, _("~E~xpression"), input));
  r.move(0, 3);
  result = new TInputLinePiped(r, 255, tilpNoPipe | tilpNoPaste);
  dialog->insert(result);
  r.move(0, -1);
  dialog->insert(new TLabel(r, _("~R~esult"), result));
  r.move(0, 3);
  r.b.x = r.a.x + 12;
  r.b.y = r.a.y + 2;
  dialog->insert(new TLButton(r, _("E~v~al"), cmOK, bfDefault));
  r.a.x = r.b.x + 2;
  r.b.x = r.a.x + 12;
  dialog->insert(new TLButton(r, _("Cancel"), cmCancel, bfNormal));
  r.a.x = r.b.x + 2;
  r.b.x = r.a.x + 12;
  dialog->insert(new TLButton(r, _("~H~elp"), cmHelp, bfNormal));
  input->select();
  dialog->options |= ofCentered;
  TProgram::deskTop->insert(dialog);
  do
  {
    dialog->setState(sfModal, True);
    retval = dialog->execute();
    if (retval == cmOK)
    {
      char input_buffer[256], *ret;

      input->getData(input_buffer);
      int err = eval(input_buffer, &ret);

      if (err)
      {
        sprintf(input_buffer, _("Error in expression (%d)"), err);
        messageBox(input_buffer, mfError | mfOKButton);
      }
      result->setData(ret);
      input->selectAll(True);
    }
  }
  while (retval == cmOK);
  destroy(dialog);
}

/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#include <string.h>
#define Uses_MsgBox
#define Uses_TDialog
#define Uses_TProgram
#define Uses_TDeskTop
#define Uses_TSortedCollection

#define Uses_TFunctionListBox
#define Uses_tvgdbFunctions
#include <libtvgdb.h>
#include <librhgdb.h>

class TFuncDialog:public TDialog
{
public:
  TFuncDialog(const char *aTitle, const char *regex);
  virtual void handleEvent(TEvent & event);
  TFunctionListBox *list;
};

TFuncDialog::TFuncDialog(const char *aTitle, const char *regex):
TDialog(TRect(0, 0, 70, 22), aTitle), TWindowInit(TFuncDialog::initFrame)
{
  TRect r = TProgram::deskTop->getExtent();
  TScrollBar *hsb;

  r.grow(-1, -1);
  locate(r);
  options |= ofCentered;
  r = getExtent();
  r.grow(-1, -1);
  list = new TFunctionListBox(r, 1, standardScrollBar(sbVertical), regex);
  hsb = standardScrollBar(sbHorizontal | sbHandleKeyboard);
  list->hScrollBar = hsb;
  insert(list);
}

void
TFuncDialog::handleEvent(TEvent & event)
{
  TDialog::handleEvent(event);
}

ushort
SelectFunction(const char *title, const char *regex, char *full_name,
               char *GDB_name, function_entry ** func)
{
  TFuncDialog *dialog = new TFuncDialog(title, regex);
  ushort result = TProgram::deskTop->execView(dialog);

  if (result == cmOK)
  {
    int foc = dialog->list->focused;
    function_entry *_func = (function_entry *) dialog->list->list()->at(foc);

    if (full_name)
      dialog->list->getText(full_name, foc, 1024);
    if (GDB_name)
      strcpy(GDB_name, _func->linkage_name);
    if (func)
      *func = _func;
  }
  destroy(dialog);
  return result;
}

/* Copyright (C) 1996-2000 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#define Uses_TProgram
#define Uses_TDeskTop
#define Uses_TScrollBar
#define Uses_TDialog
#define Uses_TStaticText
#define Uses_TButton
#define Uses_TLabel
#define Uses_TSItem
#define Uses_THistory

#define Uses_TIntInputLine
#define Uses_TEnterInputLine
#define Uses_TEnterRadioButtons
#define Uses_tvutilCommands
#define Uses_TLButton
#define Uses_tvutilFunctions
#include <libtvuti.h>

#define Uses_TBreakListBox
#define Uses_tvgdbCommands
#define Uses_TFunctionInputLine
#define Uses_tvgdbFunctions
#include <libtvgdb.h>
#include <tvgdbhis.h>

#include <librhgdb.h>

#include <string.h>
#include <stdlib.h>

class TBreakListDialog:public TDialog
{
public:
  TBreakListDialog(const TRect & r, const char *Title);
  virtual void handleEvent(TEvent &);
  TBreakListBox *bl;
  TButton *enableButton, *disableButton;
};

TBreakListDialog::TBreakListDialog(const TRect & bounds, const char *Title):
TDialog(bounds, Title), TWindowInit(TBreakListDialog::initFrame)
{
  TScrollBar *sb;
  TRect r, rr;
  int size;
  char tmp[256], *temp, *_tmp;

  helpCtx = hcBreakListDialog;
  r = getExtent();
  r.grow(-1, -2);
  r.b.y -= 2;
  sb = standardScrollBar(sbVertical | sbHandleKeyboard);
  rr = sb->getBounds();
  rr.a.y = r.a.y;
  rr.b.y = r.b.y;
  sb->changeBounds(rr);
  bl = new TBreakListBox(r, sb);
  insert(bl);
  r = getExtent();
  r.grow(-1, -1);
  r.b.y = r.a.y + 1;
  bl->filesize = 20;
  bl->linesize = 20;
  bl->countsize = 10;
  size = r.b.x - r.a.x + 1;
  bl->condsize = (size) -
    (bl->filesize + 1) - (bl->linesize + 1) - (bl->countsize + 1) - 3;
  memset(tmp, ' ', size);
  tmp[size] = 0;
  temp = tmp + 2;
  _tmp = _("File");
  memcpy(temp, _tmp, strlen(_tmp));
  temp += bl->filesize;
  *temp++ = bl->delemiter;
  _tmp = _("Line/Function");
  memcpy(temp, _tmp, strlen(_tmp));
  temp += bl->linesize;
  *temp++ = bl->delemiter;
  _tmp = _("Condition");
  memcpy(temp, _tmp, strlen(_tmp));
  temp += bl->condsize;
  *temp++ = bl->delemiter;
  _tmp = _("Count");
  memcpy(temp, _tmp, strlen(_tmp));
  insert(new TStaticText(r, tmp));
  r = getExtent();
  r.grow(0, -1);
  r.a.y = r.b.y - 2;
  r.a.x = 1;
  r.b.x = r.a.x + 10;
  insert(new TLButton(r, _("~M~odify"), cmEditBreak, bfNormal));
  r.a.x = r.b.x + 1;
  r.b.x = r.a.x + 10;
  insert(new TLButton(r, _("~N~ew"), cmNewBreak, bfNormal));
  r.a.x = r.b.x + 1;
  r.b.x = r.a.x + 10;
  insert(new TLButton(r, _("~D~elete"), cmDeleteBreak, bfNormal));
  r.a.x = r.b.x + 1;
  r.b.x = r.a.x + 10;
  enableButton = new TLButton(r, _("~E~nable"), cmEnableBreak, bfNormal);
  insert(enableButton);
  r.a.x = r.b.x + 1;
  r.b.x = r.a.x + 10;
  disableButton = new TLButton(r, _("D~i~sable"), cmDisableBreak, bfNormal);
  insert(disableButton);
  r.a.x = r.b.x + 1;
  r.b.x = r.a.x + 10;
  insert(new TLButton(r, _("~S~how"), cmSelectBreak, bfDefault));
  bl->select();
}

void
TBreakListDialog::handleEvent(TEvent & event)
{
  TDialog::handleEvent(event);
  switch (event.what)
  {
    case evCommand:
      switch (event.message.command)
      {
        case cmDisableBreak:
        case cmEnableBreak:
        case cmNewBreak:
        case cmDeleteBreak:
        case cmEditBreak:
        case cmSelectBreak:
          endModal(event.message.command);
          clearEvent(event);
          break;
        default:
          break;
      }
      break;
    case evBroadcast:
      switch (event.message.command)
      {
        case cmListItemFocused:
          int foc;
          BreakPoint *bp;

          foc = ((TBreakListBox *) event.message.infoPtr)->focused;
          bp = GetBreakPoint(foc);
          if (BREAK_ENABLED(bp))
          {
            disableCommand(cmEnableBreak);
            enableCommand(cmDisableBreak);
          }
          else
          {
            enableCommand(cmEnableBreak);
            disableCommand(cmDisableBreak);
          }
          break;
        default:
          break;
      }
    default:
      break;
  }
}

class TBreakEditDialog:public TDialog
{
public:
  TBreakEditDialog(const char *);
  virtual void handleEvent(TEvent &);
  void update();
  TEnterRadioButtons *Radio;
  TFunctionInputLine *Function;
  TEnterInputLine *File;
  TIntInputLine *Line;
  TEnterInputLine *Condition;
  TIntInputLine *Count;
};

void
TBreakEditDialog::update()
{
  ushort rdata;

  Radio->getData(&rdata);
  if (rdata == 0)
  {
    File->options |= ofSelectable;
    Line->options |= ofSelectable;
    Function->options &= ~ofSelectable;
  }
  else
  {
    File->options &= ~ofSelectable;
    Line->options &= ~ofSelectable;
    Function->options |= ofSelectable;
  }
}

void
TBreakEditDialog::handleEvent(TEvent & event)
{
  TDialog::handleEvent(event);
  switch (event.what)
  {
    case evBroadcast:
      switch (event.message.command)
      {
        case cmRadioItemSelected:
          if (event.message.infoPtr != Radio)
            break;
          update();
          clearEvent(event);
          break;
        case cmFunctionChanged:
        {
          function_entry *func = (function_entry *) event.message.infoPtr;

          File->setData(func->file_name);
          Line->setData(&func->line_number);
          update();
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

TBreakEditDialog::TBreakEditDialog(const char *Title):
TDialog(TRect(0, 0, 51, 14), Title), TWindowInit(TBreakEditDialog::initFrame)
{
  char *i1 = _("File/L~i~ne");
  char *i2 = _("Fu~n~ction");
  int l = max(cstrlen(i1), cstrlen(i2));
  TRect r(30, 2, 30 + l + 6, 4);

  Radio = new TEnterRadioButtons(r, new TSItem(i1, new TSItem(i2, NULL)));
  r.move(0, -1);
  r.b.y = r.a.y + 1;
  insert(new TLabel(r, _("~T~ype"), Radio));
  insert(Radio);
  r = TRect(2, 3, 25, 4);
  Function = new TFunctionInputLine(r, 256);
  InitHistoryID(tvgdb_History_Break_Function);
  insert(new THistory(TRect(r.b.x, r.a.y, r.b.x + 3, r.b.y), Function,
                      tvgdb_History_Break_Function));
  r.move(0, -1);
  insert(new TLabel(r, _("F~u~nction"), Function));
  insert(Function);
  r.move(0, 3);
  File = new TEnterInputLine(r, 256);
  InitHistoryID(tvgdb_History_Break_File);
  insert(new THistory(TRect(r.b.x, r.a.y, r.b.x + 3, r.b.y), File,
                      tvgdb_History_Break_File));
  r.move(0, -1);
  insert(new TLabel(r, _("~F~ilename"), File));
  insert(File);
  r.move(0, 1);
  r.a.x = r.b.x + 5;
  r.b.x = r.a.x + 15;
  Line = new TIntInputLine(r, 10);
  InitHistoryID(tvgdb_History_Break_Line);
  insert(new THistory(TRect(r.b.x, r.a.y, r.b.x + 3, r.b.y), Line,
                      tvgdb_History_Break_Line));
  r.move(0, -1);
  insert(new TLabel(r, _("~L~ine"), Line));
  insert(Line);
  r.a.x = 2;
  r.move(0, 3);
  r.b.x = r.a.x + 45;
  Condition = new TEnterInputLine(r, 512);
  InitHistoryID(tvgdb_History_Break_Condition);
  insert(new THistory(TRect(r.b.x, r.a.y, r.b.x + 3, r.b.y), Condition,
                      tvgdb_History_Break_Condition));
  r.move(0, -1);
  insert(new TLabel(r, _("Con~d~ition"), Condition));
  insert(Condition);
  r.move(0, 3);
  r.b.x = r.a.x + 15;
  Count = new TIntInputLine(r, 10);
  InitHistoryID(tvgdb_History_Break_Count);
  insert(new THistory(TRect(r.b.x, r.a.y, r.b.x + 3, r.b.y), Count,
                      tvgdb_History_Break_Count));
  r.move(0, -1);
  insert(new TLabel(r, _("Coun~t~"), Count));
  insert(Count);
  r.move(0, 3);
  r.b.y = r.a.y + 2;
  r.b.x = r.a.x + 15;
  insert(new TLButton(r, _("~O~K"), cmOK, bfNormal));
  r.move(20, 0);
  insert(new TLButton(r, _("Cancel"), cmCancel, bfNormal));
  options |= ofCentered;
  Radio->select();
}

static void
EditBreak(int number)
{
  ushort type, control;
  char buffer[512], *_buffer, *tit;
  BreakPoint *bp;
  TBreakEditDialog *dialog;

  if (number >= 0)
  {
    bp = GetBreakPoint(number);
    tit = _("Edit Breakpoint");
  }
  else
  {
    bp = NULL;
    tit = _("New Breakpoint");
  }
  dialog = new TBreakEditDialog(tit);
  if (bp)
  {
    if (BREAK_LINE(bp))
      type = 0;
    else
      type = 1;
    if (bp->filename)
      dialog->File->setData(bp->filename);
    dialog->Line->setData(&bp->line_number);
    if (BREAK_FUNCTION(bp))
      dialog->Function->setData(bp->function);
    dialog->Radio->setData(&type);
    if (BREAK_CONDITION(bp))
    {
      dialog->Condition->setData(bp->condition);
    }
    if (BREAK_COUNT(bp))
    {
      dialog->Count->setData(&bp->count);
    }
  }
  dialog->update();
  control = TProgram::deskTop->execView(dialog);
  if (control == cmOK)
  {
    if (!bp)
    {
      bp = (BreakPoint *) alloca(sizeof(BreakPoint));
      bp->type = B_ENABLED;
      bp->line_number = bp->count = 0;
      bp->filename = bp->function = bp->condition = NULL;
      bp->number = -1;
    }
    dialog->Radio->getData(&type);
    if (bp->function)
      free(bp->function);
    bp->function = NULL;
    if (type == 1)
    {
      dialog->Function->getData(buffer);
      bp->function = strdup(buffer);
    }
    if (bp->filename)
      free(bp->filename);
    dialog->File->getData(buffer);
    bp->filename = strdup(buffer);
    dialog->Line->getData(&bp->line_number);
    dialog->Condition->getData(buffer);
    _buffer = buffer;
    while (*_buffer == ' ')
      _buffer++;
    if (bp->condition)
    {
      free(bp->condition);
      bp->condition = NULL;
    }
    if (*_buffer)
      bp->condition = strdup(_buffer);
    dialog->Count->getData(&bp->count);
    EditBreakPoint(bp, number);
  }
  destroy(dialog);
}

void
BreakDialog()
{
  TBreakListDialog *dialog;
  BreakPoint *bp = NULL;
  TRect r = TProgram::deskTop->getExtent();
  ushort control;

  dialog = new TBreakListDialog(r, _("BreakPoints"));
  TProgram::deskTop->insert(dialog);
  if (dialog->bl->range > 0)
    dialog->bl->focusItem(0);
  do
  {
    if (dialog->bl->range > 0)
    {
      dialog->enableCommand(cmEditBreak);
      dialog->enableCommand(cmDeleteBreak);
    }
    else
    {
      dialog->disableCommand(cmEditBreak);
      dialog->disableCommand(cmDeleteBreak);
      dialog->disableCommand(cmEnableBreak);
      dialog->disableCommand(cmDisableBreak);
    }
    dialog->setState(sfModal, True);
    control = dialog->execute();
    switch (control)
    {
      case cmSelectBreak:
        bp = GetBreakPoint(dialog->bl->focused);
        break;
      case cmEditBreak:
        EditBreak(dialog->bl->focused);
        dialog->bl->update();
        break;
      case cmNewBreak:
        EditBreak(-1);
        dialog->bl->update();
        break;
      case cmDeleteBreak:
        if (dialog->bl->range > 0)
        {
          RemoveBreakPoint(dialog->bl->focused);
          dialog->bl->update();
        }
        break;
      case cmDisableBreak:
      case cmEnableBreak:
        BreakPoint * bp;
        int foc;

        if (dialog->bl->range > 0)
        {
          foc = dialog->bl->focused;
          bp = GetBreakPoint(dialog->bl->focused);
          if (BREAK_ENABLED(bp))
            DisableBreakPoint(foc);
          else
            EnableBreakPoint(foc);
          dialog->bl->update();
          dialog->bl->focusItem(foc);
        }
        break;
      default:
        break;
    }
  }
  while (control != cmCancel && control != cmSelectBreak);
  TProgram::deskTop->remove(dialog);
  destroy(dialog);
  if (bp && bp->filename
      && message(TProgram::application, evBroadcast, cmOpenWindow,
                 bp->filename))
  {
    message(TProgram::application, evBroadcast, cmGotoWindowLine,
            (void *) bp->line_number);
  }
}

void
DeleteAllBreakPoints()
{
  int i, count = BreakPointCount();

  for (i = count; i > 0; i--)
  {
    RemoveBreakPoint(i - 1);
  }
}

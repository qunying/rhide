/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#include <string.h>
#define Uses_MsgBox
#define Uses_TRect
#define Uses_TProgram
#define Uses_TDeskTop
#define Uses_THistory
#define Uses_TDialog
#define Uses_TInputLine
#define Uses_TButton
#define Uses_TLabel
#define Uses_TValidator
#include <tv.h>

#define Uses_tvutilFunctions
#include <libtvuti.h>

#include <stdio.h>

static TInputLine *
DefaultCreateInputLine(const TRect & rect, int aMaxLen)
{
  return new TInputLine(rect, aMaxLen);
}

TInputLine *(*CreateInputLine) (const TRect & rect, int aMaxLen) =

  &DefaultCreateInputLine;

static TRect
makeRect()
{
  TRect r(0, 0, 60, 15);

  r.move((TProgram::deskTop->size.x - r.b.x) / 2,
         (TProgram::deskTop->size.y - r.b.y) / 2);
  return r;
}

ushort
BigmessageBox(const char *msg, ushort aOptions)
{
  return messageBoxRect(makeRect(), msg, aOptions);
}

ushort
BigmessageBox(ushort aOptions, const char *fmt, ...)
{
  va_list argptr;

  va_start(argptr, fmt);
  char msg[1000];

  vsprintf(msg, fmt, argptr);
  va_end(argptr);
  return messageBoxRect(makeRect(), msg, aOptions);
}

ushort
BiginputBox(const char *Title, const char *aLabel, char *s, int limit)
{
  TRect r(0, 0, 60, 8);

  r.move((TProgram::deskTop->size.x - r.b.x) / 2,
         (TProgram::deskTop->size.y - r.b.y) / 2);
  return inputBoxRect(r, Title, aLabel, s, limit);
}

ushort
HistinputBoxRect(const TRect & bounds,
                 const char *Title,
                 const char *aLabel, char *s, int limit, int hist_id)
{
  TDialog *dialog;
  TView *control;
  TRect r;
  ushort c;

  dialog = new TDialog(bounds, Title);

  r = TRect(4 + strlen(aLabel), 2, dialog->size.x - 4, 3);
  control = (*CreateInputLine) (r, limit);
  dialog->insert(control);

  r = TRect(dialog->size.x - 4, 2, dialog->size.x - 1, 3);
  InitHistoryID(hist_id);
  dialog->insert(new THistory(r, (TInputLine *) control, hist_id));

  r = TRect(2, 2, 3 + strlen(aLabel), 3);
  dialog->insert(new TLabel(r, aLabel, control));

  r = TRect(dialog->size.x - 24, dialog->size.y - 4,
            dialog->size.x - 14, dialog->size.y - 2);
  dialog->insert(new TButton(r, _("~O~K"), cmOK, bfDefault));

  r.a.x += 12;
  r.b.x += 12;
  dialog->insert(new TButton(r, _("Cancel"), cmCancel, bfNormal));

  r.a.x += 12;
  r.b.x += 12;
  dialog->selectNext(False);
  dialog->setData(s);
  c = TProgram::deskTop->execView(dialog);
  if (c != cmCancel)
    dialog->getData(s);
  TObject::destroy(dialog);
  return c;
}

ushort
HistinputBox(const char *title, const char *aLabel, char *s,
             int limit, int hist_id)
{
  TRect r(0, 0, 60, 8);

  r.move((TProgram::deskTop->size.x - r.b.x) / 2,
         (TProgram::deskTop->size.y - r.b.y) / 2);
  return HistinputBoxRect(r, title, aLabel, s, limit, hist_id);
}

static ushort
ValidInputBoxRect(const TRect & bounds,
                  const char *Title,
                  const char *aLabel,
                  char *s, uchar limit, TValidator * validator)
{
  TDialog *dialog;
  TView *control;
  TRect r;
  ushort c;

  dialog = new TDialog(bounds, Title);

  r = TRect(4, 3, dialog->size.x - 3, 4);
//    r = TRect( 4 + strlen(aLabel), 2, dialog->size.x - 3, 3 );
  control = new TInputLine(r, limit);
  ((TInputLine *) control)->SetValidator(validator);
  dialog->insert(control);

  r = TRect(2, 2, 3 + strlen(aLabel), 3);
  dialog->insert(new TLabel(r, aLabel, control));

  r = TRect(dialog->size.x - 24, dialog->size.y - 4,
            dialog->size.x - 14, dialog->size.y - 2);
  dialog->insert(new TButton(r, _("~O~K"), cmOK, bfDefault));

  r.a.x += 12;
  r.b.x += 12;
  dialog->insert(new TButton(r, _("Cancel"), cmCancel, bfNormal));

  r.a.x += 12;
  r.b.x += 12;
  dialog->selectNext(False);
  dialog->setData(s);
  c = TProgram::deskTop->execView(dialog);
  if (c != cmCancel)
    dialog->getData(s);
  TObject::destroy(dialog);
  return c;
}

ushort
ValidInputBox(const char *Title, const char *aLabel, char *s, int limit,
              TValidator * validator)
{
  TRect r(0, 0, 60, 9);

  r.move((TProgram::deskTop->size.x - r.b.x) / 2,
         (TProgram::deskTop->size.y - r.b.y) / 2);
  return ValidInputBoxRect(r, Title, aLabel, s, limit, validator);
}

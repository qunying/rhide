/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#define Uses_TSItem
#define Uses_TDialog
#define Uses_TScreen
#define Uses_TLabel
#define Uses_TRangeValidator
#define Uses_TEvent
#define Uses_TProgram
#define Uses_TDeskTop
#define Uses_MsgBox
#include <tv.h>
#include <rhgdb.h>

#define Uses_TEnterRadioButtons
#define Uses_TEnterInputLine
#define Uses_TLButton
#define Uses_TEnterCheckBoxes
#define Uses_tvutilCommands
#include <libtvuti.h>

#include <librhgdb.h>

#include <stdio.h>

class TEnvironmentDialog:public TDialog
{
public:
  TEnvironmentDialog();
  virtual void handleEvent(TEvent &);

  TInputLine *tab;
#ifdef __DJGPP__
  TRadioButtons *radio;
  TInputLine *usermode;
#endif
  TCheckBoxes *check;
};

TEnvironmentDialog::TEnvironmentDialog():
TDialog(TRect(0, 0, 65, 17), _("Environment options")),
TWindowInit(TEnvironmentDialog::initFrame)
{
  TRect
    r;
  char *
    tmp;
  char
    tabstring[10];

#ifdef __DJGPP__
  ushort
    radiodata, oldmode;

  r = TRect(2, 2, 2 + 20, 2 + 13);
  radio = new TEnterRadioButtons(r, new TSItem(("8~0~x25"),
                                               new TSItem(("80x~2~8"),
                                                          new
                                                          TSItem(("~8~0x35"),
                                                                 new
                                                                 TSItem(
                                                                        ("80x~4~0"),
                                                                        new
                                                                        TSItem
                                                                        (
                                                                         ("80x4~3~"),
                                                                         new
                                                                         TSItem
                                                                         (
                                                                          ("80x~5~0"),
                                                                          new
                                                                          TSItem
                                                                          (_
                                                                           ("other"),
                                                                           new
                                                                           TSItem
                                                                           (
                                                                            ("80x30"),
                                                                            new
                                                                            TSItem
                                                                            (
                                                                             ("80x34"),
                                                                             new
                                                                             TSItem
                                                                             (
                                                                              ("90x30"),
                                                                              new
                                                                              TSItem
                                                                              (
                                                                               ("90x34"),
                                                                               new
                                                                               TSItem
                                                                               (
                                                                                ("94x30"),
                                                                                new
                                                                                TSItem
                                                                                (
                                                                                 ("94x34"),
                                                                                 NULL))))))))))))));
  tmp = _("~S~creen mode");
  insert(new
         TLabel(TRect(r.a.x, r.a.y - 1, r.a.x + cstrlen(tmp) + 1, r.a.y), tmp,
                radio));
  insert(radio);
  r.a.y = r.b.y;
  r.b.y = r.a.y + 1;
  r.b.x = r.a.x + 10;
  usermode = new TEnterInputLine(r, 6);
  insert(usermode);
  radiodata = 0;
  oldmode = TScreen::screenMode;
  if ((oldmode & 0xFF) == 3)
    radiodata += (oldmode >> 8);
  else
    radiodata = 6;
  sprintf(tabstring, "0x%x", oldmode);
  radio->setData(&radiodata);
  usermode->setData(tabstring);
  if (radiodata == 6)
    usermode->options |= ofSelectable;
  else
    usermode->options &= ~ofSelectable;
#endif
  sprintf(tabstring, "%d", tabsize);
  tmp = _("~T~absize");
  r.a.x = 2 + 20 + 2 + cstrlen(tmp) + 2;
  r.b.x = r.a.x + 5;
  r.a.y = 2;
  r.b.y = r.a.y + 1;
  tab = new TEnterInputLine(r, 4);
  tab->setData(tabstring);
  tab->SetValidator(new TRangeValidator(1, 100));
  insert(tab);
  r.a.x = 2 + 20 + 2;
  r.b.x = r.a.x + cstrlen(tmp) + 1;
  insert(new TLabel(r, tmp, tab));

  r.move(0, 1);
  r.b.x = r.a.x + 40;
  r.b.y = r.a.y + 2;
  uint16
    check_data = 0;

  check = new TEnterCheckBoxes(r, new TSItem(_("Show G~D~B commands"),	// d
                                             new TSItem(_("U~s~e dual display"),	// s
                                                        NULL)));
  if (verbose_gdb_commands)
    check_data |= (1 << 0);
  if (use_dual_display)
    check_data |= (1 << 1);
  check->setData(&check_data);
  insert(check);

  r.move(0, 3);
  r.b.x = r.a.x + 12;
  r.b.y = r.a.y + 2;
  insert(new TLButton(r, _("~O~K"), cmOK, bfNormal));
  r.move(0, 3);
  r.b.x = r.a.x + 12;
  insert(new TLButton(r, _("Cancel"), cmCancel, bfNormal));

  options |= ofCentered;
#ifdef __DJGPP__
  radio->select();
#else
  check->select();
#endif
}

void
TEnvironmentDialog::handleEvent(TEvent & event)
{
  TDialog::handleEvent(event);
  switch (event.what)
  {
    case evBroadcast:
      switch (event.message.command)
      {
#ifdef __DJGPP__
        case cmRadioItemSelected:
          if (radio == event.message.infoPtr)
          {
            ushort d;

            ((TRadioButtons *) event.message.infoPtr)->getData(&d);
            if (d == 6)
              usermode->options |= ofSelectable;
            else
              usermode->options &= ~ofSelectable;
            clearEvent(event);
          }
          break;
#endif
        default:
          break;
      }
      break;
    default:
      break;
  }
}

void
Preferences()
{
  TEnvironmentDialog *dialog;
  int do_it;

#ifdef __DJGPP__
  ushort radiodata;
  int oldmode = TScreen::screenMode;
#endif
  dialog = new TEnvironmentDialog();
  do_it = 1;
  while (do_it && TProgram::deskTop->execView(dialog) == cmOK)
  {
    char tabstring[10];

    do_it = 0;
    uint16 check_data;

    dialog->check->getData(&check_data);
    if (check_data & (1 << 0))
      verbose_gdb_commands = 1;
    else
      verbose_gdb_commands = 0;
    if (check_data & (1 << 1))
      use_dual_display = 1;
    else
      use_dual_display = 0;
    dialog->tab->getData(tabstring);
    sscanf(tabstring, "%d", &tabsize);
#ifdef __DJGPP__
    ushort rdata;

    dialog->radio->getData(&rdata);
    radiodata = 0x003;
    if (rdata == 6)
    {
      int mode;

      dialog->usermode->getData(tabstring);
      sscanf(tabstring, "%d", &mode);
      if (mode == 0)
        sscanf(tabstring, "0x%x", &mode);
      if (mode == 0)
        sscanf(tabstring, "0x%X", &mode);
      radiodata = mode;
    }
    else
      radiodata += (rdata << 8);
    if (oldmode != radiodata)
      TProgram::application->setScreenMode(radiodata);
#endif
    repaint();
#ifdef __DJGPP__
    if (TScreen::screenMode != radiodata)
    {
      messageBox(mfError | mfOKButton,
                 _("This video mode (0x%03x) is not supported by the "
                   "Turbo Vision library"), radiodata);
      dialog->usermode->select();
      do_it = 1;
    }
#endif
  }
  destroy(dialog);
}

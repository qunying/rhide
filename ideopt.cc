/* Copyright (C) 1996-2000 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#define Uses_TDirList
#define Uses_TParamList
#define Uses_TOptions
#define Uses_TProject
#define Uses_TDepCollection

#define Uses_TProgram
#define Uses_editCommands
#define Uses_TDeskTop
#define Uses_MsgBox
#define Uses_TDialog
#define Uses_TEvent
#define Uses_THistory
#define Uses_TCheckBoxes
#define Uses_TSItem
#define Uses_TInputLine
#define Uses_TRangeValidator
#define Uses_TLabel
#define Uses_TRadioButtons
#define Uses_TScrollBar
#define Uses_TFlagEntry
#define Uses_TFlagCollection
#define Uses_TFlagListBox
#define Uses_TStringCollection
#define Uses_TScreen
#define Uses_TKeys
#define Uses_IDEConst
#define Uses_TProjectWindow
#define Uses_TProjectListBox
#define Uses_ideCommands
#define Uses_ideFunctions
#define Uses_TFileDialog
#define Uses_TIDEFileEditor
#include <libide.h>

#define Uses_TSCollection
#define Uses_TLButton
#define Uses_tvutilFunctions
#define Uses_TEnterInputLine
#define Uses_TEnterRadioButtons
#define Uses_TEnterCheckBoxes
#define Uses_TIntInputLine
#define Uses_tvutilCommands
#include <libtvuti.h>

#define Uses_TCEditor
#include <ceditor.h>

#include <rhutils.h>

#define Uses_IDEConst
#include "rhide.h"
#include "rhidehis.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#ifdef INTERNAL_DEBUGGER
#include <librhgdb.h>
#endif

void
InitOptions()
{
  DefaultOptFlags();
  DefaultDebugFlags();
  {
    int i, count = Options.debug_flags->getCount();

    for (i = 0; i < count; i++)
    {
      TFlagEntry *f = (TFlagEntry *) Options.debug_flags->at(i);

      if (strcmp(f->GetControlString(), "-g%d?") == 0)
      {
        f->activated = True;
        break;
      }
    }
  }
  DefaultWarnFlags();
  DefaultCXXFlags();
  DefaultCFlags();
  DefaultPascalFlags();
  DefaultFpcFlags();
  DefaultReservedWords();
  DefaultGPCReservedWords();
  DefaultFPCReservedWords();
  DefaultUserWords();
  DefaultFortranFlags();

  AllDeps = 0;
  CreateBackupFiles = 0;
  ShowSyntax = 1;
  UseDualDisplay = 1;
//  AutoIndent = 1;
//  OverWrite = 0;
  ShowStderr = 1;
  ShowStdout = 0;
  ShowWhatDoing = 1;
  ShowMem = 1;
  NoFileCache = 0;
//  CAutoIndent = 0;
//  UseRealTabs = 0;

//  NoPersistentBlocks = 0;
//  ShowColumnCursor = 0;
//  ShowRowCursor = 0;
  IntenseMode = 0;
  VerboseGDB = 0;
  NoShadows = 0;
//  TransparentBlocks = 0;

  TCEditor::staticTabSize = 8;
  Project.screen_mode = TScreen::screenMode;

  SaveTextPalette = 0;
}

ushort EditParamList(TParamList * paramlist, const char *title, int hist_id)
{
  return editParamList(paramlist, title, hist_id);
}

void
EditDirList(TDirList * dirlist, const char *title, int hist_id)
{
  editDirList(dirlist, title, hist_id);
}

static void
Flags(const char *title, TFlagCollection * &coll, ushort ctx)
{
  TDialog *dialog;
  TFlagListBox *liste;
  TFlagCollection *_coll;
  TScrollBar *scrollbar;
  int i;
  ushort control;
  TRect r;

  dialog = new TDialog(TRect(0, 0, 70, 20), title);
  dialog->options |= ofCentered;
  dialog->helpCtx = ctx;
  scrollbar = new TScrollBar(TRect(69, 1, 70, 16));
  liste = new TFlagListBox(TRect(1, 1, 69, 16), 1, scrollbar);

  liste->helpCtx = 0;

  dialog->insert(scrollbar);
  _coll = new TFlagCollection();
  for (i = 0; i < coll->getCount(); i++)
  {
    _coll->insert(new TFlagEntry((TFlagEntry *) coll->at(i)));
  }
  liste->newList(_coll);
  dialog->insert(liste);
  r = TRect(5, 17, 15, 19);
  dialog->insert(new TLButton(r, _("~O~K"), cmOK, bfDefault));
  liste->select();
  control = TProgram::deskTop->execView(dialog);
  if (control == cmOK)
  {
    destroy(coll);
    coll = _coll;
  }
  else
    destroy(_coll);
  destroy(dialog);
}

void
OptimizationFlags()
{
  Flags(_("Optimization Flags"), Options.opt_flags, hcOptFlagList);
}

void
WarningFlags(void)
{
  Flags(_("Warning Flags"), Options.warn_flags, hcWarnFlagList);
}

void
DebugFlags(void)
{
  Flags(_("Debugging Flags"), Options.debug_flags, hcDebFlagList);
}

void
CXXFlags(void)
{
  Flags(_("C++ Flags"), Options.cxx_flags, hcCXXFlagList);
}

void
CFlags(void)
{
  Flags(_("C Flags"), Options.c_flags, hcCFlagList);
}

void
PascalFlags(void)
{
  if (UseFPC)
  {
    Flags(_("Pascal Flags"), Options.fpc_flags, hcPascalFlagList);
  }
  else
  {
    Flags(_("Pascal Flags"), Options.pascal_flags, hcPascalFlagList);
  }
}

void
FortranFlags(void)
{
  Flags(_("Fortran Flags"), Options.fortran_flags, hcFortranFlagList);
}

void
AdaFlags(void)
{
  Flags(_("Ada Flags"), Options.ada_flags, hcAdaFlagList);
}

class TArrowInputLine:public TInputLine
{
public:
  int up, down;


    TArrowInputLine(const TRect & rect, int alimit, int allowup =
                    0, int allowdown =
                    0):TInputLine(rect, alimit), up(allowup), down(allowdown)
  {
  }
  virtual void handleEvent(TEvent &);
};

void
TArrowInputLine::handleEvent(TEvent & event)
{
  TInputLine::handleEvent(event);
  switch (event.what)
  {
    case evKeyDown:
      switch (event.keyDown.keyCode)
      {
        case kbUp:
          if (up)
          {
            event.keyDown.keyCode = kbShiftTab;
            return;
          }
          break;
        case kbDown:
          if (down)
          {
            event.keyDown.keyCode = kbTab;
            return;
          }
          break;
        default:
          break;
      }
    default:
      break;
  }
}

void
Libraries(void)
{
  TDialog *dialog;
  TRect r(0, 0, 40, 23);
  TCheckBoxes *chk, *chk1;
  ushort control, flags, stdlibs;
  char data[28];
  int i, libcount = Options.user_libs->getCount();
  TInputLine *libs[16];

  dialog = new TDialog(r, _("Libraries"));
  dialog->options |= ofCentered;
  r = TRect(2, 1, 8, 17);
/* *INDENT-OFF* */
  chk = new TCheckBoxes(r, new TSItem("~0~",
                           new TSItem("~1~",
                           new TSItem("~2~",
                           new TSItem("~3~",
                           new TSItem("~4~",
                           new TSItem("~5~",
                           new TSItem("~6~",
                           new TSItem("~7~",
                           new TSItem("~8~",
                           new TSItem("~9~",
                           new TSItem("~A~",
                           new TSItem("~B~",
                           new TSItem("~C~",
                           new TSItem("~D~",
                           new TSItem("~E~",
                           new TSItem("~F~",
                           NULL)))))))))))))))));
/* *INDENT-ON* */
  flags = Options.libs;
  chk->setData(&flags);
  dialog->insert(chk);
  r.a.x = r.b.x;
  r.b.x = r.a.x + 29;
  r.b.y = r.a.y + 1;
  for (i = 0; i < 16; i++)
  {
    libs[i] = new TArrowInputLine(r, 27, (i == 0 ? 0 : 1), (i == 15 ? 0 : 1));
    if (libcount > i)
    {
      strncpy(data, (char *) (Options.user_libs->at(i)), 27);
      data[27] = EOS;
      libs[i]->setData(data);
    }
    dialog->insert(libs[i]);
    r.move(0, 1);
  }

  r.a.x = 2;
  r.b.x = r.a.x + 35;
  r.b.y = r.a.y + 2;
/* *INDENT-OFF* */
  chk1 = new TCheckBoxes(r, new TSItem(_("Use ~s~tandard libraries"),
                            new TSItem(_("link for ~p~rofiling"),
                            NULL)));
/* *INDENT-ON* */
  stdlibs = NoStdLib ? 0 : 1;
  if (ForProfile)
    stdlibs |= 2;
  chk1->setData(&stdlibs);
  dialog->insert(chk1);

  r.move(0, 3);
  r.a.x = 2;
  r.b.x = r.a.x + 14;
  r.b.y = r.a.y + 2;
  dialog->insert(new TLButton(r, _("~O~K"), cmOK, bfDefault));
  libs[0]->select();
  control = TProgram::deskTop->execView(dialog);
  if (control == cmOK)
  {
    char *tmp;

    chk1->getData(&stdlibs);
    if (stdlibs & 1)
      NoStdLib = 0;
    else
      NoStdLib = 1;
    if (stdlibs & 2)
      ForProfile = 1;
    else
      ForProfile = 0;
    Options.user_libs->freeAll();
    for (i = 0; i < 16; i++)
    {
      libs[i]->getData(data);
      string_dup(tmp, data);
      Options.user_libs->insert(tmp);
    }
    chk->getData(&flags);
    Options.libs = flags;
  }
  destroy(dialog);
}

void
LinkerOptions()
{
  EditParamList(Options.link_opt, _("Additional linker options"),
                RHIDE_History_linker_options);
}

void
CompilerOptions()
{
  EditParamList(Options.comp_opt, _("Additional compiler options"),
                RHIDE_History_compiler_options);
}

class TEnvironmentDialog:public TDialog
{
public:
  TEnvironmentDialog();
  virtual void handleEvent(TEvent &);
  virtual void shutDown();
#define max_cluster 2
  TCheckBoxes *cluster[max_cluster];
#ifdef __DJGPP__
  TRadioButtons *radio;
  TInputLine *usermode;
#endif
  TIntInputLine *closed;
  TLabel *options_label;
  int current_cluster;
};

const ushort cmMoreOptions = 9999;

#define SetGetOptions()       \
    S(AllDeps, 0);             \
    S(CreateBackupFiles, 1);   \
    S(ShowSyntax, 2);          \
    S(UseDualDisplay, 3);      \
    S(ShowStderr, 4);          \
    S(ShowStdout, 5);          \
    S(ShowWhatDoing, 6);       \
    S(ShowMem, 7);             \
    S(NoFileCache, 8);         \
    S(IntenseMode, 9);         \
    S(VerboseGDB, 10);         \
    S(NoShadows, 11);          \
    S(SaveTextPalette, 12);    \
    S(SaveProjectOnlyWhenClosing, 13);\
    S(ShowUserAfterExit, 14);  \
    S(OnlyUserIncludes, 15);   \
    S(AllowDirectories, 16);   \
    S(AutomaticOpenDisass, 17);\
    S(UseRCS, 18);\
    S(UseFPC, 19);

#define SetGetOptions1()      \
    S(SaveMessages, 0);\
    S(DontShowExitCode, 1);\
    S(DeleteRecursive, 2);

TEnvironmentDialog::TEnvironmentDialog():
TDialog(TRect(0, 0, 76, 23), _("Environment options")),
TWindowInit(TEnvironmentDialog::initFrame)
{
  TRect r, r1, r2;
  char *tmp;

#ifdef __DJGPP__
  char tabstring[10];

  ushort radiodata, oldmode;
#endif
  uint32 global_options[max_cluster];

  r1 = TRect(3, 2, 49, 2 + 20);

  r1.b.y = r1.a.y + 3;
/* *INDENT-OFF* */
  cluster[1] = new TEnterCheckBoxes(r1,
                 new TSItem(_("~R~emember old messages"),   // r
                 new TSItem(_("~D~on`t show exit code"),    // d
                 new TSItem(_("~C~lean files recursive"),   // c
                 NULL))), 1);
/* *INDENT-OFF* */
  cluster[1]->helpCtx = hcPreferenceCheckbox1;

  r1.b.y = r1.a.y + 20;
/* *INDENT-OFF* */
  cluster[0] = new TEnterCheckBoxes(r1,
                new TSItem(_("all dependencies in ~m~akefile"),          // m
                new TSItem(_("create ~b~ackupfiles"),                    // b
                new TSItem(_("syntax ~h~ighlighting"),                   // h
                new TSItem(_("U~s~e dual display"),                      // s
                new TSItem(_("redirect 'stde~r~r'"),                     // r
                new TSItem(_("redirect 'stdou~t~'"),                     // t
                new TSItem(_("show ~p~rocess information"),              // p
                new TSItem(_("show free memor~y~"),                      // y
                new TSItem(_("No file cachin~g~"),                       // g
                new TSItem(_("1~6~ background colors"),                  // 6
                new TSItem(_("Show G~D~B commands"),                     // d
                new TSItem(_("~U~se no shadows"),                        // u
                new TSItem(_("Save te~x~t palette"),                     // x
                new TSItem(_("Save pro~j~ect only when closing"),        // j
                new TSItem(_("Sho~w~ user screen after exit"),           // w
                new TSItem(_("Only #~i~nclude \"...\" as dependencies"), // i
                new TSItem(_("Directories in project items"),            //
                new TSItem(_("Show Disassemb~l~er Window when needed"),  // l
                new TSItem(_("Use RCS"),                                 //
                new TSItem(_("Use ~F~PC pascal compiler"),               // f
                NULL)))))))))))))))))))),
                1);
/* *INDENT-ON* */
  cluster[0]->helpCtx = hcPreferenceCheckbox;
  memset(global_options, 0, sizeof(global_options));
#define S(x,y) if (x) global_options[0] |= (1 << (y))
  SetGetOptions();
#undef S
#define S(x,y) if (x) global_options[1] |= (1 << (y))
  SetGetOptions1();
#undef S
  tmp = _("O~p~tions");
  options_label =
    new TLabel(TRect(r1.a.x, r1.a.y - 1, r1.a.x + cstrlen(tmp) + 1, r1.a.y),
               tmp, cluster[0]);
  insert(options_label);
  insert(cluster[0]);
  current_cluster = 0;
  cluster[0]->setData(&global_options[0]);
  cluster[1]->setData(&global_options[1]);

  r2.a.x = r1.b.x + 2;
  r2.b.x = r2.a.x + 20;
  r2.a.y = r1.a.y;
  r2.b.y = r2.a.y + 13;
#ifdef __DJGPP__
/* *INDENT-OFF* */
  radio = new TEnterRadioButtons(r2, new TSItem(("8~0~x25"),
           new TSItem(("80x~2~8"),
           new TSItem(("~8~0x35"),
           new TSItem(("80x~4~0"),
           new TSItem(("80x4~3~"),
           new TSItem(("80x~5~0"),
           new TSItem(_("other"),
           new TSItem(("80x30"),
           new TSItem(("80x34"),
           new TSItem(("90x30"),
           new TSItem(("90x34"),
           new TSItem(("94x30"),
           new TSItem(("94x34"),
           NULL))))))))))))));
/* *INDENT-ON* */
  tmp = _("~S~creen mode");
  insert(new
         TLabel(TRect(r2.a.x, r2.a.y - 1, r2.a.x + cstrlen(tmp) + 1, r2.a.y),
                tmp, radio));
  insert(radio);
#endif
  r2.a.y = r2.b.y;
  r2.b.y = r2.a.y + 1;
  r2.b.x = r2.a.x + 10;
#ifdef __DJGPP__
  usermode = new TEnterInputLine(r2, 6);
  insert(usermode);
  radiodata = 0;
  oldmode = TScreen::screenMode;
  if ((oldmode & 0xFF) == 3)
    radiodata += (oldmode >> 8);
  else
    radiodata = 6;
  sprintf(tabstring, "0x%x", oldmode);
  radio->setData(&radiodata);
  radio->helpCtx = hcPreferenceVideomode;
  usermode->setData(tabstring);
  if (radiodata == 6)
    usermode->options |= ofSelectable;
  else
    usermode->options &= ~ofSelectable;
#endif

  r2.move(0, 2);
  r2.b.x = r2.a.x + 10;
  closed = new TIntInputLine(r2, 7);
  tmp = _("Closed ~W~indows");
  insert(new
         TLabel(TRect(r2.a.x, r2.a.y - 1, r2.a.x + cstrlen(tmp) + 1, r2.a.y),
                tmp, closed));
  insert(closed);
  closed->setData(&Project.max_closed_windows);
  closed->helpCtx = hcClosedWindows;

  r.a.x = r2.a.x;
  r.b.x = r.a.x + 10;
  r.a.y = r2.b.y + 0;
  r.b.y = r.a.y + 2;
  insert(new TLButton(r, _("~O~K"), cmOK, bfNormal));
  r.a.x = r.b.x + 3;
  r.b.x = r.a.x + 10;
  insert(new TLButton(r, _("Cancel"), cmCancel, bfNormal));
  r.a.x = r2.a.x;
  r.b.x = r.a.x + 23;
  r.a.y += 2;
  r.b.y += 2;
  insert(new TLButton(r, _("Mor~e~ options"), cmMoreOptions, bfNormal));

  options |= ofCentered;
  cluster[0]->select();
}

void
TEnvironmentDialog::shutDown()
{
  int i;

  remove(cluster[current_cluster]);
  for (i = 0; i < max_cluster; i++)
  {
    destroy(cluster[i]);
  }
  TDialog::shutDown();
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
    case evCommand:
      switch (event.message.command)
      {
        case cmMoreOptions:
          remove(cluster[current_cluster]);
          current_cluster++;
          if (current_cluster == max_cluster)
            current_cluster = 0;
          insert(cluster[current_cluster]);
          options_label->link = cluster[current_cluster];
          clearEvent(event);
          break;
        default:
          break;
      }
      break;
    default:
      break;
  }
}

extern TPoint shadowSize;

void
EnableShadows()
{
  if (TScreen::screenMode == TDisplay::smMono)
    return;
  shadowSize.y = 1;
  shadowSize.x = 1;
  if (2 * TScreen::screenWidth >= 5 * TScreen::screenHeight)
    shadowSize.x++;
}

void
DisableShadows()
{
  shadowSize.x = 0;
  shadowSize.y = 0;
}

void
Preferences()
{
  uint32 global_options[max_cluster];
  TEnvironmentDialog *dialog;
  int do_it = 1;

#ifdef __DJGPP__
  ushort oldmode = TScreen::screenMode;
#endif
  dialog = new TEnvironmentDialog();
  while (do_it && TProgram::deskTop->execView(dialog) == cmOK)
  {
    int syntax = ShowSyntax;

    do_it = 0;
#ifdef __DJGPP__
    char tabstring[10];
    ushort rdata;
    ushort radiodata;

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
      if (mode == 0)
        mode = oldmode;
      radiodata = mode;
    }
    else
      radiodata += (rdata << 8);
    Project.screen_mode = radiodata;
#endif
    dialog->cluster[0]->getData(&global_options[0]);
    dialog->cluster[1]->getData(&global_options[1]);
    dialog->closed->getData(&Project.max_closed_windows);
    if (Project.max_closed_windows < -1)
      Project.max_closed_windows = -1;
    max_closed = Project.max_closed_windows;
#define S(x,y) if (global_options[0] & (1 << (y))) (x) = 1; else (x) = 0
    SetGetOptions();
#undef S
#define S(x,y) if (global_options[1] & (1 << (y))) (x) = 1; else (x) = 0
    SetGetOptions1();
#undef S
#ifdef __DJGPP__
    extern int save_text_palette;

    if (SaveTextPalette)
      save_text_palette = 1;
    else
      save_text_palette = 0;
#endif
#ifdef INTERNAL_DEBUGGER
    if (VerboseGDB)
      verbose_gdb_commands = 1;
    else
      verbose_gdb_commands = 0;
#endif

    if (syntax && !ShowSyntax || !syntax && ShowSyntax)
    {
      if (ShowSyntax)
      {
        TIDEFileEditor::use_syntax = 1;
        message(TProgram::application, evBroadcast, cmTurnSyntaxOn, NULL);
      }
      else
      {
        TIDEFileEditor::use_syntax = 0;
        message(TProgram::application, evBroadcast, cmTurnSyntaxOff, NULL);
      }
    }
#ifdef __DJGPP__
    SetProjectScreenMode();
#endif
    Repaint();
#ifdef __DJGPP__
    if (TScreen::screenMode != radiodata)
    {
      messageBox(mfError | mfOKButton,
                 _("This video mode (0x%03x) is not supported by the "
                   "Turbo Vision library"), radiodata);
      do_it = 1;
      dialog->usermode->select();
    }
#endif
  }
  destroy(dialog);
}

class MyDialog:public TDialog
{
public:
  MyDialog(const char *, Boolean);
  virtual void handleEvent(TEvent &);
};

const ushort
  cmAddEntry = 9999,
  cmDelEntry = 9998,

  cmEditEntry = 9997, cmDefaultEntry = 9996, cmFileEntry = 9995;

MyDialog::MyDialog(const char *tit, Boolean with_default):
TDialog(TRect(0, 0, 60, 21), tit), TWindowInit(MyDialog::initFrame)
{
  TRect r;

  options |= ofCentered;
  r = getExtent();
  r.a.x += 3;
  r.b.x = r.a.x + 10;
  r.b.y -= 3;
  r.a.y = r.b.y - 2;
  insert(new TLButton(r, _("~O~K"), cmOK, bfNormal));
  r.a.x = r.b.x + 1;
  insert(new TLButton(r, _("~A~dd"), cmAddEntry, bfNormal));
  r.a.x = r.b.x + 1;
  insert(new TLButton(r, _("~D~elete"), cmDelEntry, bfNormal));
  r.a.x = r.b.x + 1;
  insert(new TLButton(r, _("~E~dit"), cmEditEntry, bfDefault));
  r.a.x = r.b.x + 1;
  insert(new TLButton(r, _("Cancel"), cmCancel, bfNormal));
  r.move(0, 2);
  r.a.x = size.x / 2 - 15;
  r.b.x = r.a.x + 10;
  if (with_default == True)
    insert(new TLButton(r, _("Defa~u~lt"), cmDefaultEntry, bfNormal));
  r.move(15, 0);
  r.b.x = r.a.x + 10;
  insert(new TLButton(r, _("~F~ile"), cmFileEntry, bfNormal));
}

void
MyDialog::handleEvent(TEvent & event)
{
  TDialog::handleEvent(event);
  if (event.what == evCommand)
  {
    switch (event.message.command)
    {
      case cmAddEntry:
      case cmDelEntry:
      case cmEditEntry:
      case cmDefaultEntry:
      case cmFileEntry:
        endModal(event.message.command);
        clearEvent(event);
        return;
      default:
        break;
    }
  }
}

static void
UpdateListBox(TListBox * box, TCollection * list)
{
  ccIndex Count;

  Count = list->getCount();
  box->setRange(Count);
  if (box->focused >= Count)
  {
    if (Count > 0)
      box->focusItem(Count - 1);
  }
  else
  {
    box->focusItem(box->focused);
  }
  box->drawView();
}

static ushort
EditStringList(const char *title, TSCollection * list,
               Boolean with_default = True)
{
  MyDialog *dialog;
  TListBox *box;
  TScrollBar *scrollbar;
  TRect r;
  ushort result;
  char buffer[256];

  dialog = new MyDialog(title, with_default);
  r = dialog->getExtent();
  r.a.x = r.b.x - 1;
  r.a.y++;
  r.b.y -= 6;
  scrollbar = new TScrollBar(r);
  r = dialog->getExtent();
  r.grow(-1, -1);
  r.b.y -= 5;
  box = new TListBox(r, 1, scrollbar);
  box->newList(list);
  dialog->insert(scrollbar);
  dialog->insert(box);
  TProgram::deskTop->insert(dialog);
  do
  {
    dialog->setState(sfModal, True);
    result = dialog->execute();
    switch (result)
    {
      case cmDelEntry:
        if (list->getCount() == 0)
          break;
        list->atFree(box->focused);
        UpdateListBox(box, list);
        break;
      case cmAddEntry:
        buffer[0] = 0;
        if (inputBox(_("New value"), _("~V~alue"), buffer, 255) == cmOK)
        {
          if (list->getCount() == 0)
            list->insert(newStr(buffer));
          else
            list->atInsert(box->focused, newStr(buffer));
          UpdateListBox(box, list);
        }
        break;
      case cmEditEntry:
        if (list->getCount() == 0)
          break;
        strcpy(buffer, (char *) (list->at(box->focused)));
        if (inputBox(_("New value"), _("~V~alue"), buffer, 255) == cmOK)
        {
          int foc = box->focused;

          list->atFree(foc);
          list->atInsert(foc, newStr(buffer));
          UpdateListBox(box, list);
        }
        break;
      default:
        break;
    }
  }
  while (result != cmCancel && result != cmOK && result != cmDefaultEntry
         && result != cmFileEntry);
  TProgram::deskTop->remove(dialog);
  destroy(dialog);
  return result;
}

static void
EditWords(TStringCollection * Words, int &words_changed,
          Boolean with_default, const char *title,
          void (*DefaultWords) (TProject *))
{
  TSCollection *list;
  int i;
  ushort result;

  do
  {
    list = new TSCollection();
    for (i = 0; i < Words->getCount(); i++)
    {
      list->insert(newStr((char *) (Words->at(i))));
    }
    result = EditStringList(title, list, with_default);
    if (result == cmOK)
    {
      Words->freeAll();
      for (i = 0; i < list->getCount(); i++)
      {
        Words->insert(newStr((char *) (list->at(i))));
      }
      words_changed = 1;
    }
    destroy(list);
    switch (result)
    {
      case cmDefaultEntry:
        if (DefaultWords)
          DefaultWords(NULL);
        break;
      case cmFileEntry:
      {
        TFileDialog *d;
        char name[256];

        InitHistoryID(RHIDE_History_reserved_words);
        d = new TFileDialog("*.*", _("Read words from file"), _("~N~ame"),
                            fdOpenButton, RHIDE_History_reserved_words);
        if (TProgram::deskTop->execView(d) != cmCancel)
        {
          d->getData(name);
          int handle = open(name, O_RDONLY);

          if (handle > 0)
          {
            char *buffer;
            struct stat st;
            fstat(handle, &st);
            long len = st.st_size;

            buffer = (char *) malloc(len + 1);
            len =::read(handle, buffer, len);
            if (len < 0)
              len = 0;
            buffer[len] = 0;
            close(handle);
            Words->freeAll();
            char *tok;

            for (tok = strtok(buffer, " \t\r\n"); tok;
                 tok = strtok(NULL, " \t\r\n"))
            {
              Words->insert(strdup(tok));
            }
            free(buffer);
          }
          else
          {
            messageBox(mfError | mfOKButton, _("Couldn't open file %s"),
                       name);
          }
        }
        break;
      }
      default:
        break;
    }
  }
  while (result == cmDefaultEntry || result == cmFileEntry);
}

void
EditReserved()
{
  EditWords(CReservedWords, c_words_changed, True, _("Reserved words"),
            DefaultReservedWords);
}

void
EditGPCReserved()
{
  if (!UseFPC)
  {
    EditWords(GPCReservedWords, gpc_words_changed, True,
              _("GPC Reserved words"), DefaultGPCReservedWords);
  }
  else
  {
    EditWords(FPCReservedWords, fpc_words_changed, True,
              _("FPC Reserved words"), DefaultFPCReservedWords);
  }
}

void
EditUserWords()
{
  EditWords(RHIDEUserWords, user_words_changed, True,
            _("User Reserved words"), NULL);
}

static TSCollection *
BuildStringList(TFlagCollection & coll)
{
  int i;
  TSCollection *list;

  list = new TSCollection();
  for (i = 0; i < coll.getCount(); i++)
  {
    list->insert(((TFlagEntry *) (coll.at(i)))->GetControlString());
  }
  return list;
}

static void
BuildFlagList(TFlagCollection & coll, TSCollection * list)
{
  int i;

  coll.freeAll();
  for (i = 0; i < list->getCount(); i++)
  {
    coll.insert(new TFlagEntry((char *) (list->at(i))));
  }
}

static void
EditFlags(const char *title, TFlagCollection & coll,
          void (*default_function) ())
{
  TSCollection *list;
  ushort result;

  do
  {
    list = BuildStringList(coll);
    result = EditStringList(title, list);
    if (result == cmOK)
    {
      BuildFlagList(coll, list);
    }
    destroy(list);
    if (result == cmDefaultEntry)
    {
      default_function();
    }
  }
  while (result == cmDefaultEntry);
}

void
EditCFlags()
{
  EditFlags(_("C Flags"), *(Options.c_flags), DefaultCFlags);
}

void
EditCXXFlags()
{
  EditFlags(_("C++ Flags"), *(Options.cxx_flags), DefaultCXXFlags);
}

void
EditDebugFlags()
{
  EditFlags(_("Debugging Flags"), *(Options.debug_flags), DefaultDebugFlags);
}

void
EditWarnFlags()
{
  EditFlags(_("Warning Flags"), *(Options.warn_flags), DefaultWarnFlags);
}

void
EditOptFlags()
{
  EditFlags(_("Optimization Flags"), *(Options.opt_flags), DefaultOptFlags);
}

void
EditPascalFlags()
{
  if (UseFPC)
  {
    EditFlags(_("Pascal Flags"), *(Options.fpc_flags), DefaultFpcFlags);
  }
  else
  {
    EditFlags(_("Pascal Flags"), *(Options.pascal_flags), DefaultPascalFlags);
  }
}

void
EditFortranFlags()
{
  EditFlags(_("Fortran Flags"), *(Options.fortran_flags),
            DefaultFortranFlags);
}

void
EditAdaFlags()
{
  EditFlags(_("Ada Flags"), *(Options.ada_flags), DefaultAdaFlags);
}

class TLocalDialog:public TDialog
{
public:
  TLocalDialog();
  virtual void handleEvent(TEvent &);
  TEnterInputLine *cmdline;
  TEnterRadioButtons *compiler_id;
  TEnterRadioButtons *syntax_type;
  TEnterInputLine *compiler;
  TEnterRadioButtons *error_type;
  TEnterInputLine *error_prog;
  TEnterInputLine *output_name;
  TEnterCheckBoxes *others;
};

TLocalDialog::TLocalDialog():
TDialog(TRect(0, 0, 69, 25), _("local options for project-item")),
TWindowInit(&TLocalDialog::initFrame)
{
  TRect r, rr, rrr;
  char *tmp;

  r = TRect(2, 2, 32, 3);
  cmdline = new TEnterInputLine(r, 255);
  insert(new THistory(TRect(r.b.x, r.a.y, r.b.x + 3, r.b.y), cmdline,
                      RHIDE_History_local_defines));
  r.move(0, -1);
  tmp = _("local ~c~ommandline options");
  r.b.x = r.a.x + cstrlen(tmp) + 1;
  insert(new TLabel(r, tmp, cmdline));
  cmdline->helpCtx = hcLocalCommandline;
  insert(cmdline);
  r.move(0, 3);
  r.b.x = r.a.x + 30;
  output_name = new TEnterInputLine(r, 255);
  insert(new THistory(TRect(r.b.x, r.a.y, r.b.x + 3, r.b.y), output_name,
                      RHIDE_History_local_outfile));
  r.move(0, -1);
  tmp = _("~N~ame of the output file");
  r.b.x = r.a.x + cstrlen(tmp) + 1;
  insert(new TLabel(r, tmp, output_name));
  output_name->helpCtx = hcLocalOutputname;
  insert(output_name);
  rrr = r;
  r = TRect(35, 2, 65, 2 + 9);
/* *INDENT-OFF* */
  compiler_id = new TEnterRadioButtons(r,
                    new TSItem(_("Auto"),
                    new TSItem(_("User"),
                    new TSItem(_("None"),
                    new TSItem(_("GNU C compiler"),
                    new TSItem(_("GNU C++ compiler"),
                    new TSItem(_("GNU assembler"),
                    new TSItem(_("GNU Pascal compiler"),
                    new TSItem(_("GNU Fortran compiler"),
                    new TSItem(_("Free Pascal compiler"),
                    NULL))))))))));
/* *INDENT-ON* */
  rr = r;
  r.a.y--;
  r.b.y = r.a.y + 1;
  tmp = _("Comp~i~ler type");
  r.b.x = r.a.x + cstrlen(tmp) + 1;
  insert(new TLabel(r, tmp, compiler_id));
  compiler_id->helpCtx = hcLocalCompilertype;
  insert(compiler_id);
  rr.a.y = rr.b.y + 1;
  rr.b.y = rr.a.y + 1;
  rr.b.x = rr.a.x + 30;
  compiler = new TEnterInputLine(rr, 255);
  insert(new THistory(TRect(rr.b.x, rr.a.y, rr.b.x + 3, rr.b.y), compiler,
                      RHIDE_History_local_compiler_prog));
  rr.move(0, -1);
  tmp = _("Com~p~iler");
  rr.b.x = rr.a.x + cstrlen(tmp) + 1;
  insert(new TLabel(rr, tmp, compiler));
  compiler->helpCtx = hcLocalCompiler;
  insert(compiler);
  r.a.x = rr.a.x;
  r.a.y = rr.b.y + 2;
  r.b.y = r.a.y + 8;
  r.b.x = r.a.x + 30;
/* *INDENT-OFF* */
  error_type = new TEnterRadioButtons(r, new TSItem(_("Auto"),
                                         new TSItem(_("User"),
                                         new TSItem(_("None"),
                                         new TSItem(_("builtin C"),
                                         new TSItem(_("builtin assembler"),
                                         new TSItem(_("builtin linker"),
                                         new TSItem(_("return value"),
                                         new TSItem(_("builtin FPC"),
                                         NULL)))))))));
/* *INDENT-ON* */
  rr = r;
  r.move(0, -1);
  tmp = _("~E~rror checking");
  r.b.x = r.a.x + cstrlen(tmp) + 1;
  r.b.y = r.a.y + 1;
  insert(new TLabel(r, tmp, error_type));
  error_type->helpCtx = hcLocalErrortype;
  insert(error_type);
  r.a.y = rr.b.y + 1;
  r.b.y = r.a.y + 1;
  r.b.x = r.a.x + 30;
  error_prog = new TEnterInputLine(r, 255);
  insert(new THistory(TRect(r.b.x, r.a.y, r.b.x + 3, r.b.y), error_prog,
                      RHIDE_History_local_error_prog));
  r.move(0, -1);
  tmp = _("E~r~ror program");
  r.b.x = r.a.x + cstrlen(tmp) + 1;
  insert(new TLabel(r, tmp, error_prog));
  error_prog->helpCtx = hcLocalErrorprogram;
  insert(error_prog);

  r.a.y = rrr.b.y + 2;
  r.b.y = r.a.y + 2;
  r.a.x = rrr.a.x;
  r.b.x = r.a.x + 30;
/* *INDENT-OFF* */
  others = new TEnterCheckBoxes(r, new TSItem(_("exclude from link"),
                                   new TSItem(_("rebuild seldom"),
                                   NULL)));
/* *INDENT-ON* */
  r.move(0, -1);
  tmp = _("o~t~her options");
  r.b.x = r.a.x + cstrlen(tmp) + 1;
  insert(new TLabel(r, tmp, others));
  others->helpCtx = hcLocalOtheroptions;
  insert(others);

  r = getExtent();
  r.a.y = r.b.y - 3;
  r.b.y = r.a.y + 2;
  r.a.x = 3;
  r.b.x = r.a.x + 10;
  insert(new TLButton(r, _("~O~K"), cmOK, bfNormal));
  r.a.x = r.b.x + 3;
  r.b.x = r.a.x + 10;
  insert(new TLButton(r, _("Cancel"), cmCancel, bfNormal));
  options |= ofCentered;
  cmdline->select();
}

void
TLocalDialog::handleEvent(TEvent & event)
{
  TRadioButtons *radio;

  TDialog::handleEvent(event);
  switch (event.what)
  {
    case evBroadcast:
      switch (event.message.command)
      {
        case cmRadioItemSelected:
          radio = (TRadioButtons *) event.message.infoPtr;
          ushort data;

          radio->getData(&data);
          if (radio == error_type)
          {
            if (data == 1)
              error_prog->options |= ofSelectable;
            else
              error_prog->options &= ~ofSelectable;
            clearEvent(event);
            break;
          }
          if (radio == compiler_id)
          {
            if (data == 1)
            {
              compiler->options |= ofSelectable;
            }
            else
            {
              compiler->options &= ~ofSelectable;
            }
            clearEvent(event);
            break;
          }
          break;
        default:
          break;
      }
      break;
    default:
      break;
  }
}

static void
SetOptions(TLocalDialog * dialog, TDependency * dep)
{
  char *tmp;

  dep->local_options->ToString(tmp);
  dialog->cmdline->setData(tmp);
  string_free(tmp);
  dialog->output_name->setData((char *) FName(dep->dest_name));
  dialog->compiler_id->setData(&dep->compiler_type);
  if (dep->compiler)
    dialog->compiler->setData(dep->compiler);
  if (dep->compiler_type == COMPILER_USER)
    dialog->compiler->options |= ofSelectable;
  else
    dialog->compiler->options &= ~ofSelectable;
  dialog->error_type->setData(&dep->error_type);
  if (dep->error_check)
    dialog->error_prog->setData(dep->error_check);
  if (dep->error_type == ERROR_USER)
    dialog->error_prog->options |= ofSelectable;
  else
    dialog->error_prog->options &= ~ofSelectable;
  dialog->others->setData(&dep->flags);
  if (dep->compile_id == COMPILE_PROJECT)
    dialog->output_name->options &= ~ofSelectable;
}

static Boolean
GetOptions(TLocalDialog * dialog, TDependency * dep)
{
  Boolean retval;
  TParamList *pl = new TParamList();

  retval = pl->FromString(dialog->cmdline->data);
  destroy(pl);
  if (retval == False)
    return False;
  dep->local_options->FromString(dialog->cmdline->data);
  if (dep->compile_id != COMPILE_PROJECT)
  {
    if (dep->dest_name)
      delete dep->dest_name;
    char *tmp;

    BaseName(dialog->output_name->data, tmp);
    InitFName(dep->dest_name, tmp);
    string_free(tmp);
    dep->dest_file_type = get_file_type(FName(dep->dest_name));
  }
  dialog->compiler_id->getData(&dep->compiler_type);
  switch (dep->compiler_type)
  {
    case COMPILER_AUTO:
      dep->compile_id =
        how_to_compile(FName(dep->source_name), FName(dep->dest_name));
      break;
    case COMPILE_NONE:
      dep->compile_id = COMPILE_NONE;
      break;
    case COMPILER_USER:
      dep->compile_id = COMPILE_USER;
      break;
    case COMPILER_C:
      dep->compile_id = COMPILE_C;
      break;
    case COMPILER_CC:
      dep->compile_id = COMPILE_CC;
      break;
    case COMPILER_ASM:
      dep->compile_id = COMPILE_ASM;
      break;
    case COMPILER_GPC:
      dep->compile_id = COMPILE_PASCAL;
      break;
    case COMPILER_FPC:
      dep->compile_id = COMPILE_FPC;
      break;
    case COMPILER_FORTRAN:
      dep->compile_id = COMPILE_FORTRAN;
      break;
    default:
      dep->compile_id = COMPILE_UNKNOWN;
  }
  string_free(dep->compiler);
  string_dup(dep->compiler, dialog->compiler->data);
  string_free(dep->error_check);
  string_dup(dep->error_check, dialog->error_prog->data);
  dialog->error_type->getData(&dep->error_type);
  dialog->others->getData(&dep->flags);
  return True;
}

void
LocalOptions()
{
  TDependency *dep;
  ushort result;
  Boolean retval;

  if (!project_window)
    return;
  if (project->dependencies->getCount() == 0)
    return;
  dep =
    (TDependency *) (project->
                     dependencies->at(project_window->liste->focused));
  TLocalDialog *dialog = new TLocalDialog();

  SetOptions(dialog, dep);
  do
  {
    result = TProgram::deskTop->execView(dialog);
    if (result == cmOK)
    {
      retval = GetOptions(dialog, dep);
    }
    else
    {
      retval = False;
    }
  }
  while (result == cmOK && retval == False);
  destroy(dialog);
}

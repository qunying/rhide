/* Copyright (C) 1996-2000 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#include <string.h>
#define Uses_TRect
#define Uses_TColorDialog
#define Uses_TColorGroup
#define Uses_TColorItem
#define Uses_TPalette
#define Uses_TProgram
#define Uses_TDeskTop
#define Uses_TIDEFileEditor
#include <libide.h>

#include "rhide.h"

static TPalette *temp_pal = NULL;

#if 0
class _TColorItem:public TColorItem
{
public:
  _TColorItem(const char *nm, uchar idx);
};

_TColorItem::_TColorItem(const char *nm, uchar idx):
TColorItem(_(nm), idx)
{
}

static TColorGroup &
windows_colors(char *name, int base)
{
  return
    *new TColorGroup(name)
    + *new _TColorItem(__("Frame disabled"), base)
    + *new _TColorItem(__("Frame"), base + 0x01)
    + *new _TColorItem(__("Frame icons"), base + 0x02)
    + *new _TColorItem(__("Scroll bar page"), base + 0x03)
    + *new _TColorItem(__("Scroll bar icons"), base + 0x04)
    + *new _TColorItem(__("Static text"), base + 0x05)
    + *new _TColorItem(__("Selected text"), base + 0x06)
#if 0
    + *new _TColorItem(__("reserved"), base + 0x07)
#endif
    ;
}

static TColorGroup &
editor_colors(char *name, int base)
{
  return
    *new TColorGroup(name)
    + *new _TColorItem(__("Frame disabled"), base)
    + *new _TColorItem(__("Frame/background"), base + 0x01)
    + *new _TColorItem(__("Frame icons"), base + 0x02)
    + *new _TColorItem(__("Scroll bar page"), base + 0x03)
    + *new _TColorItem(__("Scroll bar icons"), base + 0x04)
    + *new _TColorItem(__("normal text"), base + 0x05)
    + *new _TColorItem(__("marked text"), base + 0x06)
    + *new _TColorItem(__("comment"), base + 0x07)
    + *new _TColorItem(__("reserved word"), base + 0x08)
    + *new _TColorItem(__("identifier"), base + 0x09)
    + *new _TColorItem(__("symbol"), base + 0x0A)
    + *new _TColorItem(__("string"), base + 0x0B)
    + *new _TColorItem(__("integer"), base + 0x0C)
    + *new _TColorItem(__("float"), base + 0x0D)
    + *new _TColorItem(__("octal"), base + 0x0E)
    + *new _TColorItem(__("hex"), base + 0x0F)
    + *new _TColorItem(__("character"), base + 0x10)
    + *new _TColorItem(__("preprocessor"), base + 0x11)
    + *new _TColorItem(__("illegal char"), base + 0x12)
    + *new _TColorItem(__("user defined words"), base + 0x13)
    + *new _TColorItem(__("CPU line"), base + 0x14)
    + *new _TColorItem(__("Breakpoint"), base + 0x15)
    + *new _TColorItem(__("symbol2"), base + 0x16)
    + *new _TColorItem(__("Cross cursor"), base + 0x17)
    + *new _TColorItem(__("editor statusline"), base + 0x18)
    + *new _TColorItem(__("parens matching"), base + 0x19)
    + *new _TColorItem(__("rectangle block"), base + 0x1A);
}
#else

static void
addItem(TColorGroup * &group, int index, const char *name,
        const char *group_name)
{
  if (strcmp(name, "reserved") == 0)
    return;
  if (*group_name)
  {
    if (!group)
      group = new TColorGroup(_(group_name));
    else
      *group = *group + *new TColorGroup(_(group_name));
  }
  *group = *group + *new TColorItem(_(name), index);
}

#include <pal.h>
#undef S
#undef S_
#define S(index,foreground,background,name,comment...) \
  addItem(group,0x##index,#name,"");
#define S_(index,foreground,background,name,_group,comment...) \
  addItem(group,0x##index,#name,#_group);
#endif

static TColorDialog *
GetColorDialog()
{
  TColorDialog *c;

  TColorGroup *group = NULL;

  cpIDEColor
    c = new TColorDialog(&TProgram::application->getPalette(), group);
  if (temp_pal)
    delete temp_pal;

  temp_pal = new TPalette(TProgram::application->getPalette());
  c->setData(&TProgram::application->getPalette());
  return c;
}

void
Colors()
{
  TColorDialog *c = GetColorDialog();

  if (TProgram::application->validView(c) != 0)
  {
    if (TProgram::deskTop->execView(c) == cmCancel)
    {
      // restore the old palette
      TProgram::application->getPalette() = *temp_pal;
    }
    // force to reread the chached colors for the editor
    TIDEFileEditor::colorsCached = 0;

    Repaint();
    destroy(c);
  }
}

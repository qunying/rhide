/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#include <rhutils.h>

#define Uses_TFlagListBox
#define Uses_TFlagEntry
#define Uses_TFlagCollection
#define Uses_ideCommands
#include <libide.h>
#include <string.h>
#include <stdio.h>

TFlagListBox::TFlagListBox(const TRect & bounds, ushort aNumCols,
TScrollBar * aScrollBar):
TListBox(bounds, aNumCols, aScrollBar)
{
  helpCtx = hcFlagListBox;
}

void
TFlagListBox::getText(char *dest, ccIndex item, short maxlen)
{
  char *tmp = ((TFlagEntry *) list()->at(item))->GetText();

  strncpy(dest, tmp, maxlen);
  string_free(tmp);
}

void
TFlagListBox::selectItem(ccIndex item)
{
  TListBox::selectItem(item);
  ((TFlagEntry *) list()->at(item))->ToggleActive();
  drawView();
}

/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#define Uses_TRect
#define Uses_TLButton
#include <libtvuti.h>

TLButton::TLButton(TRect & bounds, const char *aTitle, ushort aCommand,
                   ushort aFlags):
TButton(bounds, aTitle, aCommand, aFlags)
{
  int len = cstrlen(aTitle);

  if (len > size.x - 2)
  {
    bounds.b.x = bounds.a.x + len + 3;
    locate(bounds);
  }
}

/* Copyright (C) 1996-2000 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#define Uses_TDialog
#define Uses_TProgram
#define Uses_TApplication
#define Uses_TDeskTop

#define Uses_tvutilFunctions
#include <libtvuti.h>

ushort
execDialog(TDialog * d, void *data)
{
  TView *p = TProgram::application->validView(d);

  if (p == 0)
    return cmCancel;
  else
  {
    if (data != 0)
      p->setData(data);
    ushort result = TProgram::deskTop->execView(p);

    if (result != cmCancel && data != 0)
      p->getData(data);
    destroy(p);
    return result;
  }
}

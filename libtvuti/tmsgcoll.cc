/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#define Uses_TMsgCollection
#define Uses_MsgRec
#include <libtvuti.h>

void
TMsgCollection::freeItem(void *item)
{
  delete((MsgRec *) item);
}

/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#define Uses_TGroup
#define Uses_TEvent
#define Uses_TKeys

#define Uses_TEnterCheckBoxes
#define Uses_tvutilCommands
#include <libtvuti.h>

void
TEnterCheckBoxes::press(int item)
{
  TCheckBoxes::press(item);
  message(owner, evBroadcast, cmCheckBoxItemSelected, this);
}

void
TEnterCheckBoxes::movedTo(int item)
{
  TCheckBoxes::movedTo(item);
  message(owner, evBroadcast, cmCheckBoxItemSelected, this);
}

void
TEnterCheckBoxes::handleEvent(TEvent & event)
{
  TCheckBoxes::handleEvent(event);
  if (!(state & sfSelected))
    return;
  switch (event.what)
  {
    case evKeyDown:
      switch (event.keyDown.keyCode)
      {
        case kbEnter:
          event.keyDown.keyCode = kbTab;
          break;
        default:
          break;
      }
      break;
    default:
      break;
  }
}

uint32
TEnterCheckBoxes::dataSize()
{
  return big_check ? sizeof(uint32) : sizeof(ushort);
}

/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#define Uses_TGroup
#define Uses_TEvent
#define Uses_TKeys

#define Uses_TEnterRadioButtons
#define Uses_tvutilCommands
#include <libtvuti.h>

void
TEnterRadioButtons::press(int item)
{
  TRadioButtons::press(item);
  message(owner, evBroadcast, cmRadioItemSelected, this);
}

void
TEnterRadioButtons::movedTo(int item)
{
  TRadioButtons::movedTo(item);
  message(owner, evBroadcast, cmRadioItemSelected, this);
}

void
TEnterRadioButtons::handleEvent(TEvent & event)
{
  TRadioButtons::handleEvent(event);
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
TEnterRadioButtons::dataSize()
{
  return big_radio ? sizeof(uint32) : sizeof(ushort);
}

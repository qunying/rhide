/* Copyright (C) 1996-2000 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#define Uses_TEvent
#define Uses_TKeys

#define Uses_TEnterInputLine
#include <libtvuti.h>

void
TEnterInputLine::handleEvent(TEvent & event)
{
  TInputLine::handleEvent(event);
  if (!handle_enter)
    return;
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

/* Copyright (C) 1996-2000 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#if !defined( __TEnterInputLine )
#define __TEnterInputLine

class TRect;

class TEnterInputLine:public TInputLine
{
public:
  TEnterInputLine(const TRect & bounds, int aMaxlen):TInputLine(bounds,
                                                                aMaxlen),
    handle_enter(true)
  {
  }
  virtual void handleEvent(TEvent &);
  bool handle_enter;
};

#endif

/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#if !defined( __TFunctionInputLine )
#define __TFunctionInputLine

class TEvent;

class TFunctionInputLine:public TEnterInputLine
{
public:
  TFunctionInputLine(const TRect & bounds, int aMaxLen);
  virtual void handleEvent(TEvent & event);
};

#endif

/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
/*
 $Id$
*/
#if !defined( __TEnterInputLine )
#define __TEnterInputLine

class TRect;

class TEnterInputLine : public TInputLine
{
public:
  TEnterInputLine(const TRect &bounds,int aMaxlen) :
    TInputLine(bounds,aMaxlen) {}
  virtual void handleEvent(TEvent &);
};

#endif

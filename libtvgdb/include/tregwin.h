/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#if !defined( __TRegisterWindow )
#define __TRegisterWindow

class TRect;
class TEvent;

class TRegisterWindow:public TWindow
{
public:
  TRegisterWindow(const TRect &);
  void update();
  virtual void handleEvent(TEvent &);
};

#endif

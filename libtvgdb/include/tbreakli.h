/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#if !defined( __TBreakListBox )
#define __TBreakListBox

class TBreakListBox:public TEnterListBox
{
public:
  TBreakListBox(const TRect &, TScrollBar *);
  virtual void getText(char *dest, ccIndex item, short maxLen);
  void update();
  int filesize;
  int linesize;
  int condsize;
  int countsize;
  char delemiter;
};

#endif

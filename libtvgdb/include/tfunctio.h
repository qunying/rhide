/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#if !defined( __TFunctionListBox )
#define __TFunctionListBox

class TFunctionListBox:public TSortedListBox
{
public:
  TFunctionListBox(const TRect & bounds, ushort aNumCols,
                   TScrollBar * aScrollBar, const char *regex);
  virtual void getText(char *, ccIndex, short);
  virtual void handleEvent(TEvent & event);
  virtual void selectItem(ccIndex item);
};

#endif

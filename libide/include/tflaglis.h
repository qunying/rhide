/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#if defined( Uses_TFlagListBox ) && !defined( __TFlagListBox__ )
#define __TFlagListBox__

class TFlagListBox:public TListBox
{
public:
  TFlagListBox(const TRect & bounds, ushort aNumCols,
               TScrollBar * aScrollBar);
  virtual void selectItem(ccIndex);
  virtual void getText(char *, ccIndex, short);
};

#endif

/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#if defined( Uses_TProjectListBox ) && !defined( __TProjectListBox__ )
#define __TProjectListBox__

class TProjectListBox:public TSortedListBox
{
public:
  TProjectListBox(const TRect & bounds, ushort aNumCols,
                  TScrollBar * aScrollBar);
  virtual void handleEvent(TEvent &);
  virtual void selectItem(ccIndex item);
  virtual void getText(char *dest, ccIndex item, short maxLen);
};

#endif

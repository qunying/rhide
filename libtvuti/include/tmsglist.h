/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#if defined( Uses_TMsgListBox ) && !defined( __TMsgListBox__ )
#define __TMsgListBox__

class TMsgListBox:public TEnterListBox
{
public:
  TMsgListBox(const TRect & bounds, ushort aNumCols, TScrollBar * aScrollBar);
  virtual void handleEvent(TEvent & event);
  virtual void getText(char *dest, ccIndex item, short maxLen);
  const char *getText(ccIndex item);
  virtual void focusItem(ccIndex item);
  virtual void selectItem(ccIndex item);
  virtual void shutDown();
  virtual void setState(ushort, Boolean);
};

#endif

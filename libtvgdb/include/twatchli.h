/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#if !defined( __TWatchListBox )
#define __TWatchListBox

class TRect;
class TScrollBar;
class TEvent;

class TWatchListBox:public TEnterListBox
{
public:
  TWatchListBox(const TRect & bounds, int numCols, TScrollBar * vbar,
                TScrollBar * hbar = NULL);
   ~TWatchListBox();
  virtual void selectItem(ccIndex item);
  virtual void handleEvent(TEvent & event);
  void update();
  void GetVariable(ccIndex item, char *buffer, int maxlen);
  void SetVariable(ccIndex item, char *buffer);
  void AddVariable(char *var);
  void RemoveVariable(ccIndex item);
  virtual void getText(char *dest, ccIndex index, short maxlen);
};

#endif

/* Copyright (C) 1996-2000 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#if !defined( __TEnterListBox )
#define __TEnterListBox

class TRect;
class TScrollBar;
class TNSCollection;

class TEnterListBox:public TListViewer
{
public:
  TEnterListBox(const TRect & bounds, ushort numcols,
                TScrollBar * bar):TListViewer(bounds, numcols, NULL, bar),
    items(NULL)
  {
  }
  virtual void handleEvent(TEvent &);
  virtual void focusItem(ccIndex);
  virtual void newList(TNSCollection * aList);
  virtual TNSCollection *swapList(TNSCollection * aList);

  TNSCollection *list()
  {
    return items;
  }

private:

    TNSCollection * items;

};

#endif

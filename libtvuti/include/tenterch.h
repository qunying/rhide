/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#if !defined( __TEnterCheckBoxes )
#define __TEnterCheckBoxes

class TEnterCheckBoxes:public TCheckBoxes
{
public:
  TEnterCheckBoxes(const TRect & bounds, TSItem * aStrings,
                   int _big_check = 0):TCheckBoxes(bounds, aStrings),
    big_check(_big_check)
  {
  }
  virtual void handleEvent(TEvent &);
  virtual void press(int);
  virtual void movedTo(int);
  virtual uint32 dataSize();

protected:
  int big_check;
};

#endif

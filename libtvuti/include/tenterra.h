/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#if !defined( __TEnterRadioButtons )
#define __TEnterRadioButtons

class TEnterRadioButtons:public TRadioButtons
{
public:
  TEnterRadioButtons(const TRect & bounds, TSItem * aStrings,
                     int _big_radio = 0):TRadioButtons(bounds, aStrings),
    big_radio(_big_radio)
  {
  }
  virtual void handleEvent(TEvent &);
  virtual void press(int);
  virtual void movedTo(int);
  virtual uint32 dataSize();

protected:
  int big_radio;
};

#endif

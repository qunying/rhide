/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#if !defined( __TEnterRadioButtons )
#define __TEnterRadioButtons

class TEnterRadioButtons:public TRadioButtons
{
public:
  TEnterRadioButtons(const TRect & bounds,
                     TSItem * aStrings):TRadioButtons(bounds, aStrings)
  {
  }
  virtual void handleEvent(TEvent &);
  virtual void movedTo(int);

  STREAMIO(TEnterRadioButtons);
};

STREAMINLINES(TEnterRadioButtons)
#endif

/* Copyright (C) 1996-1998 Robert H�hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#if !defined( __TWatchDialog )
#define __TWatchDialog

class TRect;
class TInputLine;

class TWatchDialog : public TDialog
{
public:
  TWatchDialog(const TRect & bounds, char *Title, char *StartVal = NULL,
               int evaluate = 1);
  virtual void handleEvent(TEvent &);
  TInputLine *input;
  TInputLine *result;
  TInputLine *newval;
};

#endif

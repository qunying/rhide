/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#if !defined( __TWatchDialog )
#define __TWatchDialog

class TRect;
class TInputLine;

class TInspectDialog:public TDialog
{
public:
  TInspectDialog(const TRect & bounds, char *Title, char *StartVal = NULL);
  virtual void handleEvent(TEvent &);
  TInputLine *input;
};

#endif
